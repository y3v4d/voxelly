#include "engine/engine.hpp"
#include "chunk.hpp"
#include "world.hpp"
#include "text_renderer.hpp"
#include "chunk_mesh.hpp"

#include <chrono>
#include <iostream>

#include <vector>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

// rotate the camera based on the center of the chunk

class Ray {
public:
    Ray(const glm::vec3& origin, const glm::vec3& direction)
        : origin(origin), direction(direction) {}

    glm::vec3 at(float t) const {
        return origin + direction * t;
    }

    glm::vec3 origin;
    glm::vec3 direction;
};

std::optional<WorldRayHit> getVoxelFromMouse(const core::Window& window, const Camera& camera, const World& world, double mouseX, double mouseY) {
    auto viewMatrix = camera.getViewMatrix();
    auto projectionMatrix = camera.getProjectionMatrix();

    glm::vec3 nearPlane = glm::unProject(
        glm::vec3(mouseX, mouseY, 0.0f),
        viewMatrix,
        projectionMatrix,
        glm::ivec4(0, 0, window.getWidth(), window.getHeight())
    );

    glm::vec3 farPlane = glm::unProject(
        glm::vec3(mouseX, mouseY, 1.0f),
        viewMatrix,
        projectionMatrix,
        glm::ivec4(0, 0, window.getWidth(), window.getHeight())
    );

    glm::vec3 rayOrigin = nearPlane;
    glm::vec3 rayDir = glm::normalize(farPlane - nearPlane);

    return world.findRayHit(
        rayOrigin,
        rayDir,
        256.0f
    );
}

