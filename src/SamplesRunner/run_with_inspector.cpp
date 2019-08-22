#include "run_with_inspector.h"
#include <babylon/inspector/babylon_inspector_app.h>

// This file demonstrates how to run a scene inside the inspector window
void runWithInspector(
  std::shared_ptr<BABYLON::IRenderableScene> scene,
  const std::string & sceneName,
  bool flagScreenshotOneSampleAndExit
  )
{
  BABYLON::BabylonInspectorApp app;
  app.RunApp(scene, sceneName, flagScreenshotOneSampleAndExit);
}

