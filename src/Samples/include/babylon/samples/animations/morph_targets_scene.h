#ifndef BABYLON_SAMPLES_ANIMATIONS_MORPH_TARGETS_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_MORPH_TARGETS_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
class MorphTarget;
using MeshPtr        = std::shared_ptr<Mesh>;
using MorphTargetPtr = std::shared_ptr<MorphTarget>;

namespace Samples {

/**
 * @brief Morph Targets Scene. Example demonstrating how to morph a mesh between
 * multiple targets
 * @see https://www.babylonjs-playground.com/#2JDN66#7
 * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
class MorphTargetsScene : public IRenderableScene {

public:
  MorphTargetsScene(ICanvas* iCanvas);
  ~MorphTargetsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  void _addSpike(const MeshPtr& mesh);

private:
  MorphTargetPtr _target0;
  MorphTargetPtr _target1;
  MorphTargetPtr _target2;
  MorphTargetPtr _target3;

}; // end of class MorphTargetsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_MORPH_TARGETS_SCENE_H
