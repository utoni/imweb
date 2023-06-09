#ifndef IMWEB_EMSCRIPTEN_GLFW_H
#define IMWEB_EMSCRIPTEN_GLFW_H 1

#include <imweb.hpp>
#include <memory>

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
