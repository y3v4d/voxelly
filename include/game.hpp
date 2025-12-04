#pragma once

#include "engine/engine.hpp"
#include "chunk.hpp"
#include "world.hpp"
#include "text_renderer.hpp"
#include "world_mesh.hpp"
#include "ray.hpp"

#include <chrono>
#include <iostream>

#include <vector>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include <SDL3/SDL.h>

enum ToolType {
    TOOL_PLACE,
    TOOL_ERASE,
    TOOL_BRUSH,
    TOOL_LINE,
    TOOL_MOVE
};

enum class ToolShape {
    SHAPE_SPHERE,
    SHAPE_CUBE
};

enum AxisLock {
    AXIS_LOCK_NONE,
    AXIS_LOCK_X,
    AXIS_LOCK_Y,
    AXIS_LOCK_Z
};

class Game {
public:
    void init();
    void loop();
    void shutdown();

    bool isRunning() const {
        return !window->shouldClose();
    }

    std::unique_ptr<core::Window> window;
    
private:
    std::unique_ptr<gfx::RendererGL> renderer;

    std::unique_ptr<TextRenderer> textRenderer;
    std::unique_ptr<UIRenderer> uiRenderer;

    std::unique_ptr<assets::AssetManager> assetManager;

    std::shared_ptr<assets::Font> font;

    gfx::Shader voxelShader;
    gfx::Shader textShader;
    gfx::Shader gridShader;
    gfx::Shader outlineShader;
    gfx::Shader text3DShader;
    gfx::Shader basic2DShader;

    std::shared_ptr<Camera> worldCamera;
    std::shared_ptr<Camera> uiCamera;

    std::unique_ptr<Sprite> uiPanelLeftSprite;
    std::unique_ptr<Text> fpsText;
    std::unique_ptr<Text> cameraText;
    std::unique_ptr<Text> cameraRotationText;
    std::unique_ptr<Text> rayHitText;
    std::unique_ptr<Text> currentToolText;
    std::unique_ptr<Text> toolShapeText;
    std::unique_ptr<Text> brushSizeText;
    std::unique_ptr<Text> currentPaletteText;
    std::unique_ptr<Text> drawCallText;
    std::unique_ptr<Text> axisLockText;

    std::unique_ptr<gfx::VertexArray> outlineVAO;
    std::unique_ptr<gfx::VertexArray> previewVAO;

    std::shared_ptr<gfx::Texture> paletteTexture;
    std::unique_ptr<Sprite> sprite;

    std::unique_ptr<Sprite> uiBackgroundSprite;

    std::unique_ptr<WorldMesh> worldMesh;

    std::unique_ptr<World> world;
    unsigned char blockType = 0;

    std::unique_ptr<World> toolWorld;
    std::unique_ptr<WorldMesh> toolWorldMesh;

    //std::optional<WorldRayHit> rayHit;

    std::vector<glm::vec4> color_palette = {
        glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), // Black
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // White
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // Red
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // Green
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // Blue
    };

    glm::vec3 placementSize = glm::vec3(1.0f, 1.0f, 1.0f);
    int brushSize = 0;

    glm::vec2 lastMousePos = glm::vec2(0.0f, 0.0f);
    glm::vec3 camVelocity = glm::vec3(0.0f, 0.0f, 0.0f);

    std::unordered_map<int, bool> canPressKey;
    std::unordered_map<int, bool> canPressMouse;
    std::unordered_map<int, bool> isKeyHeld;

    ToolType currentTool = ToolType::TOOL_PLACE;
    ToolShape toolShape = ToolShape::SHAPE_SPHERE;
    AxisLock axisLock = AxisLock::AXIS_LOCK_NONE;
    glm::ivec3 axisLockPosition = glm::ivec3(0, 0, 0);

    std::vector<glm::ivec3> moveVoxels;
    glm::ivec3 moveVoxelsStart;

    Ray mouseRay;
    std::optional<WorldRayHit> brushHit;

    bool isWireframe = false;
    bool darkMode = false;

    bool showBoundingBox = true;
    bool isMovingCamera = true;
    bool lineInProgress = false;

    int y_plane = -1;

    std::chrono::steady_clock::time_point lastTime;
    double frameCounter = 0.0;
    int drawCallCounter = 0;

    void updateWorldCameraProjection(ProjectionType type);
    void regenerate_palette();
    void set_current_palette_sprite_uvs(int index);
    void update_current_tool_text();
    void update_axis_lock_text();
    void construct_ui();
};