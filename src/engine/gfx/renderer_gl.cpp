#include "engine/gfx/renderer_gl.hpp"
#include "engine/core/window.hpp"
#include <glad/gl.h>

#include <iostream>

using namespace gfx;

GLenum getGLPrimitiveType(RenderMode mode) {
    switch (mode) {
        case RenderMode::TRIANGLES: return GL_TRIANGLES;
        case RenderMode::LINES: return GL_LINES;
        default: return GL_TRIANGLES;
    }
}

RendererGL::RendererGL(core::Window& window) : Renderer(window) {
    window.makeContextCurrent();
    window.setSwapInterval(1);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    std::cout << "OpenGL Version: " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(32.0f);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    _emptyVAO = std::make_unique<gfx::VertexArray>();
}

void RendererGL::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererGL::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    clear();
}

void RendererGL::drawVAO(const VertexArray& vao, RenderMode mode) {
    vao.bind();
    glDrawElements(getGLPrimitiveType(mode), vao.getIndexBuffer()->getCount(), GL_UNSIGNED_INT, 0);
    vao.unbind();
}

void RendererGL::drawEmpty(int count) {
    _emptyVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
    _emptyVAO->unbind();
}

void RendererGL::useShader(const Shader& shader) {
    glUseProgram(shader.getID());
}

void RendererGL::bindTexture(unsigned int slot, const Texture& texture) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.getID());
}

void RendererGL::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void RendererGL::setDepthTest(bool enabled) {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void RendererGL::setWireframe(bool enabled) {
    if (enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void RendererGL::setCullFace(bool enabled) {
    if (enabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}