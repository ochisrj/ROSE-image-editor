#include "Shader.h"
#include <glad/glad.h>
#include <cassert>

Shader::Shader() = default;

Shader::~Shader() {
    destroy();
}

bool Shader::compile(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    if (vertexShader == 0) {
        return false;
    }

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        glDeleteProgram(programID);
        programID = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programID != 0;
}

void Shader::destroy() {
    if (programID != 0) {
        glDeleteProgram(programID);
        programID = 0;
    }
}

void Shader::bind() const {
    glUseProgram(programID);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setUniformInt(const char* name, int value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::setUniformFloat(const char* name, float value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::setUniformVec2(const char* name, const Vec2& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform2fv(location, 1, &value[0]);
    }
}

void Shader::setUniformVec3(const char* name, const Vec3& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, &value[0]);
    }
}

void Shader::setUniformVec4(const char* name, const Vec4& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform4fv(location, 1, &value[0]);
    }
}

void Shader::setUniformMat4(const char* name, const Mat4& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }
}

void Shader::setUniformBool(const char* name, bool value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value ? 1 : 0);
    }
}

GLuint Shader::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLint Shader::getUniformLocation(const char* name) {
    return glGetUniformLocation(programID, name);
}
