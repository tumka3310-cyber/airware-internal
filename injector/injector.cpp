// injector — Standoff 2 0.39.4, arm64-v8a
// usage: injector <pid> <absolute-so-path>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <dlfcn.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <elf.h>

struct user_pt_regs {
    uint64_t regs[31];
    uint64_t sp;
    uint64_t pc;
    uint64_t pstate;
};

static long peek(pid_t pid, uintptr_t addr) {
    errno = 0;
    return ptrace(PTRACE_PEEKDATA, pid, (void*)addr, nullptr);
}
static void poke(pid_t pid, uintptr_t addr, long val) {
    ptrace(PTRACE_POKEDATA, pid, (void*)addr, (void*)val);
}
static bool write_mem(pid_t pid, uintptr_t addr, const void* buf, size_t len) {
    const uint8_t* p = (const uint8_t*)buf;
    size_t i = 0;
    while (i < len) {
        long word = 0;
        if (i + 8 <= len) memcpy(&word, p + i, 8);
        else {
            long cur = peek(pid, addr + i);
            uint8_t merged[8];
            memcpy(merged, &cur, 8);
            memcpy(merged, p + i, len - i);
            memcpy(&word, merged, 8);
        }
        poke(pid, addr + i, word);
        i += 8;
    }
    return true;
}

static bool read_maps(pid_t pid, std::vector<std::pair<uintptr_t,std::string>>& out) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        uintptr_t start = 0;
        char perms[8] = {};
        char rest[512] = {};
        int n = sscanf(line.c_str(), "%lx-%*lx %7s %*s %*s %*s %511[^\n]",
                       &start, perms, rest);
        if (n >= 3) {
            std::string p = rest;
            while (!p.empty() && p.front() == ' ') p.erase(p.begin());
            out.emplace_back(start, p);
        } else if (n >= 2) {
            out.emplace_back(start, std::string());
        }
    }
    return true;
}

static uintptr_t find_lib_in(const std::vector<std::pair<uintptr_t,std::string>>& maps,
                             const char* name) {
    uintptr_t best = 0;
    size_t n = strlen(name);
    for (auto& m : maps) {
        if (m.second.empty()) continue;
        auto& p = m.second;
        if (p.size() >= n && p.compare(p.size() - n, n, name) == 0)
            if (best == 0 || m.first < best) best = m.first;
    }
    return best;
}

static uintptr_t find_lib_local(const char* name) {
    std::vector<std::pair<uintptr_t,std::string>> maps;
    read_maps(getpid(), maps);
    return find_lib_in(maps, name);
}

static uintptr_t resolve_dlopen_remote(pid_t pid) {
    std::vector<std::pair<uintptr_t,std::string>> maps;
    if (!read_maps(pid, maps)) return 0;
    uintptr_t remote = find_lib_in(maps, "libdl.so");
    if (!remote) remote = find_lib_in(maps, "libc.so");
    if (!remote) return 0;

    uintptr_t local = find_lib_local("libdl.so");
    if (!local) local = find_lib_local("libc.so");
    void* fp = dlsym(RTLD_DEFAULT, "dlopen");
    if (!local || !fp) return 0;
    return remote + ((uintptr_t)fp - local);
}

static bool get_regs(pid_t pid, user_pt_regs& r) {
    iovec io { &r, sizeof(r) };
    return ptrace(PTRACE_GETREGSET, pid, (void*)NT_PRSTATUS, &io) == 0;
}
static bool set_regs(pid_t pid, user_pt_regs& r) {
    iovec io { &r, sizeof(r) };
    return ptrace(PTRACE_SETREGSET, pid, (void*)NT_PRSTATUS, &io) == 0;
}

int main(int argc, char** argv) {
    if (argc != 3) { fprintf(stderr, "usage: %s <pid> <so>\n", argv[0]); return 1; }
    pid_t pid = (pid_t)atoi(argv[1]);
    const char* so = argv[2];
    if (pid <= 0 || so[0] != '/') { fprintf(stderr, "[-] bad args\n"); return 1; }

    fprintf(stderr, "[*] pid=%d lib=%s\n", pid, so);

    if (ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) < 0) {
        fprintf(stderr, "[-] attach: %s\n", strerror(errno)); return 1;
    }
    int status = 0;
    waitpid(pid, &status, 0);
    if (!WIFSTOPPED(status)) { ptrace(PTRACE_DETACH, pid, 0, 0); return 1; }

    user_pt_regs saved{}, regs{};
    if (!get_regs(pid, saved)) { ptrace(PTRACE_DETACH, pid, 0, 0); return 1; }
    regs = saved;

    uintptr_t dlopen_addr = resolve_dlopen_remote(pid);
    if (!dlopen_addr) { fprintf(stderr, "[-] dlopen resolve failed\n");
                        ptrace(PTRACE_DETACH, pid, 0, 0); return 1; }
    fprintf(stderr, "[+] remote dlopen = 0x%lx\n", (unsigned long)dlopen_addr);

    uintptr_t str_addr = saved.sp - 0x400;
    size_t plen = strlen(so) + 1;
    write_mem(pid, str_addr, so, plen);

    regs.regs[0]  = str_addr;
    regs.regs[1]  = 2;               // RTLD_NOW
    regs.regs[30] = 0;
    regs.pc       = dlopen_addr;
    if (!set_regs(pid, regs)) { ptrace(PTRACE_DETACH, pid, 0, 0); return 1; }

    ptrace(PTRACE_CONT, pid, 0, 0);
    waitpid(pid, &status, 0);
    if (!WIFSTOPPED(status)) { fprintf(stderr, "[-] target died\n"); return 1; }

    user_pt_regs after{};
    get_regs(pid, after);
    uintptr_t handle = after.regs[0];
    fprintf(stderr, "[+] handle = 0x%lx\n", (unsigned long)handle);

    set_regs(pid, saved);
    ptrace(PTRACE_DETACH, pid, 0, 0);

    if (handle == 0) { fprintf(stderr, "[-] dlopen returned 0\n"); return 2; }
    fprintf(stderr, "[+] injected\n");
    return 0;
}
