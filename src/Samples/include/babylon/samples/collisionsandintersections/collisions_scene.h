#ifndef BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_COLLISIONS_SCENE_H
#define BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_COLLISIONS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Collisions Scene. Scene demonstrating how to handle basic collisions
 * to avoid a camera going through a box.
 * @see https://www.babylonjs-playground.com/#U8MEB0#0
 * @see
 * https://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
 */
struct CollisionsScene : public IRenderableScene {

  CollisionsScene(ICanvas* iCanvas);
  ~CollisionsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct CollisionsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_COLLISIONS_SCENE_H
