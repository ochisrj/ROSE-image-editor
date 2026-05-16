#pragma once

#include "Texture.h"
#include "Shader.h"
#include "utils/MathUtils.h"
#include <vector>

struct QuadVertex {
    Vec3 position;
    Vec2 texCoord;
    Vec4 color;
};

struct QuadData {
    const Texture* texture;
    Mat4 transform;
    Vec4 color;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init();
    void shutdown();

    void begin();
    void end();

    void submitQuad(const Texture* texture, const Mat4& transform, const Vec4& color = Vec4(1.0f));
    void drawQuad(float x, float y, float w, float h, const Vec4& color = Vec4(1.0f));
    void drawTexturedQuad(float x, float y, float w, float h, const Texture& texture);

    void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

private:
    void flush();

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;

    std::vector<QuadData> quadQueue;
    Shader defaultShader;

    static constexpr int MAX_QUADS = 1000;
    static constexpr int MAX_VERTICES = MAX_QUADS * 4;
    static constexpr int MAX_INDICES = MAX_QUADS * 6;
};
