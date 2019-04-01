#ifndef BABYLON_SAMPLES_ANIMATIONS_ANIMATED_MORPH_TARGET_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_ANIMATED_MORPH_TARGET_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class MorphTarget;
using MorphTargetPtr = std::shared_ptr<MorphTarget>;

namespace Samples {

/**
 * @brief Animated Morph Target Scene. Example demonstrating how to morph a mesh
 * between multiple targets.
 * @see https://www.babylonjs-playground.com/#HPV2TZ#8
 * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
class AnimatedMorphTargetScene : public IRenderableScene {

public:
  AnimatedMorphTargetScene(ICanvas* iCanvas);
  ~AnimatedMorphTargetScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  void _scrambleUp(Float32Array& data);
  void _scrambleDown(Float32Array& data);

private:
  float _angle;
  MorphTargetPtr _target0;

}; // end of class AnimatedMorphTargetScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_ANIMATED_MORPH_TARGET_SCENE_H
