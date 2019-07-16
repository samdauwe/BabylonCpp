#ifndef BABYLON_SAMPLES_MATERIALS_PBR_REFLECTION_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_PBR_REFLECTION_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief PBR reflection scene.
 * @see https://t.co/cSYTX8rQ5Y
 */
struct PBRReflectionScene : public IRenderableScene {

  PBRReflectionScene(ICanvas* iCanvas);
  ~PBRReflectionScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct PBRReflectionScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_PBR_REFLECTION_SCENE_H
