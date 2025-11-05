#include "engine/core/window.hpp"

#include <iostream>
#include <stdexcept>

using namespace core;

struct Window::Impl {
    GLFWwindow* window;
};

static void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

Window::Window(int width, int height, const char* title) {
    if(!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    _impl = new Impl();

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

    _impl->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!_impl->window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
}

Window::~Window() {
    if(_impl && _impl->window) {
        glfwDestroyWindow(_impl->window);
    }
    glfwTerminate();
    delete _impl;
}

int Window::getKeyState(int key) const {
    return glfwGetKey(_impl->window, key);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(_impl->window);
}

void Window::setSwapInterval(int interval) {
    glfwSwapInterval(interval);
}

void Window::swapBuffers() {
    glfwSwapBuffers(_impl->window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::makeContextCurrent() {
    glfwMakeContextCurrent(_impl->window);
}

void Window::getMousePosition(double& xpos, double& ypos) const {
    glfwGetCursorPos(_impl->window, &xpos, &ypos);
}

int Window::getMouseButtonState(int button) const {
    return glfwGetMouseButton(_impl->window, button);
}

int Window::getWidth() const {
    int width;
    glfwGetWindowSize(_impl->window, &width, NULL);

    return width;
}

int Window::getHeight() const {
    int height;
    glfwGetWindowSize(_impl->window, NULL, &height);

    return height;
}

int Window::getFramebufferWidth() const {
    int width;
    glfwGetFramebufferSize(_impl->window, &width, NULL);

    return width;
}

int Window::getFramebufferHeight() const {
    int height;
    glfwGetFramebufferSize(_impl->window, NULL, &height);

    return height;
}