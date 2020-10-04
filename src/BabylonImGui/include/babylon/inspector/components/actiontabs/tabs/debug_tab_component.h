#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_DEBUG_TAB_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_DEBUG_TAB_COMPONENT_H

#include <babylon/babylon_api.h>

#include <babylon/inspector/components/actiontabs/pane_component.h>

namespace BABYLON {

class EngineInstrumentation;
class SceneInstrumentation;

class BABYLON_SHARED_EXPORT DebugTabComponent : public PaneComponent {

public:
  DebugTabComponent(const IPaneComponentProps& props);
  ~DebugTabComponent() override;

  void componentWillMount();
  void componentWillUnmount();
  void switchSkeletonViewers();
  void switchPhysicsViewers();
  void render() override;

private:
  bool _physicsViewersEnabled;

}; // end of class StatisticsTabComponent

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_DEBUG_TAB_COMPONENT_H
