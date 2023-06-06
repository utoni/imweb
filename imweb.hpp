#ifndef IMWEB_H
#define IMWEB_H 1

#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#define IMWEB_VERSION "0.0.1"

using ImWebCallback = std::function<bool(void)>;

class ImWebBase {
public:
  ImWebBase();
  ~ImWebBase();
  ImWebBase(const ImWebBase &) = delete;

  void init(int width, int height) {
    initGL(width, height);
    initImGui();
  }
  virtual void initGL(int width, int height) = 0;
  virtual void initImGui() = 0;
  virtual void loop(std::optional<ImWebCallback> cb = {}) = 0;

  void setClearColor(float r, float g, float b, float a);
  void setCurrentWidth(int width);
  void setCurrentHeight(int height);
  std::array<float, 4> getClearColor() const;
  int getCurrentWidth() const;
  int getCurrentHeight() const;
  void createImGuiContext() const;
  void setupImGui() const;

  static void defaultUi();

private:
  struct Impl;
  std::unique_ptr<Impl> impl;
};

class ImWebException : public std::exception {
public:
  enum class ExceptionType { UNKNOWN = 0, GRAPHICS_BACKEND, IMGUI };

  ImWebException(ExceptionType type, std::string text);
  ImWebException(std::string) = delete;
  ImWebException(std::string &) = delete;
  ImWebException(const ImWebException &) = delete;
  const char *what() const noexcept override;
  ExceptionType which() const noexcept { return type; }

  static std::string type_to_str(const ExceptionType &type);

private:
  ExceptionType type;
  std::string text;
};

using ExceptionType = ImWebException::ExceptionType;
using ET = ExceptionType;

#ifdef __EMSCRIPTEN__
#include <imweb_emscripten_glfw.hpp>
#else
#include <imweb_native_glfw.hpp>
#endif

#endif
