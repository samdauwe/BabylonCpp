#ifndef BABYLON_SAMPLES_SAMPLE_LAUNCHER_H
#define BABYLON_SAMPLES_SAMPLE_LAUNCHER_H

#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>

struct GLFWmonitor;
struct GLFWwindow;

namespace BABYLON {

class ICanvas;
class Inspector;

namespace Samples {

struct Window {
  // Window related variables
  GLFWwindow* glfwWindow   = nullptr;
  GLFWmonitor* glfwMonitor = nullptr;
  bool fullscreen          = false;
  int width                = 0;
  int height               = 0;
  std::string title        = "";
  bool intialized          = false;
  // Babylon scene related variables
  bool sceneIntialized                              = false;
  std::unique_ptr<ICanvas> renderCanvas             = nullptr;
  std::unique_ptr<IRenderableScene> renderableScene = nullptr;
  double lastTime                                   = 0.0;
  int frameCount                                    = 0;
}; // end of struct Window

struct SampleLauncherOptions;

class SampleLauncher {

public:
  enum class State {
    UNINITIALIZED  = 0,
    INITIALIZED    = 1,
    RUNNING        = 2,
    FINISHED       = 3,
    DESTROYED      = 4,
    ERRORCONDITION = 5,
  }; // end of enum class State

public:
  using ResolutionSize = std::pair<int, int>;
  static const ResolutionSize DESIGN_RESOLUTION_SIZE;
  static const ResolutionSize SMALL_RESOLUTION_SIZE;
  static const ResolutionSize MEDIUM_RESOLUTION_SIZE;
  static const ResolutionSize LARGE_RESOLUTION_SIZE;
  static const ResolutionSize FULL_RESOLUTION_SIZE;

public:
  SampleLauncher(const SampleLauncherOptions& options);
  ~SampleLauncher();

  bool intialize();

  /**
   * @brief Runs the sample.
   * @param runTime time in millseconds to run the sample, When set to 0, the
   * sample runs until the window is closed.
   * @return exit code
   */
  int run(long runTimeMillis = 0);

  void destroy();

  ICanvas* getRenderCanvas();
  void setRenderableScene(std::unique_ptr<IRenderableScene>& renderableScene);

public:
  static void CreateGLFWWindow(Window& window, int width, int height,
                               const std::string& title, GLFWmonitor* monitor,
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
#ifdef WITH_INSPECTOR
  bool _showInspectorWindow;
  // Inspector
  std::unique_ptr<Inspector> _inspector;
#endif
  // Other settings
  bool _useOpenGLES;

}; // end of class SampleLauncher

struct SampleLauncherOptions {
  std::string title        = "Sample";
  std::pair<int, int> size = SampleLauncher::SMALL_RESOLUTION_SIZE;
  bool showInspectorWindow = false;
}; // end of struct SampleLauncherOptions

} // end of namespace Samples
} // end of namespace BABYLON

#endif // BABYLON_SAMPLES_SAMPLE_LAUNCHER_H
