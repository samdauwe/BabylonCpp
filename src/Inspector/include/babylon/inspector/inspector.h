#ifndef BABYLON_INSPECTOR_INSPECTOR_H
#define BABYLON_INSPECTOR_INSPECTOR_H

#include <babylon/babylon_global.h>

struct GLFWwindow;

namespace BABYLON {

class ActionStore;
struct InspectorAction;
class StatsTab;

class BABYLON_SHARED_EXPORT Inspector {

public:
  Inspector(GLFWwindow* glfwWindow, Scene* scene = nullptr);
  ~Inspector();

  Scene* scene() const;
  void setScene(Scene* scene);

  void intialize();
  void render();
  void dispose();

private:
  void _fileMenu();
  void _addActions();
  void _doMenuItem(InspectorAction& a, bool enabled);
  void _renderDockGUI();

private:
  GLFWwindow* _glfwWindow;
  Scene* _scene;
  // Actions
  std::unique_ptr<ActionStore> _actionStore;
  // GUI
  bool _showDockingWindow;
  int _menuHeight;
  // - Tabs
  unique_ptr_t<StatsTab> _statsTab;

}; // end of class Inspector

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_INSPECTOR_H
