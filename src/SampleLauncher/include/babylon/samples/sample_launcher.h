#ifndef BABYLON_SAMPLES_SAMPLE_LAUNCHER_H
#define BABYLON_SAMPLES_SAMPLE_LAUNCHER_H

#include <babylon/babylon_stl.h>

#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>

struct GLFWmonitor;
struct GLFWwindow;

namespace BABYLON {

class ICanvas;

namespace Samples {

struct Window {
  // Window related variables
  GLFWwindow* glfwWindow   = nullptr;
  GLFWmonitor* glfwMonitor = nullptr;
  bool fullscreen          = false;
  int width                = 0;
  int height               = 0;
  string_t title           = "";
  bool intialized          = false;
  // Babylon scene related variables
  bool sceneIntialized                           = false;
  unique_ptr_t<ICanvas> renderCanvas             = nullptr;
  unique_ptr_t<IRenderableScene> renderableScene = nullptr;
  double lastTime                                = 0.0;
  int frameCount                                 = 0;
}; // end of struct Window

class SampleLauncher {

public:
  enum class State {
    UNINITIALIZED = 0,
    INITIALIZED   = 1,
    RUNNING       = 2,
    FINISHED      = 3,
    DESTROYED     = 4,
    ERROR         = 5,
  }; // end of enum class State

public:
  using ResolutionSize = std::pair<int, int>;
  static const ResolutionSize DESIGN_RESOLUTION_SIZE;
  static const ResolutionSize SMALL_RESOLUTION_SIZE;
  static const ResolutionSize MEDIUM_RESOLUTION_SIZE;
  static const ResolutionSize LARGE_RESOLUTION_SIZE;
  static const ResolutionSize FULL_RESOLUTION_SIZE;

public:
  SampleLauncher(const std::string& title   = "Sample",
                 const ResolutionSize& size = SMALL_RESOLUTION_SIZE);
  ~SampleLauncher();

  bool intialize();
  int run();
  void destroy();

  ICanvas* getRenderCanvas();
  void setRenderableScene(std::unique_ptr<IRenderableScene>& renderableScene);

public:
  static void CreateWindow(Window& window, int width, int height,
                           const string_t& title, GLFWmonitor* monitor,
                           Window* parentWindow);

private:
  int initGLFW();
  bool initGL(Window& window);
  void updateWindowFPS(Window& window);
  int initializeScene();
  void render();

private:
  State _sampleLauncherState;
  int _defaultWinResX, _defaultWinResY;

}; // end of class SampleLauncher

} // end of namespace Samples
} // end of namespace BABYLON

#endif // BABYLON_SAMPLES_SAMPLE_LAUNCHER_H
