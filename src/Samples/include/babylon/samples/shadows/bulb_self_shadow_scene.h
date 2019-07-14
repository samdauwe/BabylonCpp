#ifndef BABYLON_SAMPLES_SHADOWS_BULB_SELF_SHADOW_SCENE_H
#define BABYLON_SAMPLES_SHADOWS_BULB_SELF_SHADOW_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Mesh;
class PointLight;
class SpotLight;
using MeshPtr       = std::shared_ptr<Mesh>;
using PointLightPtr = std::shared_ptr<PointLight>;
using SpotLightPtr  = std::shared_ptr<SpotLight>;

namespace Samples {

/**
 * @brief bulb self Shadow Scene. Example scene demonstrating how to setup a
 * scene with a lbulb & self shadow.
 * @see https://t.co/LVv21x5ha2
 */
class BulbSelfShadowScene : public IRenderableScene {

public:
  BulbSelfShadowScene(ICanvas* iCanvas);
  ~BulbSelfShadowScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _i;
  Vector3 _lightPos;
  Color3 _lightDiffuse;
  SpotLightPtr _light0;
  PointLightPtr _light1;
  MeshPtr _torus;
  MeshPtr _bulb;

}; // end of class BulbSelfShadowScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SHADOWS_BULB_SELF_SHADOW_SCENE_H
