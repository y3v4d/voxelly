#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <memory>
#include <functional>

namespace core {
    using WindowKeyboardEvent = std::function<void(int key)>;
    using WindowMouseScrollEvent = std::function<void(double xoffset, double yoffset)>;
    using WindowMouseButtonEvent = std::function<void(int button)>;
    using WindowMouseMoveEvent = std::function<void(double xpos, double ypos)>;
    using WindowResizeEvent = std::function<void(int width, int height)>;

    class Window {
    public:
        Window(int width = 800, int height = 600, const char* title = "Window");
        ~Window();

        bool shouldClose() const;
        void swapBuffers();
        void processEvent(const SDL_Event& event);
        void pollEvents();
        void makeContextCurrent();
        void setSwapInterval(int interval);

        void setCursorLocked(bool locked);

        int getKeyState(int key) const;
        void getMousePosition(float& xpos, float& ypos) const;
        int getMouseButtonState(int button) const;

        void setMousePosition(float xpos, float ypos);

        int getWidth() const;
        int getHeight() const;

        int getFramebufferWidth() const;
        int getFramebufferHeight() const;

        WindowKeyboardEvent getKeyDownCallback() const { return _keyDownCallback; }
        void setKeyDownCallback(WindowKeyboardEvent callback);

        WindowKeyboardEvent getKeyUpCallback() const { return _keyUpCallback; }
        void setKeyUpCallback(WindowKeyboardEvent callback);

        WindowMouseButtonEvent getMouseButtonDownCallback() const { return _mouseButtonDownCallback; }
        void setMouseButtonDownCallback(WindowMouseButtonEvent callback);

        WindowMouseButtonEvent getMouseButtonUpCallback() const { return _mouseButtonUpCallback; }
        void setMouseButtonUpCallback(WindowMouseButtonEvent callback);

        WindowMouseMoveEvent getMouseMoveCallback() const { return _mouseMoveCallback; }
        void setMouseMoveCallback(WindowMouseMoveEvent callback);

        WindowMouseScrollEvent getMouseScrollCallback() const { return _mouseScrollCallback; }
        void setMouseScrollCallback(WindowMouseScrollEvent callback);

        WindowResizeEvent getResizeCallback() const { return _resizeCallback; }
        void setResizeCallback(WindowResizeEvent callback);

    private:
        struct Impl;
        Impl* _impl;

        bool _shouldClose = false;

        WindowKeyboardEvent _keyDownCallback;
        WindowKeyboardEvent _keyUpCallback;

        WindowMouseButtonEvent _mouseButtonDownCallback;
        WindowMouseButtonEvent _mouseButtonUpCallback;
        WindowMouseMoveEvent _mouseMoveCallback;
        WindowMouseScrollEvent _mouseScrollCallback;
        WindowResizeEvent _resizeCallback;
    };
}