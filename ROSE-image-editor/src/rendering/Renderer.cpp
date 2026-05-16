#include "Renderer.h"
#include <glad/glad.h>
#include "utils/MathUtils.h"
#include <cassert>

static const char* defaultVertexSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColor;
out vec2 TexCoord;
out vec4 Color;
uniform mat4 uProjection;
uniform mat4 uView;
void main() {
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Color = aColor;
}
)";

static const char* defaultFragmentSrc = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec4 Color;
uniform sampler2D uTexture;
uniform bool uUseTexture;
void main() {
    if (uUseTexture) {
        FragColor = texture(uTexture, TexCoord) * Color;
    } else {
        FragColor = Color;
    }
}
)";

Renderer::Renderer() = default;

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(QuadVertex), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, texCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, color));

    glBindVertexArray(0);

    if (!defaultShader.compile(defaultVertexSrc, defaultFragmentSrc)) {
        return false;
    }

    quadQueue.reserve(MAX_QUADS);
    return true;
}

void Renderer::shutdown() {
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (ibo != 0) {
        glDeleteBuffers(1, &ibo);
        ibo = 0;
    }
    defaultShader.destroy();
    quadQueue.clear();
}

void Renderer::begin() {
    quadQueue.clear();
}

void Renderer::end() {
    flush();
}

void Renderer::submitQuad(const Texture* texture, const Mat4& transform, const Vec4& color) {
    if (quadQueue.size() >= MAX_QUADS) {
        flush();
    }
    quadQueue.push_back({ texture, transform, color });
}

void Renderer::drawQuad(float x, float y, float w, float h, const Vec4& color) {
    Mat4 transform = Mat4(1.0f);
    transform = glm::translate(transform, Vec3(x, y, 0.0f));
    transform = glm::scale(transform, Vec3(w, h, 1.0f));
    submitQuad(nullptr, transform, color);
}

void Renderer::drawTexturedQuad(float x, float y, float w, float h, const Texture& texture) {
    Mat4 transform = Mat4(1.0f);
    transform = glm::translate(transform, Vec3(x, y, 0.0f));
    transform = glm::scale(transform, Vec3(w, h, 1.0f));
    submitQuad(&texture, transform);
}

void Renderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::flush() {
    if (quadQueue.empty()) return;

    defaultShader.bind();

    Mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
    Mat4 view = Mat4(1.0f);
    defaultShader.setUniformMat4("uProjection", projection);
    defaultShader.setUniformMat4("uView", view);

    std::vector<QuadVertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(quadQueue.size() * 4);
    indices.reserve(quadQueue.size() * 6);

    uint32_t vertexOffset = 0;
    int currentTextureID = -1;

    for (size_t i = 0; i < quadQueue.size(); i++) {
        const auto& quad = quadQueue[i];

        bool hasTexture = quad.texture != nullptr && quad.texture->getID() != 0;
        int textureID = hasTexture ? (int)quad.texture->getID() : -1;

        if (textureID != currentTextureID && i > 0) {
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(QuadVertex), vertices.data());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t), indices.data());

            defaultShader.setUniformBool("uUseTexture", currentTextureID != -1);
            if (currentTextureID != -1) {
                glBindTexture(GL_TEXTURE_2D, (GLuint)currentTextureID);
            }

            glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);

            vertices.clear();
            indices.clear();
            vertexOffset = 0;
        }

        currentTextureID = textureID;

        Vec3 positions[4] = {
            Vec3(0.0f, 0.0f, 0.0f),
            Vec3(1.0f, 0.0f, 0.0f),
            Vec3(1.0f, 1.0f, 0.0f),
            Vec3(0.0f, 1.0f, 0.0f)
        };

        Vec2 texCoords[4] = {
            Vec2(0.0f, 0.0f),
            Vec2(1.0f, 0.0f),
            Vec2(1.0f, 1.0f),
            Vec2(0.0f, 1.0f)
        };

        for (int j = 0; j < 4; j++) {
            Vec3 pos = quad.transform * Vec4(positions[j], 1.0f);
            vertices.push_back({ pos, texCoords[j], quad.color });
        }

        indices.push_back(vertexOffset + 0);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 0);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 3);

        vertexOffset += 4;
    }

    if (!vertices.empty()) {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(QuadVertex), vertices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint32_t), indices.data());

        defaultShader.setUniformBool("uUseTexture", currentTextureID != -1);
        if (currentTextureID != -1) {
            glBindTexture(GL_TEXTURE_2D, (GLuint)currentTextureID);
        }

        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
    defaultShader.unbind();
}
