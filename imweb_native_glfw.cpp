#include "imweb_native_glfw.hpp"

#ifndef __EMSCRIPTEN__

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "imweb_glfw_helper.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

struct ImWeb::Impl {
  Impl() {}
  ~Impl() {}

  GLFWwindow *m_window = NULL;
  static void glfw_error_callback(int error, const char *description);
  static void glfw_close_callback(GLFWwindow *window);
};

ImWeb::ImWeb() : ImWebBase(), impl{std::make_unique<ImWeb::Impl>()} {}

ImWeb::~ImWeb() {}

void ImWeb::Impl::glfw_error_callback(int error, const char *description) {
  std::string err_str =
      "GLFW Error #" + std::to_string(error) + ": " + description;
  throw ImWebException(ET::GRAPHICS_BACKEND, err_str);
}

void ImWeb::Impl::glfw_close_callback(GLFWwindow *window) {
  // Nothing to do here for now.
}

void ImWeb::initGL(int width, int height) {
  if (glfwSetErrorCallback(ImWeb::Impl::glfw_error_callback) != NULL)
    throw ImWebException(ET::GRAPHICS_BACKEND,
                         "GLFW Error Callback was set previously");

  if (!glfwInit())
    throw ImWebException(ET::GRAPHICS_BACKEND, "GLFW Initialization failed");

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  impl->m_window = glfwCreateWindow(width, height, "ImWeb", NULL, NULL);
  if (impl->m_window == NULL) {
    glfwTerminate();
    throw ImWebException(ET::GRAPHICS_BACKEND, "GLFW Create Window failed");
  }

  glfwSetWindowUserPointer(impl->m_window, reinterpret_cast<void *>(this));
  glfwSetWindowCloseCallback(impl->m_window, ImWeb::Impl::glfw_close_callback);
  glfwMakeContextCurrent(impl->m_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw ImWebException(ET::GRAPHICS_BACKEND, "Failed to initialize GLAD");
}

void ImWeb::initImGui() {
  createImGuiContext();

  ImGui_ImplGlfw_InitForOpenGL(impl->m_window, true);
  ImGui_ImplOpenGL3_Init();

  setupImGui();
}

void ImWeb::loop(std::optional<ImWebCallback> cb) {
  m_running = true;
  m_init = true;

  while (isRunning()) {
    glfwSwapBuffers(impl->m_window);
    glfwPollEvents();
    glfwMakeContextCurrent(impl->m_window);

    {
      int width = 0, height = 0;
      glfwGetFramebufferSize(impl->m_window, &width, &height);
      setCurrentWidth(width);
      setCurrentHeight(height);

      glViewport(0, 0, width, height);
    }

    GlfwHelper::clearColor(getClearColor());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    /* ----------------------------------------- */
    if (getDrawableCount() > 0 && !draw())
      m_running = false;

    if (cb.has_value() && !cb.value()())
      m_running = false;

    if (getDrawableCount() == 0 && !cb.has_value())
      defaultUi();
    /* ----------------------------------------- */

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwMakeContextCurrent(impl->m_window);
  }

  stop();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  GlfwHelper::destroy(&impl->m_window);
  m_init = false;
}

void ImWeb::stop() {
  m_running = false;
  glfwSetWindowShouldClose(impl->m_window, GLFW_TRUE);
}

bool ImWeb::isRunning() const {
  return m_running && !glfwWindowShouldClose(impl->m_window);
}

bool ImWeb::isInitialized() const { return m_init; }

#endif