int main() {
    auto assetManager = assets::AssetManager();
    auto font = assetManager.loadAsset<assets::Font>("assets/fonts/kenney-mini-square.ttf");

    core::Window window(800, 600, "Voxelly | v0.0.1");
    
    std::cout << "Window created: " << window.getWidth() << "x" << window.getHeight() << std::endl;
    std::cout << "Framebuffer size: " << window.getFramebufferWidth() << "x" << window.getFramebufferHeight() << std::endl;
    
    gfx::RendererGL renderer(window);
    renderer.setViewport(0, 0, window.getFramebufferWidth(), window.getFramebufferHeight());

    gfx::Shader voxelShader;
    voxelShader.compile(assets::Shader("assets/shaders/voxel"));

    gfx::Shader textShader;
    textShader.compile(assets::Shader("assets/shaders/text"));

    gfx::Shader gridShader;
    gridShader.compile(assets::Shader("assets/shaders/grid"));

    gfx::Shader outlineShader;
    outlineShader.compile(assets::Shader("assets/shaders/outline"));

    gfx::Shader text3DShader;
    text3DShader.compile(assets::Shader("assets/shaders/text-3d"));

    gfx::Shader basic2DShader;
    basic2DShader.compile(assets::Shader("assets/shaders/basic-2d"));

    float ratio = static_cast<float>(window.getFramebufferWidth()) / static_cast<float>(window.getFramebufferHeight());
    float orthoHeight = CHUNK_SIZE * std::sqrt(3.0f);
    float orthoHalfHeight = orthoHeight / 2.0f;

    Camera worldCamera(
        45.0f,
        ratio,
        0.1f, 1000.0f
    );

    Camera uiCamera(
        0.0f, static_cast<float>(window.getWidth()),
        0.0f, static_cast<float>(window.getHeight()),
        0.0f, 1000.0f
    );

    TextRenderer textRenderer(renderer, uiCamera);
    UIRenderer uiRenderer(renderer, uiCamera);

    auto updateWorldCameraProjection = [&](ProjectionType type) {
        if(type == ProjectionType::ORTHOGRAPHIC) {
            worldCamera.setOrthographic(
                -orthoHalfHeight * ratio, orthoHalfHeight * ratio,
                -orthoHalfHeight, orthoHalfHeight,
                0.1f, 1000.0f
            );
        } else {
            worldCamera.setPerspective(
                45.0f,
                ratio,
                0.1f, 1000.0f
            );
        }

        worldCamera.position = glm::vec3(-7.0f, 21.0f, 24.0f);
        worldCamera.rotation = glm::vec3(-30.0f, 45.0f, 0.0f);
    };

    updateWorldCameraProjection(ProjectionType::ORTHOGRAPHIC);

    auto fpsText = std::make_unique<Text>("FPS: ", font, 16);
    fpsText->position = glm::vec3(10.0f, window.getHeight() - 10.0f, 0.0f);
    fpsText->anchor = glm::vec2(0.0f, 1.0f);

    auto cameraText = std::make_unique<Text>("Camera Pos: ", font, 16);
    cameraText->position = glm::vec3(10.0f, window.getHeight() - 30.0f, 0.0f);
    cameraText->anchor = glm::vec2(0.0f, 1.0f);

    auto cameraRotationText = std::make_unique<Text>("Camera Rot: ", font, 16);
    cameraRotationText->position = glm::vec3(10.0f, window.getHeight() - 50.0f, 0.0f);
    cameraRotationText->anchor = glm::vec2(0.0f, 1.0f);

    auto rayHitText = std::make_unique<Text>("Ray Hit: ", font, 16);
    rayHitText->position = glm::vec3(10.0f, window.getHeight() - 70.0f, 0.0f);
    rayHitText->anchor = glm::vec2(0.0f, 1.0f);

    auto blockTypeText = std::make_unique<Text>("Block Type: 1", font, 16);
    blockTypeText->position = glm::vec3(10.0f, window.getHeight() - 90.0f, 0.0f);
    blockTypeText->anchor = glm::vec2(0.0f, 1.0f);

    auto brushSizeText = std::make_unique<Text>("Brush Size: 2x1x2", font, 16);
    brushSizeText->position = glm::vec3(10.0f, 10.0f, 0.0f);
    brushSizeText->anchor = glm::vec2(0.0f, 0.0f);

    auto currentPaletteText = std::make_unique<Text>("Current Palette: ", font, 16);
    currentPaletteText->position = glm::vec3(10.0f, 30.0f, 0.0f);
    currentPaletteText->anchor = glm::vec2(0.0f, 0.0f);

    auto img = std::make_unique<assets::Image>("assets/textures/test.png");
    auto crosshairImage = std::make_unique<assets::Image>("assets/textures/crosshair.png");
    auto btnPlusImage = std::make_unique<assets::Image>("assets/textures/btn_plus.png");
    auto btnMinusImage = std::make_unique<assets::Image>("assets/textures/btn_minus.png");

    auto atlasImage = std::make_unique<assets::Image>("assets/textures/atlas.png");
    
    auto texture = std::make_unique<gfx::Texture>(*img);
    auto crosshairTexture = std::make_unique<gfx::Texture>(*crosshairImage);
    auto atlasTexture = std::make_shared<gfx::Texture>(*atlasImage);
    auto btnPlusTexture = std::make_unique<gfx::Texture>(*btnPlusImage);
    auto btnMinusTexture = std::make_unique<gfx::Texture>(*btnMinusImage);
    
    auto cubeMesh = assets::MeshBuilder::createCube();
    auto planeMesh = assets::MeshBuilder::createPlane();

    auto outlineVAO = renderer.createMeshVAO(cubeMesh);
    auto previewVAO = createCubeMeshVAO(0);
    auto planeVAO = renderer.createMeshVAO(planeMesh);

    bool isWireframe = false;
    bool darkMode = false;

    glm::vec3 placementSize(1.0f, 1.0f, 1.0f);

    unique<World> world;
    try {
        world = World::loadFromAsset(WorldAsset::loadFromFile("assets/saves/world0.dat"));
    } catch (...) {
        world = std::make_unique<World>();
        world->createChunk(0, 0, 0);
    }

    unsigned char blockType = 0;

    std::unordered_map<Chunk*, std::unique_ptr<ChunkMesh>> chunkMeshes;
    std::optional<WorldRayHit> rayHit;

    bool breakingMode = false;
    bool showBoundingBox = true;
    bool isMovingCamera = true;

    glm::dvec2 lastMousePos(0.0, 0.0);
    glm::vec3 camVelocity(0.0f, 0.0f, 0.0f);

    std::unordered_map<int, bool> canPressKey;
    std::unordered_map<int, bool> canPressMouse;
    std::unordered_map<int, bool> isKeyHeld;

    std::vector<glm::vec4> color_palette = {
        glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), // Black
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // White
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // Red
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // Green
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // Blue
    };

    auto paletteTexture = std::make_shared<gfx::Texture>();

    auto sprite = std::make_unique<Sprite>(
        paletteTexture
    );

    sprite->position = glm::vec3(
        currentPaletteText->position.x + 146,
        currentPaletteText->position.y + 5.0f,
        0.0f
    );
    sprite->anchor = glm::vec2(0.0f, 0.5f);
    sprite->size = glm::vec2(12.0f, 12.0f);

    auto set_current_palette_sprite_uvs = [&](int index) {
        const int colorsPerRow = 256;
        const float uvSize = 1.0f / colorsPerRow;

        float u = (index % colorsPerRow) / (float)colorsPerRow;
        float v = (index / colorsPerRow) / (float)colorsPerRow;

        std::cout << "Setting sprite UVs to: " << u << ", " << v << ", " << u + uvSize << ", " << v + uvSize << std::endl;

        sprite->setUVs(glm::vec4(
            u, v,
            u + uvSize, v + uvSize
        ));
    };

    set_current_palette_sprite_uvs(0);

    auto regenerate_palette = [&]() {
        const int width = 256;
        const int height = 256;
        const int channels = 4;

        unsigned char* data = new unsigned char[width * height * channels];
        for(int i = 0; i < color_palette.size(); ++i) {
            int x = i % width;
            int y = i / width;
            if(y >= height) break;

            glm::vec4 color = color_palette[i];
            data[(y * width + x) * channels + 0] = static_cast<unsigned char>(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
            data[(y * width + x) * channels + 1] = static_cast<unsigned char>(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
            data[(y * width + x) * channels + 2] = static_cast<unsigned char>(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);
            data[(y * width + x) * channels + 3] = static_cast<unsigned char>(glm::clamp(color.a, 0.0f, 1.0f) * 255.0f);
        }
        std::fill(
            data + color_palette.size() * channels, 
            data + (width * height * channels), 
            0
        );

        paletteTexture->uploadImage(data, width, height, channels);
        delete[] data;
    };

    regenerate_palette();

    window.setKeyDownCallback([&](int key) {
        for(int i = GLFW_KEY_0; i <= GLFW_KEY_9; ++i) {
            if(key != i) {
                continue;
            }

            if(i == GLFW_KEY_0) {
                blockType = 9;
            } else {
                blockType = static_cast<unsigned char>(i - GLFW_KEY_0 - 1);
            }

            set_current_palette_sprite_uvs(blockType);
            renderCubeMesh(*previewVAO, blockType);
        }

        switch(key) {
            case GLFW_KEY_F:
                isWireframe = !isWireframe;
                renderer.setWireframe(isWireframe);
                break;

            case GLFW_KEY_R:
                try {
                    voxelShader.compile(assets::Shader("assets/shaders/voxel"));
                    textShader.compile(assets::Shader("assets/shaders/text"));
                    gridShader.compile(assets::Shader("assets/shaders/grid"));
                    outlineShader.compile(assets::Shader("assets/shaders/outline"));

                    std::cout << "Shader recompiled successfully." << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Shader recompilation failed: " << e.what() << std::endl;
                }
                break;

            case GLFW_KEY_P:
                if(worldCamera.projectionType == ProjectionType::PERSPECTIVE) {
                    updateWorldCameraProjection(ProjectionType::ORTHOGRAPHIC);
                } else {
                    updateWorldCameraProjection(ProjectionType::PERSPECTIVE);
                }
                break;

            case GLFW_KEY_O:
                showBoundingBox = !showBoundingBox;
                break;

            case GLFW_KEY_B:
                breakingMode = !breakingMode;
                break;

            case GLFW_KEY_I:
                darkMode = !darkMode;
                break;

            case GLFW_KEY_EQUAL:
                if(isKeyHeld[GLFW_KEY_X]) {
                    placementSize.x += 1.0f;
                } else if(isKeyHeld[GLFW_KEY_Y]) {
                    placementSize.y += 1.0f;
                } else if(isKeyHeld[GLFW_KEY_Z]) {
                    placementSize.z += 1.0f;
                } else {
                    worldCamera.orthoSettings.zoom -= 0.25f;
                }
                
                break;

            case GLFW_KEY_MINUS:
                if(isKeyHeld[GLFW_KEY_X]) {
                    placementSize.x = std::max(1.0f, placementSize.x - 1.0f);
                } else if(isKeyHeld[GLFW_KEY_Y]) {
                    placementSize.y = std::max(1.0f, placementSize.y - 1.0f);
                } else if(isKeyHeld[GLFW_KEY_Z]) {
                    placementSize.z = std::max(1.0f, placementSize.z - 1.0f);
                } else {
                    worldCamera.orthoSettings.zoom += 0.25f;
                }

                break;

            case GLFW_KEY_LEFT: case GLFW_KEY_RIGHT: {
                auto hit = getVoxelFromMouse(window, worldCamera, *world, window.getWidth() / 2.0, window.getHeight() / 2.0);
                if(hit.has_value()) {
                    worldCamera.position = glm::vec3(
                        static_cast<float>(hit->block.x) + 0.5f,
                        static_cast<float>(hit->block.y) + 0.5f,
                        static_cast<float>(hit->block.z) + 0.5f
                    );
                }

                if(key == GLFW_KEY_LEFT)
                    worldCamera.rotation.y += 45.0f;
                else if(key == GLFW_KEY_RIGHT)
                    worldCamera.rotation.y -= 45.0f;

                break;
            }
        }

        isKeyHeld[key] = true;
    });

    window.setKeyUpCallback([&](int key) {
        isKeyHeld[key] = false;
    });

    window.setMouseScrollCallback([&](double xoffset, double yoffset) {
        worldCamera.orthoSettings.zoom += static_cast<float>(yoffset) * 0.1f;
    });

    window.setMouseButtonDownCallback([&](int button) {
        if(button == GLFW_MOUSE_BUTTON_RIGHT) {
            //isMovingCamera = true;

            //window.getMousePosition(lastMousePos.x, lastMousePos.y);
            //window.setCursorLocked(true);
        }
    });

    window.setMouseButtonUpCallback([&](int button) {
        if(button == GLFW_MOUSE_BUTTON_RIGHT) {
            //isMovingCamera = false;

            //window.setCursorLocked(false);
        }
    });

    /*window.setMouseMoveCallback([&](double xpos, double ypos) {
        if(!isMovingCamera) {
            return;
        }

        float xoffset = static_cast<float>(xpos - lastMousePos.x);
        float yoffset = static_cast<float>(ypos - lastMousePos.y);

        if(worldCamera.projectionType == ProjectionType::PERSPECTIVE) {
            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            worldCamera.rotation.y += xoffset;
            worldCamera.rotation.x -= yoffset;
        } else {
            float panSpeed = 0.05f * worldCamera.orthoSettings.zoom;

            glm::vec3 right = worldCamera.getRight();
            glm::vec3 up = worldCamera.getUp();

            worldCamera.position -= right * xoffset * panSpeed;
            worldCamera.position += up * yoffset * panSpeed;
        }

        lastMousePos.x = xpos;
        lastMousePos.y = ypos;
    });*/

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    double frameCounter = 0.0;

    while (!window.shouldClose()) {
        window.pollEvents();
        
        auto currentTime = clock::now();
        frameCounter++;

        if(std::chrono::duration<double>(currentTime - lastTime).count() >= 1.0) {
            fpsText->setContent("FPS: " + std::to_string(static_cast<int>(frameCounter)));

            lastTime = currentTime;
            frameCounter = 0.0;
        }

        
        cameraText->setContent(
            "Camera Pos: " +
            std::to_string(static_cast<int>(worldCamera.position.x)) + ", " +
            std::to_string(static_cast<int>(worldCamera.position.y)) + ", " +
            std::to_string(static_cast<int>(worldCamera.position.z))
        );
        cameraRotationText->setContent(
            "Camera Rot: " +
            std::to_string(static_cast<int>(worldCamera.rotation.x)) + ", " +
            std::to_string(static_cast<int>(worldCamera.rotation.y)) + ", " +
            std::to_string(static_cast<int>(worldCamera.rotation.z))
        );

        worldCamera.orthoSettings.zoom = std::clamp(worldCamera.orthoSettings.zoom, 0.5f, 2.0f);

        glm::vec3 cam_forward = worldCamera.getForward();
        glm::vec3 cam_right = worldCamera.getRight();
        glm::vec3 cam_up = worldCamera.getUp();
        
        if(window.getKeyState(GLFW_KEY_W)) {
            if(worldCamera.projectionType == ProjectionType::ORTHOGRAPHIC) {
                worldCamera.position += cam_up * 0.1f;
            } else {
                worldCamera.position += cam_forward * 0.1f;
            }
        }
        
        if(window.getKeyState(GLFW_KEY_S)) {
            if(worldCamera.projectionType == ProjectionType::ORTHOGRAPHIC) {
                worldCamera.position -= cam_up * 0.1f;
            } else {
                worldCamera.position -= cam_forward * 0.1f;
            }
        }
        
        if(window.getKeyState(GLFW_KEY_A)) {
            worldCamera.position -= cam_right * 0.1f;
        }
        
        if(window.getKeyState(GLFW_KEY_D)) {
            worldCamera.position += cam_right * 0.1f;
        }
        
        if(worldCamera.projectionType == ProjectionType::PERSPECTIVE) {
            if(window.getKeyState(GLFW_KEY_Q)) {
                worldCamera.position.y += 0.1f;
            }
            
            if(window.getKeyState(GLFW_KEY_E)) {
                worldCamera.position.y -= 0.1f;
            }
        }

        /*if(window.getKeyState(GLFW_KEY_LEFT)) {
            worldCamera.rotation.y -= 1.0f;
        }

        if(window.getKeyState(GLFW_KEY_RIGHT)) {
            worldCamera.rotation.y += 1.0f;
        }*/

        /*if(window.getKeyState(GLFW_KEY_J)) {
            worldCamera.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        }*/

        blockTypeText->setContent("Block Type: " + std::to_string(static_cast<int>(blockType)));

        if(window.getMouseButtonState(GLFW_MOUSE_BUTTON_LEFT)) {
            if(canPressMouse.at(GLFW_MOUSE_BUTTON_LEFT) && rayHit.has_value()) {
                int x = breakingMode ? static_cast<int>(rayHit->block.x) : static_cast<int>(rayHit->block.x + rayHit->side.x);
                int y = breakingMode ? static_cast<int>(rayHit->block.y) : static_cast<int>(rayHit->block.y + rayHit->side.y);
                int z = breakingMode ? static_cast<int>(rayHit->block.z) : static_cast<int>(rayHit->block.z + rayHit->side.z);

                for(int rx = 0; rx < placementSize.x; ++rx) {
                    for(int ry = 0; ry < placementSize.y; ++ry) {
                        for(int rz = 0; rz < placementSize.z; ++rz) {
                            int px = x + rx;
                            int py = y + ry;
                            int pz = z + rz;

                            if(px >= 0 && py >= 0 && pz >= 0) {
                                auto chunk = world->getChunkContainingBlock(px, py, pz);
                                if(!chunk) {
                                    chunk = world->createChunk(
                                        px / CHUNK_SIZE,
                                        py / CHUNK_SIZE,
                                        pz / CHUNK_SIZE
                                    );
                                } else {
                                    chunk->setBlock(px % CHUNK_SIZE, py % CHUNK_SIZE, pz % CHUNK_SIZE, breakingMode ? 0 : blockType + 1);
                                    std::cout << (breakingMode ? "Broke" : "Placed") << " block " << static_cast<int>(blockType) << " at " << px << ", " << py << ", " << pz << std::endl;
                                }
                            }
                        }
                    }
                }

                canPressMouse[GLFW_MOUSE_BUTTON_LEFT] = false;
            }
        } else {
            canPressMouse[GLFW_MOUSE_BUTTON_LEFT] = true;
        }

        rayHit.reset();

        glm::dvec2 currentMouse;
        window.getMousePosition(currentMouse.x, currentMouse.y);

        //std::cout << "Mouse Position: " << currentMouse.x << ", " << currentMouse.y << std::endl;

        if(window.getMouseButtonState(GLFW_MOUSE_BUTTON_RIGHT)) {
            if(!isMovingCamera) {
                window.setCursorLocked(true);
                window.setMousePosition(currentMouse.x, currentMouse.y);
            }

            isMovingCamera = true;
        } else {
            if(isMovingCamera) {
                window.setCursorLocked(false);
                window.setMousePosition(currentMouse.x, currentMouse.y);
            }

            isMovingCamera = false;
        }

        rayHit = getVoxelFromMouse(window, worldCamera, *world, currentMouse.x, window.getHeight() - currentMouse.y);

        if(rayHit.has_value()) {
            rayHitText->setContent(
                "Ray Hit: " +
                std::to_string(static_cast<int>(rayHit->block.x)) + ", " +
                std::to_string(static_cast<int>(rayHit->block.y)) + ", " +
                std::to_string(static_cast<int>(rayHit->block.z))
            );
        } else {
            rayHitText->setContent("Ray Hit: None");
        }

        if(worldCamera.projectionType == ProjectionType::ORTHOGRAPHIC) {
            const float desiredDistance = 80.0f;
            float distance = 0.0f;

            auto centerHit = getVoxelFromMouse(window, worldCamera, *world, window.getWidth() / 2.0, window.getHeight() / 2.0);
            if(centerHit.has_value()) {
                distance = centerHit->distance;
            } else {
                distance = worldCamera.position.y / cam_forward.y;
            }

            worldCamera.position -= cam_forward * (desiredDistance - distance);
        }

        if(isMovingCamera) {
            float xoffset = static_cast<float>(currentMouse.x - lastMousePos.x);
            float yoffset = static_cast<float>(currentMouse.y - lastMousePos.y);

            if(worldCamera.projectionType == ProjectionType::PERSPECTIVE) {
                float sensitivity = 0.1f;
                xoffset *= sensitivity;
                yoffset *= sensitivity;

                worldCamera.rotation.y += xoffset;
                worldCamera.rotation.x -= yoffset;
            } else {
                float panSpeed = 0.05f * worldCamera.orthoSettings.zoom;

                glm::vec3 right = worldCamera.getRight();
                glm::vec3 up = worldCamera.getUp();

                worldCamera.position -= right * xoffset * panSpeed;
                worldCamera.position += up * yoffset * panSpeed;
            }
        }

        //std::cout << "Delta Mouse: " << (currentMouse.x - lastMousePos.x) << ", " << (currentMouse.y - lastMousePos.y) << std::endl;
        lastMousePos = currentMouse;

        brushSizeText->setContent(
            "Brush Size: " +
            std::to_string(static_cast<int>(placementSize.x)) + "/" +
            std::to_string(static_cast<int>(placementSize.y)) + "/" +
            std::to_string(static_cast<int>(placementSize.z))
        );

        for(const auto& [key, chunk] : world->getChunks()) {
            if(!chunk->isDirty()) {
                continue;
            }

            if(chunkMeshes.find(chunk.get()) == chunkMeshes.end()) {
                chunkMeshes[chunk.get()] = std::make_unique<ChunkMesh>(chunk.get());
            } else {
                chunkMeshes[chunk.get()]->updateMesh();
            }
        }

        renderer.setViewport(0, 0, window.getFramebufferWidth(), window.getFramebufferHeight());

        if(isWireframe) {
            renderer.clear(0.6, 0.6, 0.6, 1.0);
        } else {
            if(darkMode) {
                renderer.clear(0.0, 0.0, 0.0, 1.0);
            } else {
                renderer.clear(0.6, 0.6, 0.6, 1.0);
            }
        }

        renderer.setDepthTest(true);
        renderer.setCullFace(true);

        renderer.useShader(gridShader);
        gridShader.setUniformMat4("model", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(worldCamera.position.x, 0.0f, worldCamera.position.z))));
        gridShader.setUniformMat4("view", glm::value_ptr(worldCamera.getViewMatrix()));
        gridShader.setUniformMat4("projection", glm::value_ptr(worldCamera.getProjectionMatrix()));
        auto gridColor = !darkMode ? glm::vec4(0.2f, 0.2f, 0.2f, 0.3f) : glm::vec4(0.8f, 0.8f, 0.8f, 0.3f);
        gridShader.setUniformVec4("borderColor", gridColor.r, gridColor.g, gridColor.b, gridColor.a);

        renderer.setCullFace(false);
        renderer.drawEmpty(6);
        renderer.setCullFace(true);

        renderer.useShader(voxelShader);
        renderer.bindTexture(0, *paletteTexture);

        voxelShader.setUniformMat4("view", glm::value_ptr(worldCamera.getViewMatrix()));
        voxelShader.setUniformMat4("projection", glm::value_ptr(worldCamera.getProjectionMatrix()));
        voxelShader.setUniformInt("useTexture", 1);
        voxelShader.setUniformInt("useLight", 1);
        voxelShader.setUniformVec4("colorTint", 1.0f, 1.0f, 1.0f, 1.0f);

        for(const auto& [chunkPtr, chunkMesh] : chunkMeshes) {
            glm::mat4 model = glm::translate(
                glm::mat4(1.0f),
                glm::vec3(
                    chunkPtr->getX() * CHUNK_SIZE,
                    chunkPtr->getY() * CHUNK_SIZE,
                    chunkPtr->getZ() * CHUNK_SIZE
                )
            );

            voxelShader.setUniformMat4("model", glm::value_ptr(model));
            renderer.drawVAO(chunkMesh->getVertexArray());
        }

        if(rayHit.has_value() && !isMovingCamera) {
            int x = breakingMode ? static_cast<int>(rayHit->block.x) : static_cast<int>(rayHit->block.x + rayHit->side.x);
            int y = breakingMode ? static_cast<int>(rayHit->block.y) : static_cast<int>(rayHit->block.y + rayHit->side.y);
            int z = breakingMode ? static_cast<int>(rayHit->block.z) : static_cast<int>(rayHit->block.z + rayHit->side.z);

            if(x >= 0 && y >= 0 && z >= 0) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
                                        glm::scale(glm::mat4(1.0f), placementSize);
                
                voxelShader.setUniformInt("useLight", 0);
                voxelShader.setUniformMat4("model", glm::value_ptr(model));

                if(!breakingMode) {
                    voxelShader.setUniformInt("useTexture", 1);
                    voxelShader.setUniformVec4("colorTint", 1.0f, 1.0f, 1.0f, 0.6f);
                    renderer.enablePolygonOffsetFill(1.0f, 1.0f);
                } else {
                    voxelShader.setUniformInt("useTexture", 0);
                    voxelShader.setUniformVec4("colorTint", 0.8f, 0.2f, 0.2f, 0.6f);
                    renderer.enablePolygonOffsetFill(-1.0f, -1.0f);
                }

                renderer.drawVAO(*previewVAO, gfx::RenderMode::TRIANGLES);
                renderer.disablePolygonOffsetFill();

                model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z) + glm::vec3(0.5f, 0.5f, 0.5f) * placementSize) * 
                                    glm::scale(glm::mat4(1.0f), placementSize);

                renderer.useShader(outlineShader);

                outlineShader.setUniformMat4("view", glm::value_ptr(worldCamera.getViewMatrix()));
                outlineShader.setUniformMat4("projection", glm::value_ptr(worldCamera.getProjectionMatrix()));
                outlineShader.setUniformMat4("model", glm::value_ptr(model));

                if(!breakingMode) {
                    outlineShader.setUniformVec4("borderColor", 0.6f, 0.6f, 0.6f, 0.8f);
                    renderer.enablePolygonOffsetFill(0.9f, 0.9f);
                } else {
                    outlineShader.setUniformVec4("borderColor", 0.5f, 0.2f, 0.2f, 0.8f);
                    renderer.enablePolygonOffsetFill(-1.1f, -1.1f);
                }

                //renderer.drawVAO(*outlineVAO, gfx::RenderMode::TRIANGLES);
                renderer.disablePolygonOffsetFill();
            }
        }

        if(showBoundingBox) {
            renderer.useShader(outlineShader);
            
            outlineShader.setUniformMat4("view", glm::value_ptr(worldCamera.getViewMatrix()));
            outlineShader.setUniformMat4("projection", glm::value_ptr(worldCamera.getProjectionMatrix()));
            outlineShader.setUniformMat4("model", glm::value_ptr(
                glm::translate(glm::mat4(1.0f), glm::vec3(CHUNK_SIZE / 2, CHUNK_SIZE / 2, CHUNK_SIZE / 2)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE))
            ));

            outlineShader.setUniformVec4("borderColor", 0.8f, 0.8f, 0.8f, 1.0f);

            renderer.enablePolygonOffsetFill(-1.0f, -1.0f);
            renderer.setCullFace(false);
            renderer.drawVAO(*outlineVAO, gfx::RenderMode::TRIANGLES);
            renderer.setCullFace(true);
            renderer.disablePolygonOffsetFill();
        }

        renderer.setDepthTest(false);

        auto transform2D = Transform2D(
            glm::vec2(
                window.getWidth() / 2.0f,
                window.getHeight() / 2.0f
            ),
            0.0f,
            glm::vec2(600.0f, 512.0f)
        );

        /*uiRenderer.renderQuad(
            basic2DShader,
            *crosshairTexture,
            transform2D
        );*/
        uiRenderer.renderSprite(basic2DShader, *sprite);

        textRenderer.renderText(textShader, *fpsText);
        textRenderer.renderText(textShader, *cameraText);
        textRenderer.renderText(textShader, *cameraRotationText);
        textRenderer.renderText(textShader, *rayHitText);
        textRenderer.renderText(textShader, *blockTypeText);
        textRenderer.renderText(textShader, *brushSizeText);
        textRenderer.renderText(textShader, *currentPaletteText);

        window.swapBuffers();
    }

    auto worldAsset = World::saveToAsset(*world);
    worldAsset.saveToFile("assets/saves/world0.dat");

    return 0;
}