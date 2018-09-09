#ifndef BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SKY_MATERIAL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SKY_MATERIAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace MaterialsLibrary {
class SkyMaterial;
using SkyMaterialPtr = std::shared_ptr<SkyMaterial>;
} // end of namespace MaterialsLibrary

namespace Samples {

/**
 * @brief Scene demonstrating the use of the sky material from the materials
 * library.
 * Press the following keys:
 * - 1: Set Day
 * - 2: Set Evening
 * - 3: Increase Luminance
 * - 4: Decrease Luminance
 * - 5: Increase Turbidity
 * - 6: Decrease Turbidity
 * @see https://www.babylonjs-playground.com/#E6OZX#122
 */
class SkyMaterialScene : public IRenderableScene {

public:
  SkyMaterialScene(ICanvas* iCanvas);
  ~SkyMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  void setSkyConfig(const std::string& property, float from, float to);

private:
  MaterialsLibrary::SkyMaterialPtr _skyboxMaterial;

}; // end of class SkyMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_MATERIALS_LIBRARY_SKY_MATERIAL_SCENE_H
