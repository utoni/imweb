#ifndef IMWEB_NATIVE_GLFW_H
#define IMWEB_NATIVE_GLFW_H 1

#ifndef __EMSCRIPTEN__

#include <atomic>
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
  void stop() override;
  bool isRunning() const override;
  bool isInitialized() const override;

private:
  std::atomic<bool> m_init = false;
  std::atomic<bool> m_running = false;
  struct Impl;
  std::unique_ptr<Impl> impl;
};

#endif

#endif
