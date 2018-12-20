#ifndef BABYLON_SAMPLES_PROCEDURAL_TEXTURES_SCENE_H
#define BABYLON_SAMPLES_PROCEDURAL_TEXTURES_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;

namespace Samples {

/**
 * @brief Procedural Textures Scene. Example demonstrating how to use procedual
 * textures for wood, grass, marble, fire, etc.
 * @see https://www.babylonjs-playground.com/#B2ZXG6#0
 * @see https://doc.babylonjs.com/how_to/how_to_use_procedural_textures
 */
class ProceduralTexturesScene : public IRenderableScene {

public:
  /** Pixel (Fragment) Shader **/
  static constexpr const char* customProceduralTexturePixelShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "varying vec2 vPosition;\n"
      "varying vec2 vUV;\n"
      "\n"
      "uniform sampler2D dirtSampler;\n"
      "uniform sampler2D grassSampler;\n"
      "uniform float dirtAmplifier;\n"
      "\n"
      "float rand(vec2 n) {\n"
      "    return fract(cos(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);\n"
      "}\n"
      "\n"
      "float noise(vec2 n) {\n"
      "    const vec2 d = vec2(0.0, 1.0);\n"
      "    vec2 b = floor(n);\n"
      "    vec2 f = smoothstep(vec2(0.0), vec2(1.0), fract(n));\n"
      "    return mix(mix(rand(b), rand(b + d.yx), f.x),\n"
      "               mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);\n"
      "}\n"
      "\n"
      "float fbm(vec2 n) {\n"
      "    float total = 0.0, amplitude = 0.5;\n"
      "    for (int i = 0; i < 4; i++) {\n"
      "        total += noise(n) * amplitude;\n"
      "        n += n;\n"
      "        amplitude *= 0.5;\n"
      "    }\n"
      "    return total;\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "    vec3 color = mix(texture2D(dirtSampler, vUV).xyz,\n"
      "                     texture2D(grassSampler, vUV).xyz,\n"
      "                     fbm(vUV * dirtAmplifier));\n"
      "    gl_FragColor = vec4(color, 1.0);\n"
      "}\n";

public:
  ProceduralTexturesScene(ICanvas* iCanvas);
  ~ProceduralTexturesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;

}; // end of class ProceduralTexturesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_PROCEDURAL_TEXTURES_SCENE_H
