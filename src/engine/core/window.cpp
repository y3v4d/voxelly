#include "engine/core/window.hpp"

#include <iostream>
#include <stdexcept>

using namespace core;

struct Window::Impl {
    GLFWwindow* window;
};

static void error_callback(int error, const char* description) {
    std::cout << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(!win) {
        return;
    }

    if(action == GLFW_PRESS && win->getKeyDownCallback()) {
        win->getKeyDownCallback()(key);
    }

    if(action == GLFW_RELEASE && win->getKeyUpCallback()) {
        win->getKeyUpCallback()(key);
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(!win) {
        return;
    }

    if(win->getMouseScrollCallback()) {
        win->getMouseScrollCallback()(xoffset, yoffset);
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(!win) {
        return;
    }

    if(action == GLFW_PRESS && win->getMouseButtonDownCallback()) {
        win->getMouseButtonDownCallback()(button);
    }

    if(action == GLFW_RELEASE && win->getMouseButtonUpCallback()) {
        win->getMouseButtonUpCallback()(button);
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(!win) {
        return;
    }

    if(win->getMouseMoveCallback()) {
        win->getMouseMoveCallback()(xpos, ypos);
    }
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

    glfwSetWindowUserPointer(_impl->window, this);
    glfwSetKeyCallback(_impl->window, key_callback);
    glfwSetScrollCallback(_impl->window, scroll_callback);
    glfwSetCursorPosCallback(_impl->window, cursor_position_callback);
    glfwSetMouseButtonCallback(_impl->window, mouse_button_callback);

    // raw input
    glfwSetInputMode(_impl->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
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

void Window::setCursorLocked(bool locked) {
    if(locked) {
        glfwSetInputMode(_impl->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(_impl->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

void Window::setMousePosition(double xpos, double ypos) {
    glfwSetCursorPos(_impl->window, xpos, ypos);
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