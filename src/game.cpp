#include "game.hpp"

std::vector<glm::ivec3> getVoxelsForTool(const World& world, const glm::ivec3& center, int brushSize, ToolShape shape) {
    std::vector<glm::ivec3> voxels;

    if(shape == ToolShape::SHAPE_SPHERE) {
        voxels = world.getVoxelsInSphere(center, brushSize);
    } else if(shape == ToolShape::SHAPE_CUBE) {
        glm::ivec3 min = center - glm::ivec3(brushSize);
        glm::ivec3 max = center + glm::ivec3(brushSize);
        voxels = world.getVoxelsInCube(min, max);
    }

    return voxels;
}

std::optional<glm::ivec3> find_lowest_voxel_on_xz(const std::vector<glm::ivec3>& voxels, int x, int z) {
    if(voxels.empty()) {
        return std::nullopt;
    }

    glm::ivec3 lowestVoxel(0, 0, 0);
    bool found = false;
    for(const auto& voxel : voxels) {
        if(!found || (voxel.x == x && voxel.z == z && voxel.y < lowestVoxel.y)) {
            lowestVoxel = voxel;
            found = true;
        }
    }

    if(!found) {
        return std::nullopt;
    } else {
        return lowestVoxel;
    }
}

void Game::init() {
    y_plane = -1;

    assetManager = std::make_unique<assets::AssetManager>();
    window = std::make_unique<core::Window>(1280, 720, "Voxelly | v0.0.2");

    std::cout << "Filesystem Base Path: " << core::FileSystem::getExecutablePath() << std::endl;
    std::cout << "Filesystem Data Path: " << core::FileSystem::getDataPath() << std::endl;
    
    std::cout << "Window created: " << window->getWidth() << "x" << window->getHeight() << std::endl;
    std::cout << "Framebuffer size: " << window->getFramebufferWidth() << "x" << window->getFramebufferHeight() << std::endl;

    std::cout << "Initializing renderer..." << std::endl;
    
    renderer = std::make_unique<gfx::RendererGL>(*window);
    renderer->setViewport(0, 0, window->getFramebufferWidth(), window->getFramebufferHeight());

    std::cout << "Compiling shaders..." << std::endl;

    std::cout << "Compiling Voxel Shader..." << std::endl;
    voxelShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/voxel.glsl"));
    std::cout << "Compiling Text Shader..." << std::endl;
    textShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/text.glsl"));
    std::cout << "Compiling Grid Shader..." << std::endl;
    gridShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/grid.glsl"));
    std::cout << "Compiling Outline Shader..." << std::endl;
    outlineShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/outline.glsl"));
    std::cout << "Compiling Text 3D Shader..." << std::endl;
    text3DShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/text-3d.glsl"));
    std::cout << "Compiling Basic 2D Shader..." << std::endl;
    basic2DShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/basic-2d.glsl"));
    std::cout << "Setting up cameras..." << std::endl;

    float ratio = static_cast<float>(window->getFramebufferWidth()) / static_cast<float>(window->getFramebufferHeight());
    worldCamera = std::make_shared<Camera>(
        45.0f,
        ratio,
        0.1f, 1000.0f
    );

    uiCamera = std::make_shared<Camera>(
        0.0f, static_cast<float>(window->getWidth()),
        0.0f, static_cast<float>(window->getHeight()),
        0.0f, 1000.0f
    );

    std::cout << "Setting up text and UI renderers..." << std::endl;

    font = assetManager->loadAsset<assets::Font>("assets/fonts/kenney-mini-square.ttf");

    textRenderer = std::make_unique<TextRenderer>(*renderer, *uiCamera);
    uiRenderer = std::make_unique<UIRenderer>(*renderer, *uiCamera);

    updateWorldCameraProjection(ProjectionType::ORTHOGRAPHIC);

    fpsText = std::make_unique<Text>("FPS: -", font, 16);
    fpsText->anchor = glm::vec2(1.0f, 1.0f);

    cameraText = std::make_unique<Text>("Camera Pos: ", font, 16);
    cameraText->anchor = glm::vec2(0.0f, 1.0f);

    cameraRotationText = std::make_unique<Text>("Camera Rot: ", font, 16);
    cameraRotationText->anchor = glm::vec2(0.0f, 1.0f);

    rayHitText = std::make_unique<Text>("Ray Hit: ", font, 16);
    rayHitText->anchor = glm::vec2(0.0f, 1.0f);

    brushSizeText = std::make_unique<Text>("Brush Size: 2x1x2", font, 16);
    brushSizeText->anchor = glm::vec2(0.0f, 0.0f);

    currentPaletteText = std::make_unique<Text>("Current Palette: ", font, 16);
    currentPaletteText->anchor = glm::vec2(0.0f, 0.0f);

    drawCallText = std::make_unique<Text>("Draw Calls: 0", font, 16);
    drawCallText->anchor = glm::vec2(1.0f, 1.0f);

    currentToolText = std::make_unique<Text>("Tool: Place", font, 16);
    currentToolText->anchor = glm::vec2(0.0f, 0.0f);

    axisLockText = std::make_unique<Text>("Axis Lock: None", font, 16);
    axisLockText->anchor = glm::vec2(0.0f, 0.0f);

    toolShapeText = std::make_unique<Text>("Tool Shape: Sphere", font, 16);
    toolShapeText->anchor = glm::vec2(0.0f, 0.0f);

    paletteTexture = std::make_shared<gfx::Texture>();

    sprite = std::make_unique<Sprite>(
        paletteTexture
    );

    sprite->anchor = glm::vec2(0.0f, 0.5f);
    sprite->size = glm::vec2(12.0f, 12.0f);

    auto whiteImg = assets::Image(1, 1, 4);
    whiteImg.setPixel(0, 0, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    uiBackgroundSprite = std::make_unique<Sprite>(
        std::make_shared<gfx::Texture>(whiteImg)
    );

    construct_ui();

    auto img = assetManager->loadAsset<assets::Image>("assets/textures/test.png");
    auto crosshairImage = assetManager->loadAsset<assets::Image>("assets/textures/crosshair.png");
    auto btnPlusImage = assetManager->loadAsset<assets::Image>("assets/textures/btn_plus.png");
    auto btnMinusImage = assetManager->loadAsset<assets::Image>("assets/textures/btn_minus.png");

    auto atlasImage = assetManager->loadAsset<assets::Image>("assets/textures/atlas.png");
    
    auto texture = std::make_unique<gfx::Texture>(*img);
    auto crosshairTexture = std::make_unique<gfx::Texture>(*crosshairImage);
    auto atlasTexture = std::make_shared<gfx::Texture>(*atlasImage);
    
    auto cubeMesh = assets::MeshBuilder::createCube();

    outlineVAO = renderer->createMeshVAO(cubeMesh);
    previewVAO = createCubeMeshVAO(0);
    
    try {
        world = World::loadFromAsset(WorldAsset::loadFromFile("assets/examples/chess.dat"));
    } catch (...) {
        world = std::make_unique<World>();
        world->createChunk(0, 0, 0);
    }

    worldMesh = std::make_unique<WorldMesh>(world.get());

    toolWorld = std::make_unique<World>();
    toolWorldMesh = std::make_unique<WorldMesh>(toolWorld.get());

    set_current_palette_sprite_uvs(0);
    regenerate_palette();

    window->setKeyDownCallback([&](int key) {
        switch(key) {
            case SDL_SCANCODE_1:
                currentTool = ToolType::TOOL_PLACE;
                update_current_tool_text();
                break;
            case SDL_SCANCODE_2:
                currentTool = ToolType::TOOL_BRUSH;
                update_current_tool_text();
                break;
            case SDL_SCANCODE_3:
                currentTool = ToolType::TOOL_ERASE;
                update_current_tool_text();
                break;
            case SDL_SCANCODE_4:
                currentTool = ToolType::TOOL_LINE;
                lineInProgress = false;
                update_current_tool_text();
                break;
            case SDL_SCANCODE_5:
                currentTool = ToolType::TOOL_MOVE;
                moveVoxels.clear();
                moveVoxelsStart = glm::ivec3(0, 0, 0);
                update_current_tool_text();
                break;

            case SDL_SCANCODE_F:
                isWireframe = !isWireframe;
                renderer->setWireframe(isWireframe);
                break;

            case SDL_SCANCODE_R:
                try {
                    voxelShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/voxel.glsl", true));
                    textShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/text.glsl", true));
                    gridShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/grid.glsl", true));
                    outlineShader.compile(*assetManager->loadAsset<assets::Shader>("assets/shaders/outline.glsl", true));

                    std::cout << "Shader recompiled successfully." << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Shader recompilation failed: " << e.what() << std::endl;
                }
                break;

            case SDL_SCANCODE_P:
                if(worldCamera->projectionType == ProjectionType::PERSPECTIVE) {
                    updateWorldCameraProjection(ProjectionType::ORTHOGRAPHIC);
                } else {
                    updateWorldCameraProjection(ProjectionType::PERSPECTIVE);
                }
                break;

            case SDL_SCANCODE_O:
                showBoundingBox = !showBoundingBox;
                break;

            case SDL_SCANCODE_I:
                darkMode = !darkMode;
                break;

            case SDL_SCANCODE_EQUALS:
                if(isKeyHeld[SDL_SCANCODE_LSHIFT]) {
                    brushSize += 1;
                    brushSize = std::min(brushSize, 16);
                } else {
                    worldCamera->orthoSettings.zoom -= 0.25f;
                }
                
                break;

            case SDL_SCANCODE_MINUS:
                if(isKeyHeld[SDL_SCANCODE_LSHIFT]) {
                    brushSize = std::max(0, brushSize - 1);
                } else {
                    worldCamera->orthoSettings.zoom += 0.25f;
                }

                break;

            case SDL_SCANCODE_LEFT: case SDL_SCANCODE_RIGHT: {
                auto centerRay = worldCamera->getRayFromScreenCoords(
                    window->getWidth() / 2.0f,
                    window->getHeight() / 2.0f,
                    static_cast<float>(window->getWidth()),
                    static_cast<float>(window->getHeight())
                );

                float y0Dist = (8.0f - centerRay.origin.y) / centerRay.direction.y;
                if(y0Dist < 0) {
                    break;
                }

                glm::vec3 hitPoint;
                centerRay.at(y0Dist, hitPoint);

                worldCamera->position = glm::vec3(
                    static_cast<float>(hitPoint.x),
                    8.0f,
                    static_cast<float>(hitPoint.z)
                );

                if(key == SDL_SCANCODE_LEFT)
                    worldCamera->rotation.y += 90.0f;
                else if(key == SDL_SCANCODE_RIGHT)
                    worldCamera->rotation.y -= 90.0f;

                break;
            }

            case SDL_SCANCODE_UP: 
                blockType = (blockType + 1) %  color_palette.size();
                set_current_palette_sprite_uvs(blockType);
                break;

            case SDL_SCANCODE_DOWN: 
                blockType = (blockType - 1 + color_palette.size()) % color_palette.size();
                set_current_palette_sprite_uvs(blockType);
                break;

            case SDL_SCANCODE_TAB:
                toolShape = (toolShape == ToolShape::SHAPE_SPHERE) ? ToolShape::SHAPE_CUBE : ToolShape::SHAPE_SPHERE;
                break;

            case SDL_SCANCODE_X:
                if(axisLock == AxisLock::AXIS_LOCK_X) {
                    axisLock = AxisLock::AXIS_LOCK_NONE;
                } else {
                    axisLock = AxisLock::AXIS_LOCK_X;
                    axisLockPosition = brushHit.has_value() ? brushHit->block : glm::ivec3(0, 0, 0);
                }

                update_axis_lock_text();
                break;

            case SDL_SCANCODE_Y:
                if(axisLock == AxisLock::AXIS_LOCK_Y) {
                    axisLock = AxisLock::AXIS_LOCK_NONE;
                } else {
                    axisLock = AxisLock::AXIS_LOCK_Y;
                    axisLockPosition = brushHit.has_value() ? brushHit->block : glm::ivec3(0, 0, 0);
                }

                update_axis_lock_text();
                break;

            case SDL_SCANCODE_Z:
                if(axisLock == AxisLock::AXIS_LOCK_Z) {
                    axisLock = AxisLock::AXIS_LOCK_NONE;
                } else {
                    axisLock = AxisLock::AXIS_LOCK_Z;
                    axisLockPosition = brushHit.has_value() ? brushHit->block : glm::ivec3(0, 0, 0);
                }

                update_axis_lock_text();
                break;
        }

        isKeyHeld[key] = true;
    });

    window->setKeyUpCallback([&](int key) {
        isKeyHeld[key] = false;
    });

    window->setMouseScrollCallback([&](double xoffset, double yoffset) {
        worldCamera->orthoSettings.zoom += static_cast<float>(yoffset) * 0.1f;
    });

    window->setMouseButtonDownCallback([&](int button) {
        if(button == SDL_BUTTON_LEFT && brushHit.has_value()) {
            if(brushHit->block.x < 0 || brushHit->block.y < 0 || brushHit->block.z < 0) {
                return;
            }

            if(currentTool == ToolType::TOOL_MOVE) {
                moveVoxels = world->getConnectedVoxels(brushHit->block);
                moveVoxelsStart = brushHit->block;
            }
        }
        if(button == SDL_BUTTON_RIGHT) {
            //isMovingCamera = true;

            //window.getMousePosition(lastMousePos.x, lastMousePos.y);
            //window.setCursorLocked(true);
        }
    });

    window->setMouseButtonUpCallback([&](int button) {
        if(button == SDL_BUTTON_LEFT && currentTool == ToolType::TOOL_MOVE && brushHit.has_value()) {
            auto pos = brushHit->block + brushHit->side;
            if(pos.x < 0 || pos.y < 0 || pos.z < 0) {
                moveVoxels.clear();
                return;
            }

            glm::ivec3 delta = pos - moveVoxelsStart;
            if(delta.y < 0) {
                delta.y = 0;
            }
            if(delta == glm::ivec3(0, 0, 0)) {
                moveVoxels.clear();
                return;
            }

            std::vector<unsigned char> tempBlockIds(moveVoxels.size());

            for(size_t i = 0; i < moveVoxels.size(); i++) {
                const auto& pos = moveVoxels[i];
                unsigned char blockId = world->getBlock(pos.x, pos.y, pos.z);

                tempBlockIds[i] = blockId;
                world->setBlock(pos.x, pos.y, pos.z, 0);
            }

            for(size_t i = 0; i < moveVoxels.size(); i++) {
                if(tempBlockIds[i] == 0) {
                    continue;
                }

                const auto& voxelPos = moveVoxels[i];
                glm::ivec3 newPos = voxelPos + delta;

                world->setBlock(newPos.x, newPos.y, newPos.z, tempBlockIds[i]);
            }

            moveVoxels.clear();
        }

        if(button == SDL_BUTTON_RIGHT) {
            //isMovingCamera = false;

            //window.setCursorLocked(false);
        }
    });

    window->setResizeCallback([&](int width, int height) {
        std::cout << "Window resized\n";
        std::cout << "New resolution: " << width << "x" << height << std::endl;
        std::cout << "New framebuffer size: " << window->getFramebufferWidth() << "x" << window->getFramebufferHeight() << std::endl;

        renderer->setViewport(0, 0, window->getFramebufferWidth(), window->getFramebufferHeight());
        updateWorldCameraProjection(worldCamera->projectionType);
        uiCamera->setOrthographic(
            0.0f, static_cast<float>(width),
            0.0f, static_cast<float>(height),
            0.0f, 1000.0f
        );

        construct_ui();
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
    
    lastTime = std::chrono::high_resolution_clock::now();
}

void Game::loop() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    frameCounter++;
    
    unsigned currentDrawCalls = renderer->getDrawCallCount();
    unsigned deltaDrawCalls = currentDrawCalls - drawCallCounter;
    drawCallCounter = currentDrawCalls;

    if(std::chrono::duration<double>(currentTime - lastTime).count() >= 1.0) {
        fpsText->setContent("FPS: " + std::to_string(static_cast<int>(frameCounter)));

        lastTime = currentTime;
        frameCounter = 0.0;
    }

    drawCallText->setContent("Draw Calls: " + std::to_string(deltaDrawCalls));
    
    cameraText->setContent(
        "Pos (" +
        std::to_string(static_cast<int>(worldCamera->position.x)) + ", " +
        std::to_string(static_cast<int>(worldCamera->position.y)) + ", " +
        std::to_string(static_cast<int>(worldCamera->position.z)) + ")"
    );
    cameraRotationText->setContent(
        "Rot (" +
        std::to_string(static_cast<int>(worldCamera->rotation.x)) + ", " +
        std::to_string(static_cast<int>(worldCamera->rotation.y)) + ", " +
        std::to_string(static_cast<int>(worldCamera->rotation.z)) + ")"
    );

    worldCamera->orthoSettings.zoom = std::clamp(worldCamera->orthoSettings.zoom, 0.5f, 10.0f);
    glm::vec3 cam_forward = worldCamera->getForward();
    glm::vec3 cam_right = worldCamera->getRight();
    glm::vec3 cam_up = worldCamera->getUp();
    
    if(window->getKeyState(SDL_SCANCODE_W)) {
        if(worldCamera->projectionType == ProjectionType::ORTHOGRAPHIC) {
            worldCamera->position += cam_up * 0.1f;
        } else {
            worldCamera->position += cam_forward * 0.1f;
        }
    }
    
    if(window->getKeyState(SDL_SCANCODE_S)) {
        if(worldCamera->projectionType == ProjectionType::ORTHOGRAPHIC) {
            worldCamera->position -= cam_up * 0.1f;
        } else {
            worldCamera->position -= cam_forward * 0.1f;
        }
    }
    
    if(window->getKeyState(SDL_SCANCODE_A)) {
        worldCamera->position -= cam_right * 0.1f;
    }
    
    if(window->getKeyState(SDL_SCANCODE_D)) {
        worldCamera->position += cam_right * 0.1f;
    }
    
    if(worldCamera->projectionType == ProjectionType::PERSPECTIVE) {
        if(window->getKeyState(SDL_SCANCODE_Q)) {
            worldCamera->position.y += 0.1f;
        }
        
        if(window->getKeyState(SDL_SCANCODE_E)) {
            worldCamera->position.y -= 0.1f;
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

    static glm::vec3 lineStart;
    static glm::vec3 lineEnd;

    if(window->getMouseButtonState(SDL_BUTTON_LEFT)) {
        if(canPressMouse[SDL_BUTTON_LEFT] && brushHit.has_value()) {
            if(currentTool == ToolType::TOOL_PLACE) {
                auto voxels = getVoxelsForTool(*world, brushHit->block + brushHit->side, brushSize, toolShape);
                world->setBlocks(voxels, blockType + 1);
            } else if(currentTool == ToolType::TOOL_BRUSH) {
                auto voxels = getVoxelsForTool(*world, brushHit->block, brushSize, toolShape);
                for(const auto& voxel : voxels) {
                    if(world->getBlock(voxel.x, voxel.y, voxel.z) != 0) {
                        world->setBlock(voxel.x, voxel.y, voxel.z, blockType + 1);
                    }
                }
            } else if(currentTool == ToolType::TOOL_ERASE) {
                auto voxels = getVoxelsForTool(*world, brushHit->block, brushSize, toolShape);
                world->setBlocks(voxels, 0);
            } else if(currentTool == ToolType::TOOL_LINE) {
                int x = brushHit->block.x + brushHit->side.x;
                int y = brushHit->block.y + brushHit->side.y;
                int z = brushHit->block.z + brushHit->side.z;

                if(x >= 0 && y >= 0 && z >= 0) {
                    if(!lineInProgress) {
                        lineStart = glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                        lineInProgress = true;
                    } else {
                        lineEnd = glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

                        world->forVoxelsInLine(
                            glm::ivec3(
                                static_cast<int>(lineStart.x),
                                static_cast<int>(lineStart.y),
                                static_cast<int>(lineStart.z)
                            ),
                            glm::ivec3(
                                static_cast<int>(lineEnd.x),
                                static_cast<int>(lineEnd.y),
                                static_cast<int>(lineEnd.z)
                            ),
                            [&](int px, int py, int pz) {
                                world->setBlock(px, py, pz, blockType + 1);
                            }
                        );

                        lineInProgress = false;
                    }
                }
            }

            canPressMouse[SDL_BUTTON_LEFT] = false;
        }
    } else {
        canPressMouse[SDL_BUTTON_LEFT] = true;
    }

    glm::vec2 currentMouse;
    window->getMousePosition(currentMouse.x, currentMouse.y);

    mouseRay = worldCamera->getRayFromScreenCoords(
        currentMouse.x,
        static_cast<float>(window->getHeight()) - currentMouse.y,
        static_cast<float>(window->getWidth()),
        static_cast<float>(window->getHeight())
    );

    if(window->getMouseButtonState(SDL_BUTTON_RIGHT)) {
        if(!isMovingCamera) {
            window->setCursorLocked(true);
            window->setMousePosition(currentMouse.x, currentMouse.y);
        }

        isMovingCamera = true;
    } else {
        if(isMovingCamera) {
            window->setCursorLocked(false);
            window->setMousePosition(currentMouse.x, currentMouse.y);
        }

        isMovingCamera = false;
    }

    auto rayBlockHit = world->findRayHitBlock(
        mouseRay,
        256.0f
    );

    auto rayPlane0Hit = world->findRayHitYPlane(
        mouseRay,
        256.0f,
        0
    );

    brushHit = std::nullopt;

    if(axisLock != AxisLock::AXIS_LOCK_NONE) {
        float t0 = 0.0f;
        float t1 = 0.0f;

        if(axisLock == AxisLock::AXIS_LOCK_X) {
            t0 = (static_cast<float>(axisLockPosition.y + 0.5f) - mouseRay.origin.y) / mouseRay.direction.y;
            t1 = (static_cast<float>(axisLockPosition.z + 0.5f) - mouseRay.origin.z) / mouseRay.direction.z;
        } else if(axisLock == AxisLock::AXIS_LOCK_Y) {
            t0 = (static_cast<float>(axisLockPosition.x + 0.5f) - mouseRay.origin.x) / mouseRay.direction.x;
            t1 = (static_cast<float>(axisLockPosition.z + 0.5f) - mouseRay.origin.z) / mouseRay.direction.z;
        } else if(axisLock == AxisLock::AXIS_LOCK_Z) {
            t0 = (static_cast<float>(axisLockPosition.x + 0.5f) - mouseRay.origin.x) / mouseRay.direction.x;
            t1 = (static_cast<float>(axisLockPosition.y + 0.5f) - mouseRay.origin.y) / mouseRay.direction.y;
        }

        float t = std::max(t0, t1);
        if(t >= 0.0f && t <= 256.0f) {
            glm::vec3 hitPos = mouseRay.origin + mouseRay.direction * t;
            glm::ivec3 voxelPos;

            if(axisLock == AxisLock::AXIS_LOCK_X) {
                voxelPos = glm::ivec3(
                    static_cast<int>(std::floor(hitPos.x)),
                    axisLockPosition.y,
                    axisLockPosition.z
                );
            } else if(axisLock == AxisLock::AXIS_LOCK_Y) {
                voxelPos = glm::ivec3(
                    axisLockPosition.x,
                    static_cast<int>(std::floor(hitPos.y)),
                    axisLockPosition.z
                );
            } else if(axisLock == AxisLock::AXIS_LOCK_Z) {
                voxelPos = glm::ivec3(
                    axisLockPosition.x,
                    axisLockPosition.y,
                    static_cast<int>(std::floor(hitPos.z))
                );
            }

            brushHit = WorldRayHit{
                voxelPos,
                glm::ivec3(0, 0, 0),
                t
            };
        }
    } else {
        if(rayBlockHit.has_value()) {
            if(currentTool == ToolType::TOOL_MOVE) {
                bool isInMoveVoxels = false;
                for(const auto& voxelPos : moveVoxels) {
                    if(voxelPos == rayBlockHit->block) {
                        isInMoveVoxels = true;
                        break;
                    }
                }

                if(!moveVoxels.empty() && isInMoveVoxels) {
                    brushHit = std::nullopt;
                } else {
                    brushHit = rayBlockHit;
                }
            } else {
                brushHit = rayBlockHit;
            }
        } 
        
        if(!brushHit.has_value()) {
            if(currentTool == ToolType::TOOL_MOVE) {
                auto rayYPlane = world->findRayHitYPlane(
                    mouseRay,
                    256.0f,
                    moveVoxelsStart.y
                );

                if(rayYPlane.has_value()) {
                    brushHit = rayYPlane;
                }
            } else {
                brushHit = rayPlane0Hit;
            }
        }
    }

    if(brushHit.has_value()) {
        int x = static_cast<int>(brushHit->block.x);
        int y = static_cast<int>(brushHit->block.y);
        int z = static_cast<int>(brushHit->block.z);

        rayHitText->setContent(
            "Ray Hit (" +
            std::to_string(x) + ", " +
            std::to_string(y) + ", " +
            std::to_string(z) + ") " +
            "Side (" +
            std::to_string(brushHit->side.x) + ", " +
            std::to_string(brushHit->side.y) + ", " +
            std::to_string(brushHit->side.z) + ")"
        );
    } else {
        rayHitText->setContent("Ray Hit (-, -, -)");
    }

    if(worldCamera->projectionType == ProjectionType::ORTHOGRAPHIC) {
        const float desiredDistance = 128.0f;
        float distance = 0.0f;

        auto cameraRay = worldCamera->getRayFromScreenCoords(
            window->getWidth() / 2.0f,
            window->getHeight() / 2.0f,
            static_cast<float>(window->getWidth()),
            static_cast<float>(window->getHeight())
        );

        auto centerHit = world->findRayHitBlock(
            cameraRay,
            256.0f
        );

        if(!centerHit.has_value()) {
            centerHit = world->findRayHitYPlane(
                cameraRay,
                256.0f,
                0
            );
        }

        if(centerHit.has_value()) {
            distance = centerHit->distance;
        } else {
            distance = worldCamera->position.y / cam_forward.y;
        }

        worldCamera->position -= cam_forward * (desiredDistance - distance);
    }

    if(isMovingCamera) {
        float xoffset = static_cast<float>(currentMouse.x - lastMousePos.x);
        float yoffset = static_cast<float>(currentMouse.y - lastMousePos.y);

        if(worldCamera->projectionType == ProjectionType::PERSPECTIVE) {
            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            worldCamera->rotation.y += xoffset;
            worldCamera->rotation.x -= yoffset;
        } else {
            float panSpeed = 0.05f * worldCamera->orthoSettings.zoom;

            glm::vec3 right = worldCamera->getRight();
            glm::vec3 up = worldCamera->getUp();

            worldCamera->position -= right * xoffset * panSpeed;
            worldCamera->position += up * yoffset * panSpeed;
        }
    }

    toolWorld->removeAllChunks();
    toolWorldMesh->clearChunkMeshes();
    if(brushHit.has_value()) {
        if(currentTool == ToolType::TOOL_PLACE) {
            auto voxels = getVoxelsForTool(*world, brushHit->block + brushHit->side, brushSize, toolShape);
            toolWorld->setBlocks(voxels, blockType + 1);
        } else if(currentTool == ToolType::TOOL_ERASE) {
                auto voxels = getVoxelsForTool(*world, brushHit->block, brushSize, toolShape);
                toolWorld->setBlocks(voxels, 1);
        } else if(currentTool == ToolType::TOOL_BRUSH) {
                auto voxels = getVoxelsForTool(*world, brushHit->block, brushSize, toolShape);
                
                for(const auto& voxel : voxels) {
                    if(world->getBlock(voxel.x, voxel.y, voxel.z) != 0) {
                        toolWorld->setBlock(voxel.x, voxel.y, voxel.z, blockType + 1);
                    }
                }
        } else if(currentTool == ToolType::TOOL_LINE) {
                int x = brushHit->block.x + brushHit->side.x;
                int y = brushHit->block.y + brushHit->side.y;
                int z = brushHit->block.z + brushHit->side.z;

                if(x >= 0 && y >= 0 && z >= 0) {
                    if(lineInProgress) {
                        world->forVoxelsInLine(
                            glm::ivec3(
                                static_cast<int>(lineStart.x),
                                static_cast<int>(lineStart.y),
                                static_cast<int>(lineStart.z)
                            ),
                            glm::ivec3(x, y, z),
                            [&](int px, int py, int pz) {
                                if(px >= 0 && py >= 0 && pz >= 0) {
                                    toolWorld->setBlock(px, py, pz, blockType + 1);
                                }
                            }
                        );
                    } else {
                        toolWorld->setBlock(x, y, z, blockType + 1);
                    }
                }
        } else if(currentTool == ToolType::TOOL_MOVE) {
            if(!moveVoxels.empty()) {
                auto pos = brushHit->block + brushHit->side;

                if(pos.x < 0 || pos.y < 0 || pos.z < 0) {
                    return;
                }

                glm::ivec3 delta = pos - moveVoxelsStart;
                if(delta.y < 0) {
                    delta.y = 0;
                }

                for(const auto& voxelPos : moveVoxels) {
                    toolWorld->setBlock(
                        voxelPos.x + delta.x,
                        voxelPos.y + delta.y,
                        voxelPos.z + delta.z,
                        world->getBlock(voxelPos.x, voxelPos.y, voxelPos.z)
                    );
                }
            }   
        }
    }

    lastMousePos = currentMouse;

    brushSizeText->setContent(
        "Brush Size: " + std::to_string(brushSize + 1)
    );

    toolShapeText->setContent(
        std::string("Tool Shape: ") + (toolShape == ToolShape::SHAPE_CUBE ? "Cube" : "Sphere")
    );

    worldMesh->update();
    toolWorldMesh->update();

    renderer->setViewport(0, 0, window->getFramebufferWidth(), window->getFramebufferHeight());

    if(isWireframe) {
        renderer->clear(0.6, 0.6, 0.6, 1.0);
    } else {
        if(darkMode) {
            renderer->clear(0.0, 0.0, 0.0, 1.0);
        } else {
            renderer->clear(0.6, 0.6, 0.6, 1.0);
        }
    }

    renderer->setDepthTest(true);
    renderer->setCullFace(true);

    renderer->useShader(gridShader);
    gridShader.setUniformMat4("u_Model", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(worldCamera->position.x, 0.0f, worldCamera->position.z))));
    gridShader.setUniformMat4("u_View", glm::value_ptr(worldCamera->getViewMatrix()));
    gridShader.setUniformMat4("u_Projection", glm::value_ptr(worldCamera->getProjectionMatrix()));
    auto gridColor = !darkMode ? glm::vec4(0.2f, 0.2f, 0.2f, 0.3f) : glm::vec4(0.8f, 0.8f, 0.8f, 0.3f);
    gridShader.setUniformVec4("u_BorderColor", gridColor.r, gridColor.g, gridColor.b, gridColor.a);

    renderer->setCullFace(false);
    renderer->drawEmpty(6);
    renderer->setCullFace(true);

    auto lightDir = glm::normalize(cam_forward * -1.0f);

    renderer->useShader(voxelShader);
    renderer->bindTexture(0, *paletteTexture);
    voxelShader.setUniformMat4("u_View", glm::value_ptr(worldCamera->getViewMatrix()));
    voxelShader.setUniformMat4("u_Projection", glm::value_ptr(worldCamera->getProjectionMatrix()));
    voxelShader.setUniformInt("u_UseTexture", 1);
    voxelShader.setUniformInt("u_UseLight", 1);
    voxelShader.setUniformVec3("u_LightDir", -1.0f, 0.5f, 0.2f);
    voxelShader.setUniformVec4("u_ColorTint", 1.0f, 1.0f, 1.0f, 1.0f);

    for(const auto& [id, chunkMesh] : worldMesh->getChunkMeshes()) {
        auto chunkPtr = chunkMesh->getChunk();
        if(chunkPtr->getBlockCount() == 0) {
            continue;
        }

        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                chunkPtr->getX() * CHUNK_SIZE,
                chunkPtr->getY() * CHUNK_SIZE,
                chunkPtr->getZ() * CHUNK_SIZE
            )
        );

        voxelShader.setUniformMat4("u_Model", glm::value_ptr(model));
        renderer->drawVAO(chunkMesh->getVertexArray());
    }

    voxelShader.setUniformInt("u_UseLight", 0);
    if(currentTool == ToolType::TOOL_ERASE) {
        voxelShader.setUniformInt("u_UseTexture", 0);
        voxelShader.setUniformVec4("u_ColorTint", 0.6f, 0.2f, 0.2f, 0.5f);
    } else {
        voxelShader.setUniformInt("u_UseTexture", 1);
        voxelShader.setUniformVec4("u_ColorTint", 1.0f, 1.0f, 1.0f, 0.7f);
    }

    renderer->enablePolygonOffsetFill(-1.0f, -1.0f);
    for(const auto& [id, chunkMesh] : toolWorldMesh->getChunkMeshes()) {
        auto chunkPtr = chunkMesh->getChunk();
        if(chunkPtr->getBlockCount() == 0) {
            continue;
        }
        
        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                chunkPtr->getX() * CHUNK_SIZE,
                chunkPtr->getY() * CHUNK_SIZE,
                chunkPtr->getZ() * CHUNK_SIZE
            )
        );

        voxelShader.setUniformMat4("u_Model", glm::value_ptr(model));
        renderer->drawVAO(chunkMesh->getVertexArray());
    }
    renderer->disablePolygonOffsetFill();

    if(showBoundingBox) {
        renderer->useShader(outlineShader);
        
        outlineShader.setUniformMat4("u_View", glm::value_ptr(worldCamera->getViewMatrix()));
        outlineShader.setUniformMat4("u_Projection", glm::value_ptr(worldCamera->getProjectionMatrix()));
        outlineShader.setUniformMat4("u_Model", glm::value_ptr(
            glm::translate(glm::mat4(1.0f), glm::vec3(CHUNK_SIZE / 2, CHUNK_SIZE / 2, CHUNK_SIZE / 2)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE))
        ));

        outlineShader.setUniformVec4("u_BorderColor", 0.8f, 0.8f, 0.8f, 1.0f);

        renderer->enablePolygonOffsetFill(-1.0f, -1.0f);
        renderer->setCullFace(false);
        renderer->drawVAO(*outlineVAO, gfx::RenderMode::TRIANGLES);
        renderer->setCullFace(true);
        renderer->disablePolygonOffsetFill();
    }

    renderer->setDepthTest(false);

    auto transform2D = Transform2D(
        glm::vec2(
            window->getWidth() / 2.0f,
            window->getHeight() / 2.0f
        ),
        0.0f,
        glm::vec2(600.0f, 512.0f)
    );

    /*uiRenderer.renderQuad(
        basic2DShader,
        *crosshairTexture,
        transform2D
    );*/
    //uiRenderer->renderSprite(basic2DShader, *uiBackgroundSprite);
    uiRenderer->renderSprite(basic2DShader, *sprite);

    textRenderer->renderText(textShader, *fpsText);
    textRenderer->renderText(textShader, *cameraText);
    textRenderer->renderText(textShader, *cameraRotationText);
    textRenderer->renderText(textShader, *rayHitText);
    textRenderer->renderText(textShader, *brushSizeText);
    textRenderer->renderText(textShader, *currentPaletteText);
    textRenderer->renderText(textShader, *drawCallText);
    textRenderer->renderText(textShader, *currentToolText);
    textRenderer->renderText(textShader, *axisLockText);
    textRenderer->renderText(textShader, *toolShapeText);
}

