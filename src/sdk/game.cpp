#include "game.h"
#include "il2cpp.h"
#include "utils/memory.h"
#include <android/log.h>
#include <cmath>
#include <cstring>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "airware", __VA_ARGS__)

namespace game {

Offsets off;
static Camera              g_camera;
static std::vector<Player> g_players;
static void*               g_local      = nullptr;
static il2cpp::Il2CppClass* g_playerCls  = nullptr;
static il2cpp::Il2CppClass* g_transformC = nullptr;

static constexpr const char* PLAYER_NS    = "Standoff";
static constexpr const char* PLAYER_CLASS = "PlayerController";

bool init() {
    if (!il2cpp::init()) return false;
    g_playerCls  = il2cpp::find_class(PLAYER_NS, PLAYER_CLASS);
    g_transformC = il2cpp::find_class("UnityEngine", "Transform");

    if (g_playerCls) {
        off.player_team      = il2cpp::field_offset(g_playerCls, "team");
        off.player_health    = il2cpp::field_offset(g_playerCls, "health");
        off.player_armor     = il2cpp::field_offset(g_playerCls, "armor");
        off.player_dead      = il2cpp::field_offset(g_playerCls, "isDead");
        off.player_firing    = il2cpp::field_offset(g_playerCls, "isShooting");
        off.player_transform = il2cpp::field_offset(g_playerCls, "transform");
        off.player_name      = il2cpp::field_offset(g_playerCls, "nickname");
    } else {
        LOGI("airware: PlayerController not found");
    }
    return true;
}

void* local_player() { return g_local; }
const Camera& camera() { return g_camera; }
const std::vector<Player>& players() { return g_players; }

void* get_transform(void* component) {
    if (!component || !off.player_transform) return nullptr;
    return mem::read<void*>(reinterpret_cast<uintptr_t>(component) + off.player_transform);
}

Vec3 transform_position(void* transform) {
    if (!transform || !g_transformC) return {};
    static il2cpp::Il2CppMethod* m = nullptr;
    if (!m) m = il2cpp::find_method(g_transformC, "get_position");
    if (!m) return {};
    auto fp = reinterpret_cast<Vec3(*)(void*)>(il2cpp::method_pointer(m));
    return fp ? fp(transform) : Vec3{};
}

Vec3 bone_position(void* player, int /*bone*/) {
    if (!player) return {};
    // TODO-якорь: SkinnedMeshRenderer → bone transform, если требуется.
    auto tr = get_transform(player);
    return transform_position(tr);
}

void update() {
    g_players.clear();

    // TODO-якорь: список игроков (PlayerController.allPlayers / GameController)
    // + камера. Ниже каркас — заполняется под конкретный дамп версии.
    if (!g_playerCls) return;
}

bool world_to_screen(const Vec3& w, Vec2& out) {
    if (!g_camera.width || !g_camera.height) return false;
    const auto& m = g_camera.viewProj.m;
    float cx = w.x*m[0] + w.y*m[4] + w.z*m[8]  + m[12];
    float cy = w.x*m[1] + w.y*m[5] + w.z*m[9]  + m[13];
    float cw = w.x*m[3] + w.y*m[7] + w.z*m[11] + m[15];
    if (cw < 0.01f) return false;
    float nx = cx / cw, ny = cy / cw;
    if (nx < -1.f || nx > 1.f || ny < -1.f || ny > 1.f) return false;
    out.x = g_camera.width  * 0.5f * (nx + 1.f);
    out.y = g_camera.height * 0.5f * (1.f - ny);
    return true;
}

} // namespace game
