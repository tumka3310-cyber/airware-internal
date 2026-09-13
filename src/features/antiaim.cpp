#include "antiaim.h"
#include "config/config.h"
#include "sdk/game.h"
#include "sdk/il2cpp.h"
#include "utils/math.h"
#include <android/log.h>
#include <dobby.h>
#include <cstdlib>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "airware", __VA_ARGS__)

namespace antiaim {

using set_view_t = void (*)(void* self, Vec3 angles, void* method);
static set_view_t o_set_view = nullptr;

static void h_set_view(void* self, Vec3 angles, void* method) {
    auto* lp = game::local_player();
    if (cfg::antiaim.enabled && self == lp) {
        static float t = 0.f;
        t += 0.016f;
        if (cfg::antiaim.mode == 0) {
            angles.y += (float)((rand() % 360) - 180);
            angles.x += (float)((rand() % 90)  - 45);
        } else if (cfg::antiaim.mode == 1) {
            angles.y += t * cfg::antiaim.speed * 60.f;
        } else if (cfg::antiaim.mode == 2) {
            static int flip = 0; flip ^= 1;
            angles.y += flip ? 89.f : -89.f;
        }
    }
    o_set_view(self, angles, method);
}

bool init() {
    auto klass = il2cpp::find_class("Standoff", "PlayerController");
    if (!klass) return false;

    const char* cands[] = { "set_ViewAngle", "SetViewAngle", "SetAngles" };
    void* addr = nullptr;
    for (auto n : cands) {
        auto m = il2cpp::find_method(klass, n);
        if (m) { addr = il2cpp::method_pointer(m); if (addr) break; }
    }
    if (!addr) { LOGI("airware: antiaim target not found"); return false; }

    DobbyHook(addr, (void*)h_set_view, (void**)&o_set_view);
    return true;
}

} // namespace antiaim
