#pragma once

#include <cstdint>

struct Vec2 {
    float x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
};

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Vec4 {
    float x, y, z, w;
    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct Mat4 {
    float m[16];
    Mat4();
    static Mat4 identity();
    static Mat4 ortho(float left, float right, float bottom, float top, float near, float far);
    static Mat4 translate(float x, float y, float z);
    static Mat4 scale(float x, float y, float z);
    Mat4 operator*(const Mat4& other) const;
};
