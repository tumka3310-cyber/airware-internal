#include "hooks/hooks.h"
#include "hooks/input.h"
#include "config/config.h"
#include "sdk/game.h"
#include "sdk/il2cpp.h"
#include "features/esp.h"
#include "features/aimbot.h"
#include "features/chams.h"
#include "features/antiaim.h"
#include "menu/menu.h"
#include "menu/watermark.h"

#include <android/log.h>
#include <dlfcn.h>
#include <dobby.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_android.h>
#include <pthread.h>
#include <unistd.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "airware", __VA_ARGS__)

namespace hooks {

bool g_running = true;
ANativeWindow* g_window = nullptr;
int g_width = 0, g_height = 0;

using eglSwapBuffers_t            = EGLBoolean (*)(EGLDisplay, EGLSurface);
using eglSwapBuffersWithDamage_t  = EGLBoolean (*)(EGLDisplay, EGLSurface, const EGLint*, EGLint);
static eglSwapBuffers_t           o_swap  = nullptr;
static eglSwapBuffersWithDamage_t o_swapd = nullptr;

static bool g_imgui_ready = false;

static void init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.DisplaySize = ImVec2((float)g_width, (float)g_height);
    menu::apply_theme();
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGui_ImplAndroid_Init(g_window);
    g_imgui_ready = true;
    LOGI("airware: imgui ready %dx%d", g_width, g_height);
}

static void frame() {
    game::update();
    aimbot::tick();
    input::feed_imgui();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();

    esp::render();
    aimbot::render_ui();
    watermark::render();
    if (cfg::misc.menu_open) menu::render();

    ImGui::Render();
    glViewport(0, 0, g_width, g_height);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static EGLBoolean hook_swap(EGLDisplay dpy, EGLSurface surf) {
    if (!g_imgui_ready) {
        eglQuerySurface(dpy, surf, EGL_WIDTH,  &g_width);
        eglQuerySurface(dpy, surf, EGL_HEIGHT, &g_height);
        if (g_width > 0 && g_height > 0) init_imgui();
    }
    if (g_imgui_ready && !cfg::misc.stream_proof) frame();
    return o_swap(dpy, surf);
}

static EGLBoolean hook_swapd(EGLDisplay dpy, EGLSurface surf, const EGLint* r, EGLint n) {
    if (!g_imgui_ready) {
        eglQuerySurface(dpy, surf, EGL_WIDTH,  &g_width);
        eglQuerySurface(dpy, surf, EGL_HEIGHT, &g_height);
        if (g_width > 0 && g_height > 0) init_imgui();
    }
    if (g_imgui_ready) frame();
    return o_swapd(dpy, surf, r, n);
}

static void* sym(const char* lib, const char* name) {
    void* h = dlopen(lib, RTLD_NOW);
    return h ? dlsym(h, name) : nullptr;
}

bool init() {
    if (!game::init()) return false;

    chams::init();
    antiaim::init();

    if (void* s = sym("libEGL.so", "eglSwapBuffers"))
        DobbyHook(s, (void*)hook_swap, (void**)&o_swap);
    if (void* sd = sym("libEGL.so", "eglSwapBuffersWithDamageKHR"))
        DobbyHook(sd, (void*)hook_swapd, (void**)&o_swapd);

    input::init();
    return true;
}

void shutdown() {
    g_running = false;
    if (g_imgui_ready) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplAndroid_Shutdown();
        ImGui::DestroyContext();
        g_imgui_ready = false;
    }
    input::shutdown();
}

} // namespace hooks