void Game::construct_ui() {
    uiBackgroundSprite->anchor = glm::vec2(0.0f, 0.0f);
    uiBackgroundSprite->size = glm::vec2(
        static_cast<float>(220.0f),
        static_cast<float>(window->getHeight())
    );
    uiBackgroundSprite->position = glm::vec3(0.0f, 0.0f, 0.0f);
    uiBackgroundSprite->setColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    cameraText->position = glm::vec3(10.0f, window->getHeight() - 10.0f, 0.0f);
    cameraRotationText->position = glm::vec3(10.0f, window->getHeight() - 30.0f, 0.0f);
    rayHitText->position = glm::vec3(10.0f, window->getHeight() - 50.0f, 0.0f);

    fpsText->position = glm::vec3(window->getWidth() - 10.0f, window->getHeight() - 10.0f, 0.0f);
    drawCallText->position = glm::vec3(window->getWidth() - 10.0f, window->getHeight() - 30.0f, 0.0f);

    brushSizeText->position = glm::vec3(10.0f, 10.0f, 0.0f);
    currentPaletteText->position = glm::vec3(10.0f, 30.0f, 0.0f);
    toolShapeText->position = glm::vec3(10.0f, 50.0f, 0.0f);
    currentToolText->position = glm::vec3(10.0f, 70.0f, 0.0f);
    axisLockText->position = glm::vec3(10.0f, 90.0f, 0.0f);

    sprite->position = glm::vec3(
        currentPaletteText->position.x + 146,
        currentPaletteText->position.y + 5.0f,
        0.0f
    );
};

