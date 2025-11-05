#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>

namespace core {
    class Window {
    public:
        Window(int width = 800, int height = 600, const char* title = "Window");
        ~Window();

        bool shouldClose() const;
        void swapBuffers();
        void pollEvents();
        void makeContextCurrent();
        void setSwapInterval(int interval);

        int getKeyState(int key) const;
        void getMousePosition(double& xpos, double& ypos) const;
        int getMouseButtonState(int button) const;

        int getWidth() const;
        int getHeight() const;

        int getFramebufferWidth() const;
        int getFramebufferHeight() const;

    private:
        struct Impl;
        Impl* _impl;
    };
}