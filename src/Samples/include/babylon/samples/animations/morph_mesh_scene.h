#ifndef BABYLON_SAMPLES_ANIMATIONS_MORPH_MESH_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_MORPH_MESH_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class MorphTarget;
using MorphTargetPtr = std::shared_ptr<MorphTarget>;

namespace Samples {

/**
 * @brief Morph Mesh Scene. Demo to show how to morph meshes with BabylonJS.
 * @see https://github.com/kekkorider/babylonjs-mesh-morph
 * @see https://doc.babylonjs.com/how_to/how_to_use_morphtargets
 */
class MorphMeshScene : public IRenderableScene {

public:
  MorphMeshScene(ICanvas* iCanvas);
  ~MorphMeshScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  high_res_time_point_t _previousTimestamp;
  MorphTargetPtr _testMorphTarget;
  int _currentAnim;

}; // end of class MorphMeshScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_MORPH_MESH_SCENE_H
