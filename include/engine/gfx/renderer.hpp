#pragma once

#include "engine/core/window.hpp"

#include "engine/gfx/shader.hpp"
#include "engine/gfx/texture.hpp"
#include "engine/gfx/buffer/vertex_array.hpp"

#include "engine/assets/mesh.hpp"

namespace gfx {
    enum class RenderMode {
        TRIANGLES,
        LINES
    };

    struct RenderQueue {
        const VertexArray* vao;
        RenderMode mode;
    };

    class Renderer {
    public:
        Renderer(core::Window& window) : window(window) {}

        unique<VertexArray> createMeshVAO(const assets::Mesh& mesh);

        virtual void clear() = 0;
        virtual void clear(float r, float g, float b, float a) = 0;
        virtual void drawVAO(const VertexArray& vao, RenderMode mode) = 0;
        virtual void drawEmpty(int count) = 0;

        virtual void useShader(const Shader& shader) = 0;
        virtual void bindTexture(unsigned int slot, const Texture& texture) = 0;
        virtual void setViewport(int x, int y, int width, int height) = 0;
        virtual void setDepthTest(bool enabled) = 0;
        virtual void setCullFace(bool enabled) = 0;

        virtual void setWireframe(bool enabled) = 0;

        virtual void enablePolygonOffsetFill(float factor, float units) = 0;
        virtual void disablePolygonOffsetFill() = 0;

        unsigned int getDrawCallCount() const { return drawCallCount; }

    protected:
        core::Window& window;
        unsigned int drawCallCount = 0;
    };
}