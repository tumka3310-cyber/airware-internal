#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include "hooks/hooks.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "airware", __VA_ARGS__)

static void* boot(void*) {
    for (int i = 0; i < 50; ++i) {
        void* h = dlopen("libil2cpp.so", RTLD_NOLOAD | RTLD_NOW);
        if (h) { dlclose(h); break; }
        usleep(100 * 1000);
    }
    usleep(1200 * 1000);

    if (!hooks::init()) { LOGI("airware: init failed"); return nullptr; }
    LOGI("airware: ready");

    while (hooks::g_running) usleep(200 * 1000);
    hooks::shutdown();
    return nullptr;
}

__attribute__((constructor))
static void airware_on_load() {
    LOGI("airware: constructor");
    pthread_t t;
    pthread_create(&t, nullptr, boot, nullptr);
    pthread_detach(t);
}
