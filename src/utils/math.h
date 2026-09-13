#pragma once
#include <cmath>

struct Vec2 { float x, y; };
struct Vec3 { float x, y, z; };
struct Matrix4 { float m[16] = {}; };

inline Vec3 operator-(const Vec3& a, const Vec3& b) { return { a.x-b.x, a.y-b.y, a.z-b.z }; }
inline Vec3 operator+(const Vec3& a, const Vec3& b) { return { a.x+b.x, a.y+b.y, a.z+b.z }; }
inline Vec3 operator*(const Vec3& a, float s)      { return { a.x*s, a.y*s, a.z*s }; }

inline float dot(const Vec3& a, const Vec3& b)  { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline float len(const Vec3& v)                 { return std::sqrt(dot(v, v)); }
inline Vec3  norm(const Vec3& v)                { float l = len(v); return l > 1e-5f ? v * (1.f/l) : Vec3{}; }
inline float dist(const Vec3& a, const Vec3& b) { return len(a - b); }

inline Vec3 angle_from(const Vec3& d) {
    float hyp = std::sqrt(d.x*d.x + d.z*d.z);
    Vec3 a;
    a.x = -std::atan2(d.y, hyp) * 57.29578f;
    a.y =  std::atan2(d.x, d.z) * 57.29578f;
    a.z = 0.f;
    return a;
}
inline float angle_dist(float a, float b) {
    float d = std::fmod(a - b + 180.f, 360.f);
    if (d < 0.f) d += 360.f;
    return d - 180.f;
}
