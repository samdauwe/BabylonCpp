#ifndef BABYLON_SAMPLES_ANIMATIONS_BONE_ROTATION_IN_WORLD_SPACE_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_BONE_ROTATION_IN_WORLD_SPACE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class AbstractMesh;
class Skeleton;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using SkeletonPtr     = std::shared_ptr<Skeleton>;

namespace Samples {

/**
 * @brief Bone Rotation In World Space Scene. Example demonstrating how to
 * rotate a bone around an axis, using the rotate function in world space.
 * @see https://www.babylonjs-playground.com/#D4ZZ8#2
 */
class BoneRotationInWorldSpaceScene : public IRenderableScene {

public:
  BoneRotationInWorldSpaceScene(ICanvas* iCanvas);
  ~BoneRotationInWorldSpaceScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  Vector3 _rotationAxis;
  AbstractMeshPtr _mesh;
  SkeletonPtr _skeleton;

}; // end of class AnimationsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_BONE_ROTATION_IN_WORLD_SPACE_SCENE_H
