cmake_minimum_required(VERSION 3.17)
project(Voxelly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.2
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-14-1
)

# Disable examples/tests/docs
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(freetype)

add_subdirectory(libs/glad)
add_subdirectory(libs/stb)

add_executable(Voxelly 
    src/engine/assets/file.cpp src/engine/assets/font.cpp src/engine/assets/image.cpp src/engine/assets/shader.cpp src/engine/assets/mesh.cpp
    src/engine/components/camera.cpp src/engine/components/sprite.cpp src/engine/components/text.cpp
    src/engine/core/filesystem.cpp src/engine/core/window.cpp
    src/engine/font/font.cpp
    src/engine/gfx/buffer/index_buffer.cpp src/engine/gfx/buffer/vertex_buffer.cpp src/engine/gfx/buffer/vertex_array.cpp
    src/engine/gfx/renderer.cpp src/engine/gfx/renderer_gl.cpp src/engine/gfx/shader.cpp src/engine/gfx/texture.cpp
    src/engine/systems/ui_renderer.cpp
    src/engine/engine.cpp
    src/chunk.cpp
    src/chunk_mesh.cpp
    src/main.cpp
    src/text_renderer.cpp
    src/world_asset.cpp
    src/world.cpp
)

target_link_libraries(Voxelly PRIVATE glfw glad stb glm freetype)

target_include_directories(Voxelly PRIVATE include)

add_custom_command(
    TARGET Voxelly POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:Voxelly>/assets
)