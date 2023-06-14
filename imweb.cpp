#include "imweb.hpp"

#include "imgui.h"
#include "implot.h"

#ifdef EMBED_DATA
#include "webgui-data.c"
#endif

struct ImWebBase::Impl {
  Impl() {}
  ~Impl() {}

  ImPlotContext *implot = nullptr;
  ImVec4 clear_color = ImVec4{0.0f, 0.0f, 0.0f, 0.0f};
  int width = 0, height = 0;
};

ImWebBase::ImWebBase() : impl{std::make_unique<ImWebBase::Impl>()} {}
ImWebBase::~ImWebBase() {
  ImPlot::DestroyContext(impl->implot);
  ImGui::DestroyContext();
}

void ImWebBase::setClearColor(float r, float g, float b, float a) {
  impl->clear_color = ImVec4(r, g, b, a);
}

std::array<float, 4> ImWebBase::getClearColor() const {
  auto const &clear_color = impl->clear_color;

  return {clear_color.x, clear_color.y, clear_color.z, clear_color.w};
}

void ImWebBase::setCurrentWidth(int width) { impl->width = width; }

void ImWebBase::setCurrentHeight(int height) { impl->height = height; }

int ImWebBase::getCurrentWidth() const { return impl->width; }

int ImWebBase::getCurrentHeight() const { return impl->height; }

void ImWebBase::createImGuiContext() const {
  if (!IMGUI_CHECKVERSION())
    throw ImWebException(ET::IMGUI, "Version Check failed");
  if (!ImGui::CreateContext())
    throw ImWebException(ET::IMGUI, "Create IMGUI Context failed");
  impl->implot = ImPlot::CreateContext();
  if (!impl->implot)
    throw ImWebException(ET::IMGUI, "Create IMPLOT Context failed");
}

void ImWebBase::setupImGui() const {
  ImGui::StyleColorsDark();
  ImGuiIO &io = ImGui::GetIO();
#ifdef EMBED_DATA
  io.Fonts->AddFontFromMemoryTTF(xkcd_script_ttf, sizeof(xkcd_script_ttf), 23.0f);
  io.Fonts->AddFontFromMemoryTTF(xkcd_script_ttf, sizeof(xkcd_script_ttf), 18.0f);
  io.Fonts->AddFontFromMemoryTTF(xkcd_script_ttf, sizeof(xkcd_script_ttf), 26.0f);
  io.Fonts->AddFontFromMemoryTTF(xkcd_script_ttf, sizeof(xkcd_script_ttf), 32.0f);
#else
  io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 23.0f);
  io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 18.0f);
  io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 26.0f);
  io.Fonts->AddFontFromFileTTF("data/xkcd-script.ttf", 32.0f);
#endif
  io.Fonts->AddFontDefault();
}

void ImWebBase::defaultUi() {
  ImGui::Begin("Default ImWeb Window", nullptr);
  ImGui::Text("Hello from ImWeb!");
  ImGui::Text("~~~ (:) ~~~");
  ImGui::End();
}

ImWebException::ImWebException(ExceptionType type, std::string text)
    : type(type), text(text) {
  text = "[" + type_to_str(type) + "] " + text;
}

const char *ImWebException::what() const noexcept { return text.c_str(); }

std::string
ImWebException::type_to_str(const ImWebException::ExceptionType &type) {
  switch (type) {
  case ET::UNKNOWN:
    return "Unknown";
  case ET::GRAPHICS_BACKEND:
    return "Graphics Backend";
  case ET::IMGUI:
    return "Dear ImGui";
  }

  return "INVALID";
}
