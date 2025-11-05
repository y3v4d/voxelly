#pragma once

#include "engine/core/window.hpp"
#include "./renderer.hpp"

namespace gfx {
    class RendererGL : public Renderer {
    public:
        RendererGL(core::Window& window);
        
        void clear() override;
        void clear(float r, float g, float b, float a) override;
        void drawVAO(const VertexArray& vao, RenderMode mode = RenderMode::TRIANGLES) override;
        void drawEmpty(int count) override;
        void useShader(const Shader& shader) override;
        void bindTexture(unsigned int slot, const Texture& texture) override;
        void setViewport(int x, int y, int width, int height) override;
        void setWireframe(bool enabled) override;
        void setDepthTest(bool enabled) override;
        void setCullFace(bool enabled) override;

    private:
        unique<gfx::VertexArray> _emptyVAO;
    };
}