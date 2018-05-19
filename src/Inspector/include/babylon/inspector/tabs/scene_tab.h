#ifndef BABYLON_INSPECTOR_TABS_SCENE_TAB_H
#define BABYLON_INSPECTOR_TABS_SCENE_TAB_H

#include <babylon/babylon_global.h>
#include <babylon/inspector/tabs/tab.h>

namespace BABYLON {

class Inspector;
class PropertiesView;

class BABYLON_SHARED_EXPORT SceneTab : public Tab {

public:
  SceneTab(Inspector& inspector);
  ~SceneTab() override;

  void render() override;
  void dispose() override;

private:
  void _buildPropertiesView();
  void _renderActions();
  void _renderProperties();

private:
  Inspector& _inspector;
  Scene* _scene;
  unique_ptr_t<PropertiesView> _properties;
  bool _splitterIntialized;
  float _width;
  float _sz1;
  float _sz2;

}; // end of class SceneTab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_SCENE_TAB_H
