#include "chams.h"
#include "config/config.h"
#include "sdk/il2cpp.h"
#include "utils/memory.h"
#include <android/log.h>
#include <dobby.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "airware", __VA_ARGS__)

namespace chams {

using set_mat_t = void (*)(void* self, void* material, void* method);
static set_mat_t o_set_mat = nullptr;
static il2cpp::Il2CppClass* g_rendererCls = nullptr;
static il2cpp::Il2CppClass* g_materialCls = nullptr;

static void h_set_mat(void* self, void* material, void* method) {
    if (cfg::chams.enabled && g_materialCls) {
        // TODO-якорь: подмена материала рендерера игрока на glass-инстанс.
    }
    o_set_mat(self, material, method);
}

bool init() {
    g_rendererCls = il2cpp::find_class("UnityEngine", "Renderer");
    g_materialCls = il2cpp::find_class("UnityEngine", "Material");
    if (!g_rendererCls) { LOGI("airware: Renderer not found"); return false; }

    auto m = il2cpp::find_method(g_rendererCls, "set_material");
    if (!m) { LOGI("airware: set_material not found"); return false; }
    void* addr = il2cpp::method_pointer(m);
    if (!addr) return false;

    DobbyHook(addr, (void*)h_set_mat, (void**)&o_set_mat);
    return true;
}

void apply() {}

} // namespace chams
