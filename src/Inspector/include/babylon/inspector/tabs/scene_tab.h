#ifndef BABYLON_INSPECTOR_TABS_SCENE_TAB_H
#define BABYLON_INSPECTOR_TABS_SCENE_TAB_H

#include <babylon/babylon_global.h>
#include <babylon/inspector/tabs/tab.h>

namespace BABYLON {

class Inspector;

class BABYLON_SHARED_EXPORT SceneTab : public Tab {

public:
  SceneTab(Inspector& inspector);
  ~SceneTab() override;

  void render() override;
  void dispose() override;

private:
  Inspector& _inspector;
  Scene* _scene;

}; // end of class SceneTab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_SCENE_TAB_H
