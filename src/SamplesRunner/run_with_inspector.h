#pragma once
#include <memory>
#include <string>
#include <babylon/interfaces/irenderable_scene.h>

// This file demonstrates how to run a scene inside the inspector window
void runWithInspector(
  std::shared_ptr<BABYLON::IRenderableScene> scene,
  const std::string & sceneName,
  bool flagScreenshotOneSampleAndExit = false);
