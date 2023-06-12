#include <optional>
#include <iostream>

#include <imgui.h>
#include <implot.h>
#include <imweb.hpp>

int main(void) {
  std::cout << "[VERSIONS]" << std::endl;

  std::cout << "\tImGui............: " << IMGUI_VERSION << " (" << IMGUI_VERSION_NUM << ")" << std::endl;
  if (std::string(IMGUI_VERSION) != std::string(ImGui::GetVersion()))
    throw "ImGui version mismatch: " + std::string(IMGUI_VERSION) + " != " + std::string(ImGui::GetVersion());
  if (!IMGUI_CHECKVERSION())
    throw "ImGui version check failed!";

  std::cout << "\tImPlot...........: " << IMPLOT_VERSION << std::endl;

  std::cout << "\tImWeb............: " << IMWEB_VERSION << std::endl;

  return 0;
}
