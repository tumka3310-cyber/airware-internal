#include "aimbot.h"
#include "config/config.h"
#include "sdk/game.h"
#include "utils/math.h"
#include <imgui.h>
#include <cmath>

namespace aimbot {

static game::Player* pick(const game::Camera& cam) {
    game::Player* best = nullptr;
    float best_fov = cfg::aimbot.fov;
    for (auto& p : game::players()) {
        if (p.dead) continue;
        if (p.ptr == game::local_player()) continue;

        Vec3 t = p.head;
        if (cfg::aimbot.bone == 1) t = p.chest;
        if (cfg::aimbot.bone == 2) t = p.pelvis;

        Vec3 ang = angle_from(t - cam.position);
        float dx = std::fabs(angle_dist(ang.y, cam.angles.y));
        float dy = std::fabs(angle_dist(ang.x, cam.angles.x));
        float f  = std::sqrt(dx*dx + dy*dy);
        if (f > best_fov) continue;
        if (cfg::aimbot.visible_check && !p.visible) continue;
        if (cfg::aimbot.fire_check    && !p.firing)  continue;
        best_fov = f;
        best = &p;
    }
    return best;
}

void tick() {
    if (!cfg::aimbot.enabled) return;
    auto& cam = game::camera();
    auto* tgt = pick(cam);
    if (!tgt) return;

    Vec3 pt = tgt->head;
    if (cfg::aimbot.bone == 1) pt = tgt->chest;
    if (cfg::aimbot.bone == 2) pt = tgt->pelvis;

    Vec3 want = angle_from(pt - cam.position);
    float sx = angle_dist(want.x, cam.angles.x) / cfg::aimbot.smooth;
    float sy = angle_dist(want.y, cam.angles.y) / cfg::aimbot.smooth;

    // TODO-якорь: запись углов через PlayerController::viewAngle
    // mem::write<Vec3>((uintptr_t)game::local_player() + off_view_angle,
    //                  { cam.angles.x + sx, cam.angles.y + sy, 0.f });
    (void)sx; (void)sy;
}

void render_ui() {
    if (!cfg::aimbot.enabled || !cfg::aimbot.draw_fov) return;
    auto& cam = game::camera();
    if (!cam.width || !cam.height) return;
    auto* dl = ImGui::GetBackgroundDrawList();
    float r = std::tan(cfg::aimbot.fov * 3.14159265f / 360.f) * (cam.height * 0.5f);
    dl->AddCircle({ (float)cam.width*0.5f, (float)cam.height*0.5f },
                  r, IM_COL32(74,158,255,140), 64, 1.f);
}

} // namespace aimbot
