#include "window.h"
#include <iostream>

Window::Window(const char *name) {
    if (!glfwInit())
        throw std::runtime_error("Failed to init GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        glfwTerminate();
        throw std::runtime_error("Failed to monitor");
    }

    mode = glfwGetVideoMode(monitor);
    if (!mode) {
        glfwTerminate();
        throw std::runtime_error("Failed to mode");
    }

    window = glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    std::cout << mode->width << "x" << mode->height << std::endl;
    glfwMakeContextCurrent(window);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide cursor

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to init GLAD");
    }
}

GLFWwindow *Window::get() const {
    return window;
}

int Window::get_screen_width() const {
    return mode->width;
}

int Window::get_screen_height() const {
    return mode->height;
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
