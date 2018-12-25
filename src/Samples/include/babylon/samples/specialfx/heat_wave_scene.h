#ifndef BABYLON_SAMPLES_SPECIAL_FX_HEAT_WAVE_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_HEAT_WAVE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class PointLight;
class PostProcess;
class Texture;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using PointLightPtr      = std::shared_ptr<PointLight>;
using PostProcessPtr     = std::shared_ptr<PostProcess>;
using TexturePtr         = std::shared_ptr<Texture>;

namespace Samples {

/**
 * @brief Heatwave Scene. Example demonstrating how to use custom post process
 * effect to simulate heat wave effect.
 * @see https://www.babylonjs-playground.com/#TZJ0HQ#11
 */
class HeatWaveScene : public IRenderableScene {

public:
  static constexpr const char* thanosFX
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "varying vec2 vUV;\n"
      "uniform sampler2D textureSampler;\n"
      "uniform vec2 screenSize;\n"
      "\n"
      "uniform sampler2D noiseRef0;\n"
      "uniform sampler2D noiseRef1;\n"
      "\n"
      "uniform float time;\n"
      "\n"
      "void main(void){\n"
      "    vec2 unit = vUV/screenSize;\n"
      "    unit*=16.0+(sin(time*0.5)*50.0);\n"
      "    vec2 pos = vUV;\n"
      "    pos.x += sin(time*0.35);\n"
      "    pos.y -= time*0.2;\n"
      "    vec2 r = ((texture2D(noiseRef0, pos).rb)*2.0-1.0)*unit;\n"
      "    vec3 c = texture2D(textureSampler, vUV+r).rgb;\n"
      "   gl_FragColor = vec4(c, 1.0);\n"
      "}\n";

public:
  HeatWaveScene(ICanvas* iCanvas);
  ~HeatWaveScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  PointLightPtr _light;
  ArcRotateCameraPtr _camera;

  PostProcessPtr _postEffect;
  TexturePtr _noiseTexture0;
  TexturePtr _noiseTexture1;

  float _time;
  float _rate;

}; // end of class HeatWaveScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_HEAT_WAVE_SCENE_H
