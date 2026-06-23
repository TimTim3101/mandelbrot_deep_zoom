#ifndef WINDOW_H
#define WINDOW_H

#include "../third-party/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

class Window {
  private:
    GLFWmonitor *monitor = nullptr;
    GLFWwindow *window = nullptr;
    int width, height;

  public:
    const GLFWvidmode *mode = nullptr;
    Window(const char *name);
    ~Window();

    GLFWwindow *get() const;
    int get_screen_width() const;
    int get_screen_height() const;
};

#endif
