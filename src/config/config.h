#pragma once

namespace cfg {

struct EspSettings {
    bool  enabled    = false;
    bool  box        = false;
    bool  box_3d     = false;
    bool  corner     = false;
    bool  gradient   = false;
    bool  text       = false;
    bool  health     = false;
    bool  snapline   = false;
    bool  team_check = true;
    float max_dist   = 400.f;
};
struct ColorSettings {
    float box[4]          = { 0.29f, 0.62f, 1.00f, 1.f };
    float box_vis[4]      = { 0.00f, 1.00f, 0.30f, 1.f };
    float text[4]         = { 1.00f, 1.00f, 1.00f, 1.f };
    float corner[4]       = { 0.29f, 0.62f, 1.00f, 1.f };
    float gradient_top[4] = { 0.29f, 0.62f, 1.00f, 1.f };
    float gradient_bot[4] = { 0.00f, 0.10f, 0.40f, 0.f };
    float chams[4]        = { 0.29f, 0.62f, 1.00f, 0.45f };
};
struct AimbotSettings {
    bool  enabled       = false;
    int   bone          = 0;
    float fov           = 90.f;
    float smooth        = 4.f;
    bool  visible_check = true;
    bool  fire_check    = false;
    bool  shoot_check   = false;
    bool  draw_fov      = true;
};
struct ChamsSettings { bool enabled = false; bool glass = true; bool ignore_z = false; };
struct AntiAimSettings { bool enabled = false; int mode = 0; float speed = 12.f; };
struct MiscSettings { bool watermark = true; bool stream_proof = false; bool menu_open = true; };

inline EspSettings     esp;
inline ColorSettings   colors;
inline AimbotSettings  aimbot;
inline ChamsSettings   chams;
inline AntiAimSettings antiaim;
inline MiscSettings    misc;

} // namespace cfg
