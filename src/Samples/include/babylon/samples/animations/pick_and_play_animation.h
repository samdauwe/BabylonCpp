#ifndef BABYLON_SAMPLES_ANIMATIONS_PICK_AND_PLACE_ANIMATION_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_PICK_AND_PLACE_ANIMATION_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class AbstractMesh;
class Animatable;
class EventState;
class PBRMetallicRoughnessMaterial;
class TransformNode;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;
using AnimatablePtr    = std::shared_ptr<Animatable>;
using TransformNodePtr = std::shared_ptr<TransformNode>;
using PBRMetallicRoughnessMaterialPtr = std::shared_ptr<PBRMetallicRoughnessMaterial>;

namespace Samples {

/**
 * @brief Pick And Play Animation Scene.
 * @see https://t.co/WeKMnLQ1Sf
 * @see https://doc.babylonjs.com/babylon101/animations
 */
struct PickAndPlayAnimationScene : public IRenderableScene {

public:
  PickAndPlayAnimationScene(ICanvas* iCanvas);
  ~PickAndPlayAnimationScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _t;
  PBRMetallicRoughnessMaterialPtr _mat0, _mat1, _mat2;
  Float32Array _mesh0Offsets;
  TransformNodePtr _mesh0;
  bool _animYEnabled;
  std::function<void(Scene*, EventState&)> _animY;
  AnimatablePtr _anim0, _animatableColor;
  AbstractMeshPtr _pickedMesh;

}; // end of struct PickAndPlayAnimationScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_PICK_AND_PLACE_ANIMATION_SCENE_H
