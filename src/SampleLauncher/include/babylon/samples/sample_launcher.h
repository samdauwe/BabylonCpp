#ifndef BABYLON_SAMPLES_SAMPLE_LAUNCHER_H
#define BABYLON_SAMPLES_SAMPLE_LAUNCHER_H

#include <babylon/babylon_stl.h>

#include <babylon/interfaces/irenderable_scene.h>

struct GLFWmonitor;
struct GLFWwindow;

namespace BABYLON {

class ICanvas;

namespace Samples {

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

private:
  int initGLFW();
  bool initGL();
  void updateWindowFPS();
  int initializeScene();
  void render();

private:
  State _editorState;
  GLFWmonitor* _monitor;
  GLFWwindow* _window;
  int _winResX, _winResY;
  std::string _title;
  bool _fullscreen;
  double _lastTime;
  int _frameCount;
  bool _sceneIntialized;
  std::unique_ptr<ICanvas> _renderCanvas;
  std::unique_ptr<IRenderableScene> _renderableScene;

}; // end of class SampleLauncher

} // end of namespace Samples
} // end of namespace BABYLON

#endif // BABYLON_SAMPLES_SAMPLE_LAUNCHER_H
