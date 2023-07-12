#ifndef IMWEB_GLFW_HELPER_H
#define IMWEB_GLFW_HELPER_H 1

#include <unistd.h>

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

  static void limitFps(double targetFps, double *frame_start,
                       double *frame_end) {
    double wait_time = 1.0 / targetFps;
    double curr_frame_time = glfwGetTime() - *frame_start;
    double dur = 1000.0 * 1000.0 * (wait_time - curr_frame_time) + 0.5;
    int durd = static_cast<unsigned int>(dur);
    if (durd > 0) {
      usleep(durd);
    }
    *frame_end = glfwGetTime();
    *frame_start = *frame_end;
  }
};

#endif
