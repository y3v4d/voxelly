#include "engine/engine.hpp"
#include "chunk.hpp"
#include "world.hpp"
#include "text_renderer.hpp"
#include "chunk_mesh.hpp"

#include <chrono>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

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

int main() {
    auto font = std::make_shared<assets::Font>("assets/fonts/Roboto-Medium.ttf");

    core::Window window(1280, 720, "Voxelly | v0.0.1");
    
    std::cout << "Window created: " << window.getWidth() << "x" << window.getHeight() << std::endl;
    std::cout << "Framebuffer size: " << window.getFramebufferWidth() << "x" << window.getFramebufferHeight() << std::endl;
    
    gfx::RendererGL renderer(window);
    renderer.setViewport(0, 0, window.getFramebufferWidth(), window.getFramebufferHeight());

    TextRenderer textRenderer(renderer);

    gfx::Shader voxelShader;
    voxelShader.compile(assets::Shader("assets/shaders/voxel"));

    gfx::Shader textShader;
    textShader.compile(assets::Shader("assets/shaders/text"));

    gfx::Shader gridShader;
    gridShader.compile(assets::Shader("assets/shaders/grid"));

    gfx::Shader outlineShader;
    outlineShader.compile(assets::Shader("assets/shaders/outline"));

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

    auto updateWorldCameraProjection = [&](ProjectionType type) {
        if(type == ProjectionType::ORTHOGRAPHIC) {
            worldCamera.setOrthographic(
                -orthoHalfHeight * ratio, orthoHalfHeight * ratio,
                -orthoHalfHeight, orthoHalfHeight,
                0.0f, 1000.0f
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
    fpsText->position = glm::vec3(10.0f, window.getHeight() - 20.0f, 0.0f);

    auto cameraText = std::make_unique<Text>("Camera Pos: ", font, 16);
    cameraText->position = glm::vec3(10.0f, window.getHeight() - 40.0f, 0.0f);

    auto cameraRotationText = std::make_unique<Text>("Camera Rot: ", font, 16);
    cameraRotationText->position = glm::vec3(10.0f, window.getHeight() - 60.0f, 0.0f);

    auto rayHitText = std::make_unique<Text>("Ray Hit: ", font, 16);
    rayHitText->position = glm::vec3(10.0f, window.getHeight() - 80.0f, 0.0f);

    auto blockTypeText = std::make_unique<Text>("Block Type: 1", font, 16);
    blockTypeText->position = glm::vec3(10.0f, window.getHeight() - 100.0f, 0.0f);

    auto img = std::make_unique<assets::Image>("assets/textures/test.png");
    auto crosshairImage = std::make_unique<assets::Image>("assets/textures/crosshair.png");

    auto atlasImage = std::make_unique<assets::Image>("assets/textures/atlas.png");
    
    auto texture = std::make_unique<gfx::Texture>(*img);
    auto crosshairTexture = std::make_unique<gfx::Texture>(*crosshairImage);
    auto atlasTexture = std::make_unique<gfx::Texture>(*atlasImage);
    
    auto outlineMesh = assets::MeshBuilder::createOutlineCube();
    auto previewMesh = assets::MeshBuilder::createCube();

    auto outlineVAO = renderer.createMeshVAO(outlineMesh);
    auto previewVAO = createCubeMeshVAO(0);

    bool isWireframe = false;
    bool canToggleWireframe = true;

    World world;
    world.createChunk(0, 0, 0);

    unsigned char blockType = 1;

    std::unordered_map<Chunk*, std::unique_ptr<ChunkMesh>> chunkMeshes;
    std::optional<WorldRayHit> rayHit;

    bool canRecompileShader = true;
    bool breakingMode = true;

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();

    while (!window.shouldClose()) {
        //std::cout << "camera front: " << camera_front.x << ", " << camera_front.y << ", " << camera_front.z << std::endl;
        auto currentTime = clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        fpsText->setContent("FPS: " + std::to_string(static_cast<int>(1.0f / deltaTime.count())));
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

        glm::vec3 cam_forward = worldCamera.getForward();
        glm::vec3 cam_right = worldCamera.getRight();

        if(window.getKeyState(GLFW_KEY_W)) {
            worldCamera.position += cam_forward * 0.1f;
        }
        
        if(window.getKeyState(GLFW_KEY_S)) {
            worldCamera.position -= cam_forward * 0.1f;
        }
        
        if(window.getKeyState(GLFW_KEY_A)) {
            worldCamera.position -= cam_right * 0.1f;
        }
        
        if(window.getKeyState(GLFW_KEY_D)) {
            worldCamera.position += cam_right * 0.1f;
        }
        
        if(window.getKeyState(GLFW_KEY_Q)) {
            worldCamera.position.y += 0.1f;
        }
        
        if(window.getKeyState(GLFW_KEY_E)) {
            worldCamera.position.y -= 0.1f;
        }

        if(window.getKeyState(GLFW_KEY_UP)) {
            worldCamera.rotation.x += 1.0f;
        }

        if(window.getKeyState(GLFW_KEY_DOWN)) {
            worldCamera.rotation.x -= 1.0f;
        }

        if(window.getKeyState(GLFW_KEY_LEFT)) {
            worldCamera.rotation.y -= 1.0f;
        }

        if(window.getKeyState(GLFW_KEY_RIGHT)) {
            worldCamera.rotation.y += 1.0f;
        }

        if(window.getKeyState(GLFW_KEY_J)) {
            worldCamera.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        if(window.getKeyState(GLFW_KEY_F)) {
            if(canToggleWireframe) {
                renderer.setWireframe(!isWireframe);
                isWireframe = !isWireframe;
                canToggleWireframe = false;
            }
        } else {
            canToggleWireframe = true;
        }

        if(window.getKeyState(GLFW_KEY_R)) {
            if(canRecompileShader) {
                try {
                    voxelShader.compile(assets::Shader("assets/shaders/voxel"));
                    textShader.compile(assets::Shader("assets/shaders/text"));
                    gridShader.compile(assets::Shader("assets/shaders/grid"));

                    std::cout << "Shader recompiled successfully." << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Shader recompilation failed: " << e.what() << std::endl;
                }

                canRecompileShader = false;
            }
        } else {
            canRecompileShader = true;
        }

        if(window.getKeyState(GLFW_KEY_P)) {
            static float projectionToggleTimer = 0.0f;
            projectionToggleTimer += deltaTime.count();

            if(projectionToggleTimer >= 0.1f) {
                if(worldCamera.projectionType == ProjectionType::PERSPECTIVE) {
                    updateWorldCameraProjection(ProjectionType::ORTHOGRAPHIC);
                } else {
                    updateWorldCameraProjection(ProjectionType::PERSPECTIVE);
                }

                projectionToggleTimer = 0.0f;
            }
        }

        if(window.getKeyState(GLFW_KEY_B)) {
            static float modeToggleTimer = 0.0f;
            modeToggleTimer += deltaTime.count();

            if(modeToggleTimer >= 0.1f) {
                breakingMode = !breakingMode;
                modeToggleTimer = 0.0f;
            }
        }

        for(int i = 0; i < 4; ++i) {
            if(window.getKeyState(GLFW_KEY_1 + i)) {
                blockType = i + 1;
                renderCubeMesh(*previewVAO, blockType - 1);
            }
        }

        blockTypeText->setContent("Block Type: " + std::to_string(static_cast<int>(blockType)));

        if(window.getMouseButtonState(GLFW_MOUSE_BUTTON_LEFT)) {
            static float actionTimer = 0.0f;
            actionTimer += deltaTime.count();

            if(actionTimer >= 0.1f && rayHit.has_value()) {
                int x = breakingMode ? static_cast<int>(rayHit->block.x) : static_cast<int>(rayHit->block.x + rayHit->side.x);
                int y = breakingMode ? static_cast<int>(rayHit->block.y) : static_cast<int>(rayHit->block.y + rayHit->side.y);
                int z = breakingMode ? static_cast<int>(rayHit->block.z) : static_cast<int>(rayHit->block.z + rayHit->side.z);

                if(x >= 0 && y >= 0 && z >= 0) {
                    auto chunk = world.getChunkContainingBlock(x, y, z);
                    if(!chunk) {
                        chunk = world.createChunk(
                            x / CHUNK_SIZE,
                            y / CHUNK_SIZE,
                            z / CHUNK_SIZE
                        );
                    } else {
                        chunk->setBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, breakingMode ? 0 : blockType);
                        std::cout << (breakingMode ? "Broke" : "Placed") << " block " << static_cast<int>(blockType) << " at " << x << ", " << y << ", " << z << std::endl;
                    }
                }

                actionTimer = 0.0f;
            }
        }

        rayHit.reset();

        {
            double mouseX, mouseY;
            window.getMousePosition(mouseX, mouseY);

            mouseY = window.getHeight() - mouseY;

            glm::vec3 nearPlane = glm::unProject(
                glm::vec3(mouseX, mouseY, 0.0f),
                worldCamera.getViewMatrix(),
                worldCamera.projectionMatrix,
                glm::ivec4(0, 0, window.getWidth(), window.getHeight())
            );

            glm::vec3 farPlane = glm::unProject(
                glm::vec3(mouseX, mouseY, 1.0f),
                worldCamera.getViewMatrix(),
                worldCamera.projectionMatrix,
                glm::ivec4(0, 0, window.getWidth(), window.getHeight())
            );

            glm::vec3 rayOrigin = nearPlane;
            glm::vec3 rayDir = glm::normalize(farPlane - nearPlane);

            rayHit = world.findRayHit(
                rayOrigin,
                rayDir,
                256.0f
            );
        }

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

        for(const auto& [key, chunk] : world.getChunks()) {
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
        renderer.clear(0.6, 0.6, 0.6, 1.0);

        renderer.setDepthTest(true);
        renderer.useShader(voxelShader);
        renderer.bindTexture(0, *atlasTexture);

        voxelShader.setUniformMat4("view", glm::value_ptr(worldCamera.getViewMatrix()));
        voxelShader.setUniformMat4("projection", glm::value_ptr(worldCamera.projectionMatrix));
        voxelShader.setUniformInt("useTexture", 1);
        voxelShader.setUniformInt("useLight", 1);
        voxelShader.setUniformVec4("colorTint", 0.0f, 0.0f, 0.0f, 0.0f);

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

        voxelShader.setUniformMat4("model", glm::value_ptr(glm::mat4(1.0f)));

        if(rayHit.has_value()) {
            int x = breakingMode ? static_cast<int>(rayHit->block.x) : static_cast<int>(rayHit->block.x + rayHit->side.x);
            int y = breakingMode ? static_cast<int>(rayHit->block.y) : static_cast<int>(rayHit->block.y + rayHit->side.y);
            int z = breakingMode ? static_cast<int>(rayHit->block.z) : static_cast<int>(rayHit->block.z + rayHit->side.z);

            if(!breakingMode) {
                voxelShader.setUniformInt("useTexture", 1);
                voxelShader.setUniformInt("useLight", 0);

                voxelShader.setUniformMat4("model", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))));
                voxelShader.setUniformVec4("colorTint", 0.2f, 0.2f, 0.2f, 0.6f);

                renderer.drawVAO(*previewVAO, gfx::RenderMode::TRIANGLES);
            } else {
                renderer.useShader(outlineShader);

                outlineShader.setUniformMat4("view", glm::value_ptr(worldCamera.getViewMatrix()));
                outlineShader.setUniformMat4("projection", glm::value_ptr(worldCamera.projectionMatrix));
                outlineShader.setUniformMat4("model", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f))));

                renderer.drawVAO(*outlineVAO, gfx::RenderMode::LINES);
            }
        }

        renderer.setDepthTest(false);

        renderer.useShader(textShader);

        textShader.setUniformMat4("view", glm::value_ptr(uiCamera.getViewMatrix()));
        textShader.setUniformMat4("projection", glm::value_ptr(uiCamera.projectionMatrix));

        textShader.setUniformMat4("model", glm::value_ptr(fpsText->getModelMatrix()));
        textRenderer.renderText(*fpsText, 0, 0);

        textShader.setUniformMat4("model", glm::value_ptr(cameraText->getModelMatrix()));
        textRenderer.renderText(*cameraText, 0, 0);

        textShader.setUniformMat4("model", glm::value_ptr(cameraRotationText->getModelMatrix()));
        textRenderer.renderText(*cameraRotationText, 0, 0);

        textShader.setUniformMat4("model", glm::value_ptr(rayHitText->getModelMatrix()));
        textRenderer.renderText(*rayHitText, 0, 0);

        textShader.setUniformMat4("model", glm::value_ptr(blockTypeText->getModelMatrix()));
        textRenderer.renderText(*blockTypeText, 0, 0);

        renderer.setDepthTest(true);
        renderer.setCullFace(false);

        renderer.useShader(gridShader);
        gridShader.setUniformMat4("model", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(worldCamera.position.x, 0.0f, worldCamera.position.z))));
        gridShader.setUniformMat4("view", glm::value_ptr(worldCamera.getViewMatrix()));
        gridShader.setUniformMat4("projection", glm::value_ptr(worldCamera.projectionMatrix));

        renderer.drawEmpty(6);
        renderer.setCullFace(true);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}