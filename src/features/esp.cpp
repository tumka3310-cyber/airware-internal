#include "esp.h"
#include "config/config.h"
#include "sdk/game.h"
#include "utils/math.h"
#include <imgui.h>
#include <cmath>
#include <cstdio>

namespace esp {

static void line(ImDrawList* dl, Vec2 a, Vec2 b, ImU32 c, float t = 1.f) {
    dl->AddLine({a.x,a.y}, {b.x,b.y}, c, t);
}
static void rect(ImDrawList* dl, Vec2 a, Vec2 b, ImU32 c, float t = 1.f) {
    dl->AddRect({a.x,a.y}, {b.x,b.y}, c, 0.f, 0, t);
}
static void corner(ImDrawList* dl, Vec2 tl, Vec2 br, ImU32 c, float seg, float t = 2.f) {
    float w = br.x - tl.x, h = br.y - tl.y;
    float sx = w * 0.25f, sy = h * 0.25f;
    if (sx > seg) sx = seg; if (sy > seg) sy = seg;
    line(dl, {tl.x, tl.y}, {tl.x+sx, tl.y}, c, t);
    line(dl, {tl.x, tl.y}, {tl.x, tl.y+sy}, c, t);
    line(dl, {br.x, tl.y}, {br.x-sx, tl.y}, c, t);
    line(dl, {br.x, tl.y}, {br.x, tl.y+sy}, c, t);
    line(dl, {tl.x, br.y}, {tl.x+sx, br.y}, c, t);
    line(dl, {tl.x, br.y}, {tl.x, br.y-sy}, c, t);
    line(dl, {br.x, br.y}, {br.x-sx, br.y}, c, t);
    line(dl, {br.x, br.y}, {br.x, br.y-sy}, c, t);
}
static void grad_box(ImDrawList* dl, Vec2 tl, Vec2 br) {
    ImU32 top = ImGui::ColorConvertFloat4ToU32(
        { cfg::colors.gradient_top[0], cfg::colors.gradient_top[1],
          cfg::colors.gradient_top[2], cfg::colors.gradient_top[3] });
    ImU32 bot = ImGui::ColorConvertFloat4ToU32(
        { cfg::colors.gradient_bot[0], cfg::colors.gradient_bot[1],
          cfg::colors.gradient_bot[2], cfg::colors.gradient_bot[3] });
    dl->AddRectFilledMultiColor({tl.x,tl.y}, {br.x,br.y}, top, top, bot, bot);
}
static void box3d(ImDrawList* dl, const Vec3& o, const Vec3& mn, const Vec3& mx, ImU32 c) {
    Vec3 v[8] = {
        {o.x+mn.x, o.y+mn.y, o.z+mn.z}, {o.x+mx.x, o.y+mn.y, o.z+mn.z},
        {o.x+mx.x, o.y+mn.y, o.z+mx.z}, {o.x+mn.x, o.y+mn.y, o.z+mx.z},
        {o.x+mn.x, o.y+mx.y, o.z+mn.z}, {o.x+mx.x, o.y+mx.y, o.z+mn.z},
        {o.x+mx.x, o.y+mx.y, o.z+mx.z}, {o.x+mn.x, o.y+mx.y, o.z+mx.z},
    };
    Vec2 s[8];
    for (int i = 0; i < 8; ++i) if (!game::world_to_screen(v[i], s[i])) return;
    static const int e[12][2] = {
        {0,1},{1,2},{2,3},{3,0}, {4,5},{5,6},{6,7},{7,4}, {0,4},{1,5},{2,6},{3,7}
    };
    for (auto& k : e) line(dl, s[k[0]], s[k[1]], c, 1.f);
}

void render() {
    if (!cfg::esp.enabled) return;
    auto* dl = ImGui::GetBackgroundDrawList();
    if (!dl) return;

    auto& cam = game::camera();
    for (auto& p : game::players()) {
        if (p.dead) continue;
        if (p.ptr == game::local_player()) continue;

        Vec3 head = p.head, foot = p.pelvis;
        if (head.x == 0.f && head.y == 0.f && head.z == 0.f) head = p.origin;
        if (foot.x == 0.f && foot.y == 0.f && foot.z == 0.f) foot = p.origin;

        Vec2 sh, sf;
        if (!game::world_to_screen(head, sh)) continue;
        if (!game::world_to_screen(foot, sf)) continue;

        float h = sf.y - sh.y;
        if (h < 1.f) continue;
        float w = h * 0.45f;
        Vec2 tl { sh.x - w*0.5f, sh.y };
        Vec2 br { sh.x + w*0.5f, sf.y };

        float d = dist(cam.position, p.origin);
        if (d > cfg::esp.max_dist) continue;

        ImU32 c_box   = ImGui::ColorConvertFloat4ToU32({ cfg::colors.box[0],   cfg::colors.box[1],   cfg::colors.box[2],   cfg::colors.box[3] });
        ImU32 c_vis   = ImGui::ColorConvertFloat4ToU32({ cfg::colors.box_vis[0], cfg::colors.box_vis[1], cfg::colors.box_vis[2], cfg::colors.box_vis[3] });
        ImU32 c_corn  = ImGui::ColorConvertFloat4ToU32({ cfg::colors.corner[0], cfg::colors.corner[1], cfg::colors.corner[2], cfg::colors.corner[3] });
        ImU32 c_txt   = ImGui::ColorConvertFloat4ToU32({ cfg::colors.text[0],   cfg::colors.text[1],   cfg::colors.text[2],   cfg::colors.text[3] });
        ImU32 c = p.visible ? c_vis : c_box;

        if (cfg::esp.gradient) grad_box(dl, tl, br);
        if (cfg::esp.box)      rect(dl, tl, br, c, 1.5f);
        if (cfg::esp.corner)   corner(dl, tl, br, c_corn, 15.f, 2.f);
        if (cfg::esp.box_3d)   box3d(dl, p.origin, {-0.4f,-0.9f,-0.4f}, {0.4f,1.0f,0.4f}, c);

        if (cfg::esp.text) {
            char buf[128];
            snprintf(buf, sizeof(buf), "%s [%dm] %d HP",
                     p.name.empty() ? "player" : p.name.c_str(), (int)d, p.health);
            ImVec2 sz = ImGui::CalcTextSize(buf);
            dl->AddText({ sh.x - sz.x*0.5f, tl.y - sz.y - 2.f }, c_txt, buf);
        }

        if (cfg::esp.health) {
            float hb_x = tl.x - 6.f, hb_y = tl.y, hb_w = 3.f, hb_h = h;
            dl->AddRectFilled({hb_x,hb_y}, {hb_x+hb_w, hb_y+hb_h}, IM_COL32(0,0,0,180));
            float f = p.health / 100.f;
            if (f < 0.f) f = 0.f; if (f > 1.f) f = 1.f;
            ImU32 hc = IM_COL32((int)(255*(1-f)), (int)(255*f), 0, 255);
            dl->AddRectFilled({hb_x, hb_y + hb_h*(1-f)}, {hb_x+hb_w, hb_y+hb_h}, hc);
        }

        if (cfg::esp.snapline)
            line(dl, { (float)cam.width*0.5f, (float)cam.height }, { sh.x, br.y }, c, 1.f);
    }
}

} // namespace esp
