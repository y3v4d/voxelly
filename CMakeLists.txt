cmake_minimum_required(VERSION 3.17)
project(Voxelly VERSION 0.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

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

FetchContent_Declare(
    sdl
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.26
)

FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(freetype)
FetchContent_MakeAvailable(sdl)

# on Web targets, we need CMake to generate a HTML webpage. 
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
  # Package /assets/ directory into the final build output
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")
endif()

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
    src/game.cpp
)

#if(APPLE)
#set_target_properties(
#    Voxelly PROPERTIES 
#    MACOSX_BUNDLE TRUE
#    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist
#)
#endif()

if(EMSCRIPTEN)
    set(LINKER_FLAGS
        "-sNO_DISABLE_EXCEPTION_CATCHING"
        "-sMAX_WEBGL_VERSION=2"
        "-sSTACK_SIZE=1MB"
        "-sALLOW_MEMORY_GROWTH"
        "--emrun"
    )

    target_link_options(Voxelly PRIVATE ${LINKER_FLAGS})

    set_target_properties(Voxelly PROPERTIES
        SUFFIX ".js"
    )
    
    # copy build-templates/web/index.html to the build directory as index.html
    add_custom_command(
        TARGET Voxelly POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/build-templates/web/*
                ${CMAKE_BINARY_DIR}/
    )
endif()

target_link_libraries(Voxelly PRIVATE glad stb glm freetype SDL3::SDL3)

target_include_directories(Voxelly PRIVATE include)

# Copy assets to the app bundle on macOS
if(NOT EMSCRIPTEN)
    add_custom_command(
        TARGET Voxelly POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets
               # $<TARGET_BUNDLE_CONTENT_DIR:Voxelly>/Resources/assets
                ${CMAKE_BINARY_DIR}/assets
    )
endif()