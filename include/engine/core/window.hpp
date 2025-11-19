#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>
#include <functional>

namespace core {
    using WindowKeyboardEvent = std::function<void(int key)>;
    using WindowMouseScrollEvent = std::function<void(double xoffset, double yoffset)>;
    using WindowMouseButtonEvent = std::function<void(int button)>;
    using WindowMouseMoveEvent = std::function<void(double xpos, double ypos)>;

    class Window {
    public:
        Window(int width = 800, int height = 600, const char* title = "Window");
        ~Window();

        bool shouldClose() const;
        void swapBuffers();
        void pollEvents();
        void makeContextCurrent();
        void setSwapInterval(int interval);

        void setCursorLocked(bool locked);

        int getKeyState(int key) const;
        void getMousePosition(double& xpos, double& ypos) const;
        int getMouseButtonState(int button) const;

        void setMousePosition(double xpos, double ypos);

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

    private:
        struct Impl;
        Impl* _impl;

        WindowKeyboardEvent _keyDownCallback;
        WindowKeyboardEvent _keyUpCallback;

        WindowMouseButtonEvent _mouseButtonDownCallback;
        WindowMouseButtonEvent _mouseButtonUpCallback;
        WindowMouseMoveEvent _mouseMoveCallback;
        WindowMouseScrollEvent _mouseScrollCallback;
    };
}