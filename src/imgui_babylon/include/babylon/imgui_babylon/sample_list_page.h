#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_SAMPLE_LIST_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_SAMPLE_LIST_COMPONENT_H

#include <babylon/inspector/components/actiontabs/pane_component.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {

class SampleListPage {
public:
  SampleListPage();
  void render();

  using CallbackNewRenderableScene = std::function<void(std::shared_ptr<IRenderableScene>)>;
  CallbackNewRenderableScene OnNewRenderableScene;

private:
  void guiOneSample(const std::string &sampleName);
  void guiBrowseSamples();
  
  BABYLON::Samples::SamplesIndex _samplesIndex;
}; // end of class SampleListComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_SAMPLE_LIST_COMPONENT_H
