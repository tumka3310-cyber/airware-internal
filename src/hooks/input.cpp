#include "hooks/input.h"
#include "config/config.h"
#include <android/log.h>
#include <dobby.h>
#include <dlfcn.h>
#include <imgui.h>
#include <jni.h>
#include <mutex>
#include <atomic>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "airware", __VA_ARGS__)

namespace input {

using getX_t  = jfloat (*)(JNIEnv*, jobject, jint);
using getY_t  = jfloat (*)(JNIEnv*, jobject, jint);
using getA_t  = jint   (*)(JNIEnv*, jobject);

static getX_t o_getX = nullptr;
static getY_t o_getY = nullptr;
static getA_t o_getA = nullptr;

static std::mutex          g_mtx;
static std::atomic<float>  g_x{ 0.f };
static std::atomic<float>  g_y{ 0.f };
static std::atomic<bool>   g_down{ false };
static std::atomic<bool>   g_consumed{ false };

static jfloat h_getX(JNIEnv* e, jobject s, jint i) {
    jfloat x = o_getX(e, s, i);
    if (i == 0) g_x = x;
    return g_consumed ? -10000.f : x;
}
static jfloat h_getY(JNIEnv* e, jobject s, jint i) {
    jfloat y = o_getY(e, s, i);
    if (i == 0) g_y = y;
    return g_consumed ? -10000.f : y;
}
static jint h_getAction(JNIEnv* e, jobject s) {
    jint a = o_getA(e, s);
    if (a == 0) g_down = true;
    if (a == 1) g_down = false;
    return a;
}

bool capturing() { return g_consumed.load(); }

void feed_imgui() {
    if (!cfg::misc.menu_open) return;
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(g_x.load(), g_y.load());
    io.AddMouseButtonEvent(0, g_down.load());
}

static void* sym(const char* lib, const char* name) {
    void* h = dlopen(lib, RTLD_NOW);
    return h ? dlsym(h, name) : nullptr;
}

bool init() {
    auto x = sym("libandroid_runtime.so", "android_view_MotionEvent_getX");
    auto y = sym("libandroid_runtime.so", "android_view_MotionEvent_getY");
    auto a = sym("libandroid_runtime.so", "android_view_MotionEvent_getAction");
    if (!x || !y || !a) { LOGI("airware: input symbols unresolved"); return false; }
    DobbyHook(x, (void*)h_getX, (void**)&o_getX);
    DobbyHook(y, (void*)h_getY, (void**)&o_getY);
    DobbyHook(a, (void*)h_getAction, (void**)&o_getA);
    LOGI("airware: input hooked");
    return true;
}

void shutdown() {}

} // namespace input
