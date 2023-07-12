#ifndef IMWEB_H
#define IMWEB_H 1

#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#define IMWEB_VERSION "0.0.1"
#define properties public

using ImWebCallback = std::function<bool(void)>;

class ImWebDrawable {
public:
  explicit ImWebDrawable(std::string id);
  virtual ~ImWebDrawable();
  virtual bool draw() = 0;

  const std::string m_id;
};

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
  virtual void setTargetFramerate(double fps) = 0;
  virtual void loop(std::optional<ImWebCallback> cb = {}) = 0;
  virtual void stop() = 0;
  virtual bool isRunning() const = 0;
  virtual bool isInitialized() const = 0;

  bool draw();
  void setClearColor(float r, float g, float b, float a);
  void setCurrentWidth(int width);
  void setCurrentHeight(int height);
  std::array<float, 4> getClearColor() const;
  int getCurrentWidth() const;
  int getCurrentHeight() const;
  void createImGuiContext() const;
  void setupImGui() const;
  void addDrawable(std::shared_ptr<ImWebDrawable> drawable);
  bool delDrawable(std::shared_ptr<ImWebDrawable> drawable);
  std::vector<std::shared_ptr<ImWebDrawable>>::iterator drawablesBegin();
  std::vector<std::shared_ptr<ImWebDrawable>>::iterator drawablesEnd();
  std::size_t getDrawableCount() const;

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
