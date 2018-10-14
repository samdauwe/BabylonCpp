#ifndef BABYLON_SAMPLES_MESHES_CONSTRUCTIVE_SOLID_GEOMETRIES_SCENE_H
#define BABYLON_SAMPLES_MESHES_CONSTRUCTIVE_SOLID_GEOMETRIES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Constructive_solid_geometries_scene. Example demonstrating how to
 * use boolean operations on meshes using CSG.
 * @see https://www.babylonjs-playground.com/#T6NP3F#0
 */
struct ConstructiveSolidGeometriesScene : public IRenderableScene {

  ConstructiveSolidGeometriesScene(ICanvas* iCanvas);
  ~ConstructiveSolidGeometriesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ConstructiveSolidGeometriesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_CONSTRUCTIVE_SOLID_GEOMETRIES_SCENE_H
