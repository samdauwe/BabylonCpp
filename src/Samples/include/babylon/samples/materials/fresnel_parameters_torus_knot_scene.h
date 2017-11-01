#ifndef BABYLON_SAMPLES_MATERIALS_FRESNEL_PARAMETERS_TORUS_KNOT_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_FRESNEL_PARAMETERS_TORUS_KNOT_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct FresnelParametersTorusKnotScene : public IRenderableScene {

  FresnelParametersTorusKnotScene(ICanvas* iCanvas);
  ~FresnelParametersTorusKnotScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct FresnelParametersTorusKnotScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_FRESNEL_PARAMETERS_TORUS_KNOT_SCENE_H
