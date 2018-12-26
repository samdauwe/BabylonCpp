#ifndef BABYLON_SAMPLES_MESHES_LOOK_AT_SCENE_H
#define BABYLON_SAMPLES_MESHES_LOOK_AT_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class AbstractMesh;
class ArcRotateCamera;
class StandardMaterial;
using AbstractMeshPtr     = std::shared_ptr<AbstractMesh>;
using ArcRotateCameraPtr  = std::shared_ptr<ArcRotateCamera>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

namespace Samples {

/**
 * @brief Look At Scene. Example demonstrating how to use lookAt() function to
 * align meshes on a specific target.
 *
 * How to use lookat to make an eye-candy effect ! :)
 * by Steve 'Stv' Duran for BabylonJS featured demos on 02.12.2015
 *
 * @see https://www.babylonjs-playground.com/#N2K3ZN#0
 */
class LookAtScene : public IRenderableScene {

public:
  LookAtScene(ICanvas* iCanvas);
  ~LookAtScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief Better random function.
   */
  float _rnd(float min, float max) const;

  /**
   * @brief Create random cubes in a box of 100x100x100.
   */
  void _createCubesBall(size_t num, Scene* scene);

private:
  float _t;
  ArcRotateCameraPtr _camera;
  StandardMaterialPtr _cubesMat;
  StandardMaterialPtr _sphereMat;
  AbstractMeshPtr _sphere;
  std::vector<AbstractMeshPtr> _cubes;
  Color3 _randomColor;
  Vector3 _randomDirection;

}; // end of class LookAtScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_LOOK_AT_SCENE_H
