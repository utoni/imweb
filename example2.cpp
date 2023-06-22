#include <imweb.hpp>
#include <iostream>

#include <math.h>

#include "imgui.h"
#include "implot.h"

static size_t const plot_size = 1001;

template <typename T> inline T RandomRange(T min, T max) {
  T scale = rand() / (T)RAND_MAX;
  return min + scale * (max - min);
}

template <size_t S> struct plot_data {
  std::array<float, S> xs, ys, ys1, ys2, ys3, ys4;
};

static void showSparkline(const char *id, const float *values, int count,
                          float min_v, float max_v, int offset,
                          const ImVec4 &col, const ImVec2 &size) {
  ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
  if (ImPlot::BeginPlot(id, size,
                        ImPlotFlags_CanvasOnly | ImPlotFlags_NoChild)) {
    ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations,
                      ImPlotAxisFlags_NoDecorations);
    ImPlot::SetupAxesLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
    ImPlot::SetNextLineStyle(col);
    ImPlot::SetNextFillStyle(col, 0.25);
    ImPlot::PlotLine(id, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
    ImPlot::EndPlot();
  }
  ImPlot::PopStyleVar();
}

static void showTables(bool *const anim, int *const offset) {
  static ImGuiTableFlags const flags =
      ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
      ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
      ImGuiTableFlags_Reorderable;

  ImGui::Begin("ImPlot/ImGui Tables");
  ImGui::BulletText("Plots can be used inside of ImGui tables as another means "
                    "of creating subplots.");
  ImGui::Checkbox("Animate", anim);
  if (*anim)
    *offset = (*offset + 1) % 100;
  if (ImGui::BeginTable("##table", 3, flags, ImVec2(-1, 0))) {
    ImGui::TableSetupColumn("Electrode", ImGuiTableColumnFlags_WidthFixed,
                            75.0f);
    ImGui::TableSetupColumn("Voltage", ImGuiTableColumnFlags_WidthFixed, 75.0f);
    ImGui::TableSetupColumn("EMG Signal");
    ImGui::TableHeadersRow();
    ImPlot::PushColormap(ImPlotColormap_Cool);
    for (int row = 0; row < 10; row++) {
      ImGui::TableNextRow();
      static float data[100];
      srand(row);
      for (int i = 0; i < 100; ++i)
        data[i] = RandomRange(0.0f, 10.0f);
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("EMG %d", row);
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%.3f V", data[*offset]);
      ImGui::TableSetColumnIndex(2);
      ImGui::PushID(row);
      showSparkline("##spark", data, 100, 0, 11.0f, *offset,
                    ImPlot::GetColormapColor(row), ImVec2(-1, 35));
      ImGui::PopID();
    }
    ImPlot::PopColormap();
    ImGui::EndTable();
  }
  ImGui::End();
}

static void showShadedPlots(struct plot_data<plot_size> &pd,
                            float *const alpha) {
  srand(0);
  for (size_t i = 0; i < plot_size; ++i) {
    pd.xs[i] = i * 0.001f;
    pd.ys[i] = 0.25f + 0.25f * sinf(25 * pd.xs[i]) * sinf(5 * pd.xs[i]) +
               RandomRange(-0.01f, 0.01f);
    pd.ys1[i] = pd.ys[i] + RandomRange(0.1f, 0.12f);
    pd.ys2[i] = pd.ys[i] - RandomRange(0.1f, 0.12f);
    pd.ys3[i] = 0.75f + 0.2f * sinf(25 * pd.xs[i]);
    pd.ys4[i] = 0.75f + 0.1f * cosf(25 * pd.xs[i]);
  }

  ImGui::Begin("ImPlot Shaded Plots");
  ImGui::DragFloat("Alpha", alpha, 0.01f, 0, 1);
  if (ImPlot::BeginPlot("Shaded Plots")) {
    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, *alpha);
    ImPlot::PlotShaded("Uncertain Data", &pd.xs[0], &pd.ys1[0], &pd.ys2[0],
                       plot_size);
    ImPlot::PlotLine("Uncertain Data", &pd.xs[0], &pd.ys[0], plot_size);
    ImPlot::PlotShaded("Overlapping", &pd.xs[0], &pd.ys3[0], &pd.ys4[0],
                       plot_size);
    ImPlot::PlotLine("Overlapping", &pd.xs[0], &pd.ys3[0], plot_size);
    ImPlot::PlotLine("Overlapping", &pd.xs[0], &pd.ys4[0], plot_size);
    ImPlot::PopStyleVar();
    ImPlot::EndPlot();
  }
  ImGui::End();
}

// It is possible to draw elements with a provided callback in loop(...).
static void showDemoWindow(bool *const show_another_window,
                           ImVec4 *const clear_color, float *const f,
                           int *const counter) {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::Begin("ImGui Demo");
  ImGui::Text("This is some useful text.");
  ImGui::Checkbox("Another Window", show_another_window);
  ImGui::SliderFloat("float", f, 0.0f, 1.0f);
  ImGui::ColorEdit3("clear color", (float *)clear_color);
  if (ImGui::Button("Button"))
    (*counter)++;
  ImGui::SameLine();
  ImGui::Text("counter = %d", *counter);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / io.Framerate, io.Framerate);
  ImGui::End();
}

// You can use a "kind-of-widget" approach as well and add it to ImWeb via
// addDrawable(...).
class WindowWidget : public ImWebDrawable {
public:
  WindowWidget(WindowWidget *p) : ImWebDrawable(p->m_id) {}
  WindowWidget() : ImWebDrawable("WindowWidget") {}
  ~WindowWidget() override {}
  bool draw() override {
    if (!show)
      return true;

    ImGui::Begin("Another Window", &show);
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me"))
      show = false;
    ImGui::End();

    return true;
  }

  // clang-format off

// pre-processor macro resolves to public
properties:
  bool show = true;

  // clang-format on
};

int main(int argc, char **argv) {
  ImWeb imweb;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  float f = 0.0f;
  int counter = 0;
  struct plot_data<plot_size> pd = {};
  float alpha = 0.25f;
  bool anim = true;
  int offset = 0;
  auto another_window = std::make_shared<WindowWidget>(new WindowWidget());

  imweb.init(1024, 768);
  imweb.addDrawable(another_window);
  imweb.loop([&]() {
    imweb.setClearColor(clear_color.x, clear_color.y, clear_color.z,
                        clear_color.w);
    showShadedPlots(pd, &alpha);
    showTables(&anim, &offset);
    showDemoWindow(&another_window->show, &clear_color, &f, &counter);

    return true;
  });

  return 0;
}
