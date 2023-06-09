#ifndef IMWEB_GLFW_HELPER_H
#define IMWEB_GLFW_HELPER_H 1

#include <GLFW/glfw3.h>

#include <array>

class GlfwHelper {
public:
  GlfwHelper() = delete;
  GlfwHelper(const GlfwHelper &) = delete;

  static void init() {
    if (!glfwInit())
      throw ImWebException(ET::GRAPHICS_BACKEND, "GLFW Initialization failed");
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }

  static GLFWwindow *createWindow(int width, int height) {
    GLFWwindow *window = glfwCreateWindow(width, height, "ImWeb", NULL, NULL);
    if (window == NULL) {
      glfwTerminate();
      throw ImWebException(ET::GRAPHICS_BACKEND, "GLFW Create Window failed");
    }
    glfwMakeContextCurrent(window);

    return window;
  }

  static void destroy(GLFWwindow **const window) {
    if (window != NULL && *window != NULL) {
      glfwDestroyWindow(*window);
      *window = NULL;
    }
    glfwTerminate();
  }

  static void clearColor(std::array<float, 4> const &color) {
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
  }
};

#endif
