#ifndef BABYLON_SAMPLES_SPECIAL_FX_DAWN_BRINGER_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_DAWN_BRINGER_SCENE_H

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
 * @brief Dawn Bringer Scene. Example demonstrating how to use custom post
 * process effect to simulate DawnBringer effect.
 * @see https://www.babylonjs-playground.com/#TZJ0HQ#9
 */
class DawnBringerScene : public IRenderableScene {

public:
  // Maps into DawnBringer's 4-bit (16 color) palette http://www.pixeljoint.com/forum/forum_posts.asp?TID=12795
  // (https://www.shadertoy.com/view/ldXSz4)
  static constexpr const char* dawnBringerFX
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
      "float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }\n"
      "float compare(vec3 a, vec3 b) {\n"
      "    a = max(vec3(0.0), a - min(a.r, min(a.g, a.b)) * 0.25);\n"
      "    b = max(vec3(0.0), b - min(b.r, min(b.g, b.b)) * 0.25);\n"
      "    a*=a*a;\n"
      "    b*=b*b;\n"
      "    vec3 diff = (a - b);\n"
      "    return dot(diff, diff);\n"
      "}\n"
      "\n"
      "#define PIXELSIZE       4.0\n"
      "\n"
      "void main(void){\n"
      "    vec2 c = floor((vUV*screenSize) / PIXELSIZE);\n"
      "    vec2 coord = c * PIXELSIZE;\n"
      "    vec3 src = texture2D(textureSampler, c/(screenSize/PIXELSIZE)).rgb;\n"
      "\n"
      "    // Track the two best colors\n"
      "    vec3 dst0 = vec3(0), dst1 = vec3(0);\n"
      "    float best0 = 1e3, best1 = 1e3;\n"
      "#define TRY(R, G, B) { const vec3 tst = vec3(R, G, B); float err = compare(src, tst); if (err < best0) { best1 = best0; dst1 = dst0; best0 = err; dst0 = tst; } }\n"
      "\n"
      "    TRY(0.078431, 0.047059, 0.109804);\n"
      "    TRY(0.266667, 0.141176, 0.203922);\n"
      "    TRY(0.188235, 0.203922, 0.427451);\n"
      "    TRY(0.305882, 0.290196, 0.305882);\n"
      "    TRY(0.521569, 0.298039, 0.188235);\n"
      "    TRY(0.203922, 0.396078, 0.141176);\n"
      "    TRY(0.815686, 0.274510, 0.282353);\n"
      "    TRY(0.458824, 0.443137, 0.380392);\n"
      "    TRY(0.349020, 0.490196, 0.807843);\n"
      "    TRY(0.823529, 0.490196, 0.172549);\n"
      "    TRY(0.521569, 0.584314, 0.631373);\n"
      "    TRY(0.427451, 0.666667, 0.172549);\n"
      "    TRY(0.823529, 0.666667, 0.600000);\n"
      "    TRY(0.427451, 0.760784, 0.792157);\n"
      "    TRY(0.854902, 0.831373, 0.368627);\n"
      "    TRY(0.870588, 0.933333, 0.839216);\n"
      "#undef TRY\n"
      "\n"
      "    best0 = sqrt(best0); best1 = sqrt(best1);\n"
      "    gl_FragColor = vec4(mod(c.x + c.y, 2.0) >  (hash(c * 2.0 + fract(sin(vec2(floor(time), floor(time * 1.7))))) * 0.75) + (best1 / (best0 + best1)) ? dst1 : dst0, 1.0);\n"
      "}\n";

public:
  DawnBringerScene(ICanvas* iCanvas);
  ~DawnBringerScene() override;

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

}; // end of class DawnBringerScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_DAWN_BRINGER_SCENE_H