void Game::updateWorldCameraProjection(ProjectionType type) {
    float ratio = static_cast<float>(window->getFramebufferWidth()) / static_cast<float>(window->getFramebufferHeight());

    float orthoHeight = CHUNK_SIZE * std::sqrt(3.0f);
    float orthoHalfHeight = orthoHeight / 2.0f;

    if(type == ProjectionType::ORTHOGRAPHIC) {
        worldCamera->setOrthographic(
            -orthoHalfHeight * ratio, orthoHalfHeight * ratio,
            -orthoHalfHeight, orthoHalfHeight,
            0.1f, 1000.0f
        );
    } else {
        worldCamera->setPerspective(
            45.0f,
            ratio,
            0.1f, 1000.0f
        );
    }

    worldCamera->position = glm::vec3(-7.0f, 21.0f, 24.0f);
    worldCamera->rotation = glm::vec3(-30.0f, 45.0f, 0.0f);
};

void Game::update_current_tool_text() {
    std::string toolName;
    switch(currentTool) {
        case ToolType::TOOL_PLACE:
            toolName = "Place";
            break;
        case ToolType::TOOL_BRUSH:
            toolName = "Brush";
            break;
        case ToolType::TOOL_ERASE:
            toolName = "Erase";
            break;
        case ToolType::TOOL_LINE:
            toolName = "Line";
            break;
        case ToolType::TOOL_MOVE:
            toolName = "Move";
            break;
    }

    currentToolText->setContent("Tool: " + toolName);
};

void Game::update_axis_lock_text() {
    std::string lockName;
    switch(axisLock) {
        case AxisLock::AXIS_LOCK_NONE:
            lockName = "None";
            break;
        case AxisLock::AXIS_LOCK_X:
            lockName = "X";
            break;
        case AxisLock::AXIS_LOCK_Y:
            lockName = "Y";
            break;
        case AxisLock::AXIS_LOCK_Z:
            lockName = "Z";
            break;
    }

    axisLockText->setContent("Axis Lock: " + lockName);
};

void Game::set_current_palette_sprite_uvs(int index) {
    const int colorsPerRow = 256;
    const float uvSize = 1.0f / colorsPerRow;

    float u = (index % colorsPerRow) / (float)colorsPerRow;
    float v = (index / colorsPerRow) / (float)colorsPerRow;

    sprite->setUVs(glm::vec4(
        u, v,
        u + uvSize, v + uvSize
    ));
};

void Game::regenerate_palette() {
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

void Game::shutdown() {
    auto worldAsset = World::saveToAsset(*world);
    worldAsset.saveToFile("arrow.dat");
}