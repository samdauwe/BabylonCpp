#ifndef BABYLON_SAMPLES_MESHES_RAY_HELPER_SCENE_H
#define BABYLON_SAMPLES_MESHES_RAY_HELPER_SCENE_H

#include <babylon/debug/ray_helper.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Ray Helper Scene. Example demonstrating how to use the RayHelper class
 * for easily viewing and attaching a ray to a mesh..
 * @see http://www.babylonjs-playground.com/#ZHDBJ#35
 */
class RayHelperScene : public IRenderableScene {

public:
  RayHelperScene(ICanvas* iCanvas);
  ~RayHelperScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  // Meshes
  MeshPtr _box;
  MeshPtr _boxTarget;
  MeshPtr _ground;
  MeshPtr _sphere;
  std::vector<AbstractMesh*> _meshes;
  // Ray helper
  Ray _ray;
  RayHelper _rayHelper;

}; // end of class RayHelperScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_RAY_HELPER_SCENE_H
