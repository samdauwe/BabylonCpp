#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_STATISTICS_TAB_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_STATISTICS_TAB_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/pane_component.h>

namespace BABYLON {

class EngineInstrumentation;
class SceneInstrumentation;

class BABYLON_SHARED_EXPORT StatisticsTabComponent : public PaneComponent {

public:
  StatisticsTabComponent(const IPaneComponentProps& props);
  ~StatisticsTabComponent() override;

  void componentWillMount();
  void componentWillUnmount();
  void render() override;

private:
  std::unique_ptr<SceneInstrumentation> _sceneInstrumentation;
  std::unique_ptr<EngineInstrumentation> _engineInstrumentation;

}; // end of class StatisticsTabComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_STATISTICS_TAB_COMPONENT_H
