#ifndef BABYLON_SAMPLES_MESHES_IS_POINT_INSIDE_MESH_SCENE_H
#define BABYLON_SAMPLES_MESHES_IS_POINT_INSIDE_MESH_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Is Point Inside Mesh Scene. Example demonstrating how to check when a
 * point is inside a mesh.
 * @see https://www.babylonjs-playground.com/#XJEG9A#2
 * @see https://doc.babylonjs.com/snippets/isinside
 */
struct IsPointInsideMeshScene : public IRenderableScene {

  IsPointInsideMeshScene(ICanvas* iCanvas);
  ~IsPointInsideMeshScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct IsPointInsideMeshScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_IS_POINT_INSIDE_MESH_SCENE_H
