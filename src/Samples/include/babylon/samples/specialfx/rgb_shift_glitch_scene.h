#ifndef BABYLON_SAMPLES_SPECIAL_FX_RGB_SHIFT_GLITCH_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_RGB_SHIFT_GLITCH_SCENE_H

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
 * @brief RGB Shift Glitch Scene. Example demonstrating how to use a custom post
 * process effect to simulate RGB Shift glitch.
 * @see https://www.babylonjs-playground.com/#TZJ0HQ#10
 */
class RGBShiftGlitchScene : public IRenderableScene {

public:
  /** Pixel (Fragment) Shader **/
  // RGB Shift Glitch (https://www.shadertoy.com/view/4t23Rc)
  static constexpr const char* rgbGlitchFX
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
      "#define AMPLITUDE 0.05\n"
      "#define SPEED 10.0\n"
      "\n"
      "vec4 rgbShift(in vec2 p , in vec4 shift) {\n"
      "    shift *= 2.0*shift.w - 1.0;\n"
      "    vec2 rs = vec2(shift.x,-shift.y);\n"
      "    vec2 gs = vec2(shift.y,-shift.z);\n"
      "    vec2 bs = vec2(shift.z,-shift.x);\n"
      "    float r = texture2D(textureSampler, p+rs, 0.0).x;\n"
      "    float g = texture2D(textureSampler, p+gs, 0.0).y;\n"
      "    float b = texture2D(textureSampler, p+bs, 0.0).z;\n"
      "    return vec4(r,g,b,1.0);\n"
      "}\n"
      "vec4 noise( in vec2 p ) {\n"
      "    return texture2D(noiseRef0, p, 0.0);\n"
      "}\n"
      "\n"
      "vec4 vec4pow( in vec4 v, in float p ) {\n"
      "    return vec4(pow(v.x,p),pow(v.y,p),pow(v.z,p),v.w);\n"
      "}\n"
      "\n"
      "void main(void){\n"
      "    vec2 p = vUV;\n"
      "    vec4 c = vec4(0.0,0.0,0.0,1.0);\n"
      "    vec4 shift = vec4pow(noise(vec2(SPEED*time,\n"
      "                                    2.0*SPEED*time/25.0 )),8.0)\n"
      "                * vec4(AMPLITUDE,AMPLITUDE,AMPLITUDE,1.0);\n"
      "    c += rgbShift(p, shift);\n"
      "   gl_FragColor = c;\n"
      "}\n";

public:
  RGBShiftGlitchScene(ICanvas* iCanvas);
  ~RGBShiftGlitchScene() override;

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

}; // end of class RGBShiftGlitchScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_RGB_SHIFT_GLITCH_SCENE_H
