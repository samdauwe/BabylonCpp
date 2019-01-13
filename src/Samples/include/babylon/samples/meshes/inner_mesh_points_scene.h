#ifndef BABYLON_SAMPLES_MESHES_INNER_MESH_POINTS_SCENE_H
#define BABYLON_SAMPLES_MESHES_INNER_MESH_POINTS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Inner Mesh Points Scene. Example demonstrating how to create points
 * inside a mesh.
 * @see https://www.babylonjs-playground.com/#2K3T61#8
 * @see https://doc.babylonjs.com/snippets/innermeshpoints
 */
struct InnerMeshPointsScene : public IRenderableScene {

  InnerMeshPointsScene(ICanvas* iCanvas);
  ~InnerMeshPointsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct InnerMeshPointsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_INNER_MESH_POINTS_SCENE_H
