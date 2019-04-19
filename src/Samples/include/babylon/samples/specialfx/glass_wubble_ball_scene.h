#ifndef BABYLON_SAMPLES_SPECIAL_FX_GLASS_WUBBLE_BALL_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_GLASS_WUBBLE_BALL_SCENE_H

#include <babylon/babylon_common.h>

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Glass Wubble Ball Scene. This example demonstrates how to create a
 * warped ball effect.
 * @see https://www.babylonjs-playground.com/#CXOLW#3
 * @see
 * https://doc.babylonjs.com/api/classes/babylon.abstractmesh#getverticesdata
 */
class GlassWubbleBallScene : public IRenderableScene {

public:
  GlassWubbleBallScene(ICanvas* iCanvas);
  ~GlassWubbleBallScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _t;
  MeshPtr _sphere;
  Float32Array _v, _fv;

}; // end of struct GlassWubbleBallScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_GLASS_WUBBLE_BALL_SCENE_H
