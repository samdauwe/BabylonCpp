#ifndef BABYLON_SAMPLES_SPECIAL_FX_FADE_IN_OUT_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_FADE_IN_OUT_SCENE_H

#include <babylon/babylon_common.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Environment scene. This example demonstrates how to fade in and out a
 * whole scene.
 * @see http://www.babylonjs-playground.com/#2FGYE8#2
 */
class FadeInOutScene : public IRenderableScene {

public:
  static constexpr const char* fadePixelShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "varying vec2 vUV;\n"
      "\n"
      "uniform sampler2D textureSampler;\n"
      "uniform float fadeLevel;\n"
      "\n"
      "void main(void) {\n"
      "  vec4 baseColor = texture2D(textureSampler, vUV) * fadeLevel;\n"
      "  baseColor.a = 1.0;\n"
      "  gl_FragColor = baseColor;\n"
      "}\n";

public:
  FadeInOutScene(ICanvas* iCanvas);
  ~FadeInOutScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  high_res_time_point_t _startTime;
  float _msFade;
  float _fadeLevel;

}; // end of class FadeInOutScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_FADE_IN_OUT_SCENE_H
