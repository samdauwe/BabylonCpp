#ifndef BABYLON_SAMPLES_OPTIMIZATIONS_CUBE_CHAOS_SCENE_H
#define BABYLON_SAMPLES_OPTIMIZATIONS_CUBE_CHAOS_SCENE_H

#include <vector>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class FreeCamera;
class InstancedMesh;
class Mesh;
class StandardMaterial;
using FreeCameraPtr       = std::shared_ptr<FreeCamera>;
using InstancedMeshPtr    = std::shared_ptr<InstancedMesh>;
using MeshPtr             = std::shared_ptr<Mesh>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

namespace Samples {

/**
 * @brief Cube Chaos Scene. Example demonstrating how to create animated scene
 * with cubes using instanced based rendering.
 * @see http://www.babylonjs-playground.com/#1E6NJO
 */
class CubeChaosScene : public IRenderableScene {

public:
  CubeChaosScene(ICanvas* iCanvas);
  ~CubeChaosScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _time;
  FreeCameraPtr _camera;
  MeshPtr _mainMesh;
  StandardMaterialPtr _mainMeshMaterial;
  std::vector<InstancedMeshPtr> _meshes;

}; // end of class CubeChaosScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_OPTIMIZATIONS_CUBE_CHAOS_SCENE_H
