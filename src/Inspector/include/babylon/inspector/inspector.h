#ifndef BABYLON_INSPECTOR_INSPECTOR_H
#define BABYLON_INSPECTOR_INSPECTOR_H

#include <memory>

#include <babylon/babylon_api.h>

struct GLFWwindow;

namespace BABYLON {

class ActionStore;
class ActionTabsComponent;
struct InspectorAction;
class Scene;
class Tab;

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
  bool _showInspectorWindow;
  int _menuHeight;
  // Widgets
  std::unique_ptr<ActionTabsComponent> _actionTabsHost;

}; // end of class Inspector

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_INSPECTOR_H
