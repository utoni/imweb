#ifndef IMWEB_EMSCRIPTEN_GLFW_H
#define IMWEB_EMSCRIPTEN_GLFW_H 1

#ifdef __EMSCRIPTEN__

#include <imweb.hpp>
#include <memory>
#include <optional>

class ImWeb : public ImWebBase {
public:
  ImWeb();
  ~ImWeb();

  void initGL(int width, int height) override;
  void initImGui() override;
  void loop(std::optional<ImWebCallback> cb = {}) override;

private:
  struct Impl;
  std::unique_ptr<Impl> impl;
};

#endif

#endif
