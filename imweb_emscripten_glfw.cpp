#include "imweb_emscripten_glfw.hpp"

#include <emscripten.h>

#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "imweb_glfw_helper.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

struct ImWeb::Impl {
  Impl() {}
  ~Impl() {}

  GLFWwindow *m_window = NULL;
  std::optional<ImWebCallback> cb;

  static void emscripten_loop(void *this_ptr);
};

EM_JS(int, canvas_get_width, (), { return Module.canvas.width; });
EM_JS(int, canvas_get_height, (), { return Module.canvas.height; });
EM_JS(void, resizeCanvas, (), { js_resizeCanvas(); });

ImWeb::ImWeb() : ImWebBase(), impl{std::make_unique<ImWeb::Impl>()} {}
ImWeb::~ImWeb() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  GlfwHelper::destroy(&impl->m_window);
}

void ImWeb::initGL(int width, int height) {
  GlfwHelper::init();
  impl->m_window = GlfwHelper::createWindow(width, height);
}

void ImWeb::initImGui() {
  createImGuiContext();

  ImGui_ImplGlfw_InitForOpenGL(impl->m_window, true);
  ImGui_ImplOpenGL3_Init();

  setupImGui();

  resizeCanvas();
}

void ImWeb::Impl::emscripten_loop(void *this_ptr) {
  ImWeb *const imweb = (ImWeb *)this_ptr;
  auto const &impl = imweb->impl;

  int width = canvas_get_width();
  int height = canvas_get_height();

  if (width != imweb->getCurrentWidth() ||
      height != imweb->getCurrentHeight()) {
    glfwSetWindowSize(impl->m_window, width, height);
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
    imweb->setCurrentWidth(width);
    imweb->setCurrentHeight(height);

    glViewport(0, 0, width, height);
  }

  GlfwHelper::clearColor(imweb->getClearColor());

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  /* ----------------------------------------- */
  if (impl->cb.has_value()) {
    impl->cb.value()();
  } else {
    defaultUi();
  }
  /* ----------------------------------------- */

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwMakeContextCurrent(impl->m_window);
}

void ImWeb::loop(std::optional<ImWebCallback> cb) {
  impl->cb = cb;
  emscripten_set_main_loop_arg(ImWeb::Impl::emscripten_loop, this, 0, 1);
}