#include "MathUtils.h"
#include <cstring>

Mat4::Mat4() { memset(m, 0, sizeof(m)); m[0] = m[5] = m[10] = m[15] = 1.0f; }

Mat4 Mat4::identity() { return Mat4(); }

Mat4 Mat4::ortho(float left, float right, float bottom, float top, float near, float far) {
    Mat4 result;
    memset(result.m, 0, sizeof(result.m));
    result.m[0] = 2.0f / (right - left);
    result.m[5] = 2.0f / (top - bottom);
    result.m[10] = -2.0f / (far - near);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -(far + near) / (far - near);
    result.m[15] = 1.0f;
    return result;
}

Mat4 Mat4::translate(float x, float y, float z) {
    Mat4 result;
    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;
    return result;
}

Mat4 Mat4::scale(float x, float y, float z) {
    Mat4 result;
    result.m[0] = x;
    result.m[5] = y;
    result.m[10] = z;
    return result;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;
    memset(result.m, 0, sizeof(result.m));
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                result.m[j * 4 + i] += m[k * 4 + i] * other.m[j * 4 + k];
    return result;
}
