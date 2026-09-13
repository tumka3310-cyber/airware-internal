#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "utils/math.h"

namespace game {

struct Player {
    void* ptr     = nullptr;
    int   team    = 0;
    int   health  = 0;
    int   armor   = 0;
    bool  dead    = true;
    bool  visible = false;
    bool  firing  = false;
    Vec3  head    = {};
    Vec3  chest   = {};
    Vec3  pelvis  = {};
    Vec3  origin  = {};
    std::string name;
};

struct Camera {
    Vec3    position = {};
    Vec3    angles   = {};
    float   fov      = 90.f;
    Matrix4 viewProj = {};
    int     width    = 0;
    int     height   = 0;
};

bool init();
void update();
const Camera& camera();
const std::vector<Player>& players();
void* local_player();
bool  world_to_screen(const Vec3& w, Vec2& out);
void* get_transform(void* component);
Vec3  transform_position(void* transform);
Vec3  bone_position(void* player, int bone);

struct Offsets {
    uint32_t player_team      = 0;
    uint32_t player_health    = 0;
    uint32_t player_armor     = 0;
    uint32_t player_dead      = 0;
    uint32_t player_firing    = 0;
    uint32_t player_transform = 0;
    uint32_t player_name      = 0;
    uint32_t camera_fov       = 0;
};
extern Offsets off;

} // namespace game
