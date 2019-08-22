#pragma once
#include <memory>
#include <string>
#include <babylon/interfaces/irenderable_scene.h>

// This file demonstrates how to run a standalone scene, using imgui as a backend
void runStandalone_imgui(
  std::shared_ptr<BABYLON::IRenderableScene> scene,
  const std::string & sceneName);
