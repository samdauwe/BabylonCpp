#ifndef BABYLON_INSPECTOR_TABS_STATS_TAB_H
#define BABYLON_INSPECTOR_TABS_STATS_TAB_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/tabs/tab.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

class Engine;
class EngineInstrumentation;
class Inspector;
class Scene;
class SceneInstrumentation;

class BABYLON_SHARED_EXPORT StatsTab : public Tab {

public:
  StatsTab(Inspector& inspector);
  ~StatsTab() override;

  void render() override;
  void dispose() override;

private:
  void _connectToInstrumentation();

private:
  Inspector& _inspector;
  Scene* _scene;
  Engine* _engine;
  GL::GLInfo _glInfo;
  std::unique_ptr<SceneInstrumentation> _sceneInstrumentation;
  std::unique_ptr<EngineInstrumentation> _engineInstrumentation;

}; // end of class StatsTab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_STATS_TAB_H
