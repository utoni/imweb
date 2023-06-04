#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include "glad/glad.h"
#endif

#ifdef __EMSCRIPTEN_
#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

static GLFWwindow *g_window;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static bool show_demo_window = true;
static bool show_another_window = false;
static int g_width = 1024;
static int g_height = 768;

#ifdef __EMSCRIPTEN__
EM_JS(int, canvas_get_width, (), { return Module.canvas.width; });
EM_JS(int, canvas_get_height, (), { return Module.canvas.height; });
EM_JS(void, resizeCanvas, (), { js_resizeCanvas(); });
#endif

#ifndef __EMSCRIPTEN__
static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
#endif

static void on_size_changed() {
  glfwSetWindowSize(g_window, g_width, g_height);

  ImGui::SetCurrentContext(ImGui::GetCurrentContext());
}

static void loop() {
#ifdef __EMSCRIPTEN__
  int width = canvas_get_width();
  int height = canvas_get_height();

  if (width != g_width || height != g_height) {
    g_width = width;
    g_height = height;
    on_size_changed();
  }
#endif

#ifndef __EMSCRIPTEN__
  while (!glfwWindowShouldClose(g_window))
#endif
  {
#ifndef __EMSCRIPTEN__
    glfwSwapBuffers(g_window);
#endif
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      static float f = 0.0f;
      static int counter = 0;
      ImGui::Text("Hello, world!");
      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::ColorEdit3("clear color", (float *)&clear_color);

      ImGui::Checkbox("Demo Window", &show_demo_window);
      ImGui::Checkbox("Another Window", &show_another_window);

      if (ImGui::Button("Button"))
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    if (show_another_window) {
      ImGui::Begin("Another Window", &show_another_window);
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;
      ImGui::End();
    }

    if (show_demo_window) {
      ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
      ImGui::ShowDemoWindow(&show_demo_window);
    }

    {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(g_width, g_height));
      ImGui::Begin("Overlay", nullptr,
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs |
                       ImGuiWindowFlags_NoMouseInputs |
                       ImGuiWindowFlags_NoBackground |
                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                       ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration);

      auto pDrawList = ImGui::GetWindowDrawList();

      pDrawList->AddRect(ImVec2(25, 25), ImVec2(225, 225), ImColor(255, 0, 0));
      pDrawList->AddText(ImVec2(25, 25), ImColor(255, 0, 0), "test");

      ImGui::End();
    }

    ImGui::Render();

    int display_w, display_h;
    glfwMakeContextCurrent(g_window);
    glfwGetFramebufferSize(g_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwMakeContextCurrent(g_window);
  }
}

#ifndef __EMSCRIPTEN__
static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
}
#endif

static int init_gl() {
#ifndef __EMSCRIPTEN__
  glfwSetErrorCallback(glfw_error_callback);
#endif

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return 1;
  }

  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
#ifndef __EMSCRIPTEN__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

  int canvasWidth = g_width;
  int canvasHeight = g_height;
  g_window =
      glfwCreateWindow(canvasWidth, canvasHeight, "WebGui Demo", NULL, NULL);
  if (g_window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(g_window);

#ifndef __EMSCRIPTEN__
  glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
#endif

  return 0;
}

static int init_imgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(g_window, true);
  ImGui_ImplOpenGL3_Init();

  ImGui::StyleColorsDark();

  ImGuiIO &io = ImGui::GetIO();

  io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 23.0f);
  io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 18.0f);
  io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 26.0f);
  io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 32.0f);
  io.Fonts->AddFontDefault();

#ifdef __EMSCRIPTEN__
  resizeCanvas();
#endif

  return 0;
}

static int init() {
  init_gl();
  init_imgui();
  return 0;
}

static void quit() { glfwTerminate(); }

extern "C" int main(int argc, char **argv) {
#ifdef __EMSCRIPTEN__
  g_width = canvas_get_width();
  g_height = canvas_get_height();
#endif
  if (init() != 0)
    return 1;

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#else
  loop();
#endif

  quit();

  return 0;
}
