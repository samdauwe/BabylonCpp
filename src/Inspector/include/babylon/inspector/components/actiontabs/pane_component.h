#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_PANE_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_PANE_COMPONENT_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Scene;

struct BABYLON_SHARED_EXPORT IPaneComponentProps {
  std::string title;
  std::string icon;
  Scene* scene = nullptr;
}; // end of struct IPaneComponentProps

class BABYLON_SHARED_EXPORT PaneComponent {

public:
  PaneComponent(const IPaneComponentProps& props);
  virtual ~PaneComponent();

  virtual void render() = 0;

protected:
  IPaneComponentProps props;

}; // end of class PaneComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_PANE_COMPONENT_H
