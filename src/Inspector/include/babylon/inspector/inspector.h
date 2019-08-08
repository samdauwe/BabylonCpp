#ifndef BABYLON_INSPECTOR_INSPECTOR_H
#define BABYLON_INSPECTOR_INSPECTOR_H

#include <memory>

#include <babylon/babylon_api.h>
#include <functional>

struct GLFWwindow;

namespace BABYLON {

class ActionStore;
class ActionTabsComponent;
class GlobalState;
struct InspectorAction;
class Scene;
class SceneExplorerComponent;
class Tab;
using GlobalStatePtr = std::shared_ptr<GlobalState>;

class BABYLON_SHARED_EXPORT Inspector {

public:
  Inspector(GLFWwindow* glfwWindow, Scene* scene = nullptr);
  ~Inspector();

  Scene* scene() const;
  void setScene(Scene* scene);

  void intialize();
  void render();
  void dispose();

  // Global function that will be called when a new sample is selected
  // (This function is set from SampleLauncher/src/main.cpp)
  static std::function<void(const std::string &)> OnSampleChanged;

private:
  void _fileMenu();
  void _addActions();
  void _doMenuItem(InspectorAction& a, bool enabled);
  void _pushFonts();
  void _popFonts();
  void _renderInspector();

private:
  GLFWwindow* _glfwWindow;
  Scene* _scene;
  // Actions
  std::unique_ptr<ActionStore> _actionStore;
  // GUI
  bool _showInspectorWindow;
  int _menuHeight;
  // The global state
  GlobalStatePtr _globalState;
  // Widgets
  std::unique_ptr<SceneExplorerComponent> _sceneExplorerHost;
  std::unique_ptr<ActionTabsComponent> _actionTabsHost;
}; // end of class Inspector

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_INSPECTOR_H
