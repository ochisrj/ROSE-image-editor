#pragma once

#include <string>
#include "utils/MathUtils.h"

class Shader {
public:
    Shader();
    ~Shader();

    bool compile(const char* vertexSrc, const char* fragmentSrc);
    void destroy();

    void bind() const;
    void unbind() const;

    void setUniformInt(const char* name, int value);
    void setUniformFloat(const char* name, float value);
    void setUniformVec2(const char* name, const Vec2& value);
    void setUniformVec3(const char* name, const Vec3& value);
    void setUniformVec4(const char* name, const Vec4& value);
    void setUniformMat4(const char* name, const Mat4& value);
    void setUniformBool(const char* name, bool value);

    GLuint getProgramID() const { return programID; }

private:
    GLuint programID = 0;

    GLuint compileShader(GLenum type, const char* source);
    GLint getUniformLocation(const char* name);
};
