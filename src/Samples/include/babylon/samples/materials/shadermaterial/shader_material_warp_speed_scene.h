#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_WARP_SPEED_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_WARP_SPEED_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/effect_shaders_store.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Warp speed scene. Example demonstrating on how to fly through stars
 * using a custom shader texture.
 * @see https://www.babylonjs-playground.com/#1WBBW0#1
 */
class ShaderMaterialWarpSpeedScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Attributes\n"
      "attribute vec3 position;\n"
      "attribute vec3 normal;\n"
      "attribute vec2 uv;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "\n"
      "// Varying\n"
      "varying vec4 vPosition;\n"
      "varying vec3 vNormal;\n"
      "\n"
      "void main() {\n"
      "  vec4 p = vec4(position, 1.);\n"
      "  vPosition = p;\n"
      "  vNormal = normal;\n"
      "  gl_Position = worldViewProjection * p;\n"
      "}\n";

  /** Pixel (Fragment) Shader **/
  // Interstellar ( https://www.shadertoy.com/view/Xdl3D2 )
  static constexpr const char* customFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Varying\n"
      "varying vec4 vPosition;\n"
      "varying vec3 vNormal;\n"
      "\n"
      "// Uniforms\n"
      "uniform sampler2D textureSampler;\n"
      "uniform sampler2D refSampler;\n"
      "uniform vec3 iResolution;\n"
      "uniform mat4 worldView;\n"
      "\n"
      "const float tau = 6.28318530717958647692;\n"
      "\n"
      "// Gamma correction\n"
      "#define GAMMA (2.2)\n"
      "\n"
      "vec3 ToLinear(in vec3 col) {\n"
      "  // Simulate a monitor, converting colour values into light values\n"
      "  return pow(col, vec3(GAMMA));\n"
      "}\n"
      "\n"
      "vec3 ToGamma(in vec3 col) {\n"
      "  // Convert back into colour values, so the correct light will come "
      "out of the\n"
      "  // monitor\n"
      "  return pow(col, vec3(1.0 / GAMMA));\n"
      "}\n"
      "\n"
      "vec4 Noise(in ivec2 x) {\n"
      "  return texture2D(refSampler, (vec2(x) + 0.5) / 256.0, -100.0);\n"
      "}\n"
      "\n"
      "vec4 Rand(in int x) {\n"
      "  vec2 uv;\n"
      "  uv.x = (float(x) + 0.5) / 256.0;\n"
      "  uv.y = (floor(uv.x) + 0.5) / 256.0;\n"
      "  return texture2D(refSampler, uv, -100.0);\n"
      "}\n"
      "\n"
      "uniform float time;\n"
      "\n"
      "void main(void) {\n"
      "\n"
      "  vec3 ray;\n"
      "  ray.xy = .2 * (vPosition.xy - vec2(.5));\n"
      "  ray.z = 1.;\n"
      "\n"
      "  float offset = time * .5;\n"
      "  float speed2 = (cos(offset) + 1.0) * 2.0;\n"
      "  float speed = speed2 + .1;\n"
      "  offset += sin(offset) * .96;\n"
      "  offset *= 2.0;\n"
      "\n"
      "  vec3 col = vec3(0.);\n"
      "\n"
      "  vec3 stp = ray / max(abs(ray.x), abs(ray.y));\n"
      "\n"
      "  vec3 pos = 2.0 * stp + .5;\n"
      "  for (int i = 0; i < 20; i++) {\n"
      "    float z = Noise(ivec2(pos.xy)).x;\n"
      "    z = fract(z - offset);\n"
      "    float d = 50.0 * z - pos.z;\n"
      "    float w = pow(max(0.0, 1.0 - 8.0 * length(fract(pos.xy) - .5)), "
      "                  2.0);\n"
      "    vec3 c = max(vec3(0),\n"
      "                 vec3(1.0 - abs(d + speed2 * .5) / speed, 1.0 - abs(d) "
      "                    / speed,\n"
      "                      1.0 - abs(d - speed2 * .5) / speed));\n"
      "    col += 1.5 * (1.0 - z) * c * w;\n"
      "    pos += stp;\n"
      "  }\n"
      "\n"
      "  gl_FragColor = vec4(ToGamma(col), 1.);\n"
      "}\n";

public:
  ShaderMaterialWarpSpeedScene(ICanvas* iCanvas);
  ~ShaderMaterialWarpSpeedScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  EffectShadersStore _effectShadersStore;
  float _time;
  ShaderMaterial* _shaderMaterial;

}; // end of class ShaderMaterialWarpSpeedScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_WARP_SPEED_SCENE_H
