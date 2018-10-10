#ifndef BABYLON_SAMPLES_MATERIALS_GLOSSINESS_AND_ROUGHNESS_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_GLOSSINESS_AND_ROUGHNESS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Glossiness and roughness scene. Example demonstrating glossiness and
 * rouphness features of standard material.
 * @see https://www.babylonjs-playground.com/#RNBKQ#8
 */
struct GlossinessAndRoughnessScene : public IRenderableScene {

  GlossinessAndRoughnessScene(ICanvas* iCanvas);
  ~GlossinessAndRoughnessScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct GlossinessAndRoughnessScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_GLOSSINESS_AND_ROUGHNESS_SCENE_H
