#include "engine/core/window.hpp"

#include <iostream>
#include <stdexcept>

#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
#include <glad/gles2.h>
#else
#include <glad/gl.h>
#endif

using namespace core;

struct Window::Impl {
    SDL_Window* window;
    SDL_GLContext context;
};

Window::Window(int width, int height, const char* title) {
    _impl = new Impl();


    #ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2); 
    #else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    #endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _impl->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);
    if (!_impl->window) {
        throw std::runtime_error("Failed to create SDL window");
    }

    _impl->context = SDL_GL_CreateContext(_impl->window);
    if (!_impl->context) {
        throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
    }

#ifdef __EMSCRIPTEN__
    int version = gladLoadGLES2(SDL_GL_GetProcAddress);
#else
    int version = gladLoadGL(SDL_GL_GetProcAddress);
#endif
    if (version == 0) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    std::cout << "OpenGL Version: " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
}

Window::~Window() {
    if(_impl && _impl->window) {
        SDL_DestroyWindow(_impl->window);
    }

    delete _impl;
}

int Window::getKeyState(int code) const {
    const bool* state = SDL_GetKeyboardState(NULL);
    return state[code];
}

bool Window::shouldClose() const {
    return _shouldClose;
}

void Window::setSwapInterval(int interval) {
    SDL_GL_SetSwapInterval(interval);
}

void Window::setCursorLocked(bool locked) {
    if(locked) {
        SDL_SetWindowRelativeMouseMode(_impl->window, true);
    } else {
        SDL_SetWindowRelativeMouseMode(_impl->window, false);
    }
}

void Window::setKeyDownCallback(WindowKeyboardEvent callback) {
    _keyDownCallback = callback;
}

void Window::setKeyUpCallback(WindowKeyboardEvent callback) {
    _keyUpCallback = callback;
}

void Window::setMouseButtonDownCallback(WindowMouseButtonEvent callback) {
    _mouseButtonDownCallback = callback;
}

void Window::setMouseButtonUpCallback(WindowMouseButtonEvent callback) {
    _mouseButtonUpCallback = callback;
}

void Window::setMouseMoveCallback(WindowMouseMoveEvent callback) {
    _mouseMoveCallback = callback;
}

void Window::setMouseScrollCallback(WindowMouseScrollEvent callback) {
    _mouseScrollCallback = callback;
}

void Window::setResizeCallback(WindowResizeEvent callback) {
    _resizeCallback = callback;
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(_impl->window);
}

void Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        processEvent(event);
    }
}

void Window::processEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_QUIT:
            _shouldClose = true;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (_keyDownCallback) {
                _keyDownCallback(event.key.scancode);
            }
            break;
        case SDL_EVENT_KEY_UP:
            if (_keyUpCallback) {
                _keyUpCallback(event.key.scancode);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (_mouseButtonDownCallback) {
                _mouseButtonDownCallback(event.button.button);   
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (_mouseButtonUpCallback) {
                _mouseButtonUpCallback(event.button.button);
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (_mouseMoveCallback) {
                _mouseMoveCallback(event.motion.x, event.motion.y);
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            if (_mouseScrollCallback) {
                _mouseScrollCallback(event.wheel.x, event.wheel.y);
            }
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            if (_resizeCallback) {
                _resizeCallback(event.window.data1, event.window.data2);
            }
            break;
        default:
            break;
    }
}

void Window::makeContextCurrent() {
    SDL_GL_MakeCurrent(_impl->window, _impl->context);
}

void Window::getMousePosition(float& xpos, float& ypos) const {
    SDL_GetMouseState(&xpos, &ypos);
}

void Window::setMousePosition(float xpos, float ypos) {
    SDL_WarpMouseInWindow(_impl->window, static_cast<int>(xpos), static_cast<int>(ypos));
}

int Window::getMouseButtonState(int button) const {
    return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MASK(button);
}

int Window::getWidth() const {
    int width;
    SDL_GetWindowSize(_impl->window, &width, NULL);

    return width;
}

int Window::getHeight() const {
    int height;
    SDL_GetWindowSize(_impl->window, NULL, &height);

    return height;
}

int Window::getFramebufferWidth() const {
    int width;
    SDL_GetWindowSizeInPixels(_impl->window, &width, NULL);
    
    return width;
}

int Window::getFramebufferHeight() const {
    int height;
    SDL_GetWindowSizeInPixels(_impl->window, NULL, &height);

    return height;
}