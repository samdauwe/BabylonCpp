#ifndef BABYLON_INSPECTOR_SAMPLES_BROWSER_H
#define BABYLON_INSPECTOR_SAMPLES_BROWSER_H

#include <babylon/samples/samples_index.h>

namespace BABYLON {

class SamplesBrowserImpl;

class SamplesBrowser {
public:
  SamplesBrowser();
  ~SamplesBrowser();
  void render();

  using CallbackNewRenderableScene = std::function<void(std::shared_ptr<IRenderableScene>)>;
  CallbackNewRenderableScene OnNewRenderableScene;

  using CallbackEditFiles = std::function<void(const std::vector<std::string> &)>;
  CallbackEditFiles OnEditFiles;

  using CallbackLoopSamples = std::function<void(const std::vector<std::string> &)>;
  CallbackLoopSamples OnLoopSamples;

private:
  std::unique_ptr<SamplesBrowserImpl> pImpl;
}; // end of class SampleListComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_SAMPLES_BROWSER_H
