#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_SAMPLE_LIST_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_SAMPLE_LIST_COMPONENT_H

#include <babylon/inspector/components/actiontabs/pane_component.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {

class SampleListPageImpl;

class SampleListPage {
public:
  SampleListPage();
  ~SampleListPage();
  void render();

  using CallbackNewRenderableScene = std::function<void(std::shared_ptr<IRenderableScene>)>;
  CallbackNewRenderableScene OnNewRenderableScene;

  using CallbackEditFiles = std::function<void(const std::vector<std::string> &)>;
  CallbackEditFiles OnEditFiles;

private:
  std::unique_ptr<SampleListPageImpl> pImpl;
}; // end of class SampleListComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_SAMPLE_LIST_COMPONENT_H
