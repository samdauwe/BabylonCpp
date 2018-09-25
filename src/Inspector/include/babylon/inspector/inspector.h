#ifndef BABYLON_INSPECTOR_INSPECTOR_H
#define BABYLON_INSPECTOR_INSPECTOR_H

#include <memory>

#include <babylon/babylon_api.h>

struct GLFWwindow;

namespace BABYLON {

class ActionStore;
struct InspectorAction;
class Scene;
class Tab;

class BABYLON_SHARED_EXPORT Inspector {

public:
  // Const statics
  // - Scene tab
  static constexpr unsigned int SCENE_TAB = 0;
  // - Logs tab
  static constexpr unsigned int LOGS_TAB = 1;
  // - Statistics tab
  static constexpr unsigned int STATS_TAB = 2;
  // - Mesh tab
  static constexpr unsigned int MESH_TAB = 3;
  // - Ligt tab
  static constexpr unsigned int LIGHT_TAB = 4;
  // - Material tab
  static constexpr unsigned int MATERIAL_TAB = 5;
  // - Camera tab
  static constexpr unsigned int CAMERA_TAB = 6;

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
  // Tabs
  std::array<std::unique_ptr<Tab>, 7> _tabs;

}; // end of class Inspector

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_INSPECTOR_H
