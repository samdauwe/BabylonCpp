#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SKYBOX_CLOUDS_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SKYBOX_CLOUDS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ShaderMaterial;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;

namespace Samples {

class ShaderMaterialSkyboxCloudsScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Attributes\n"
      "attribute vec3 position;\n"
      "attribute vec2 uv;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "\n"
      "// Varying\n"
      "varying vec2 vUV;\n"
      "\n"
      "void main(void) {\n"
      "    gl_Position = worldViewProjection * vec4(position, 1.0);\n"
      "    vUV = uv;\n"
      "}\n";

  /** Pixel (Fragment) Shader **/
  // 2D Clouds ( https://www.shadertoy.com/view/4tdSWr )
  static constexpr const char* customFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Varying\n"
      "varying vec3 vPosition;\n"
      "varying vec3 vNormal;\n"
      "varying vec2 vUV;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "uniform float time;\n"
      "\n"
      "const float cloudscale = 1.1;\n"
      "const float speed = 0.03;\n"
      "const float clouddark = 0.5;\n"
      "const float cloudlight = 0.3;\n"
      "const float cloudcover = 0.2;\n"
      "const float cloudalpha = 8.0;\n"
      "const float skytint = 0.5;\n"
      "const vec3 skycolour1 = vec3(0.2, 0.4, 0.6);\n"
      "const vec3 skycolour2 = vec3(0.4, 0.7, 1.0);\n"
      "\n"
      "const mat2 m = mat2(1.6, 1.2, -1.2, 1.6);\n"
      "\n"
      "vec2 hash(vec2 p) {\n"
      "  p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));\n"
      "  return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);\n"
      "}\n"
      "\n"
      "float noise(in vec2 p) {\n"
      "  const float K1 = 0.366025404; // (sqrt(3)-1)/2;\n"
      "  const float K2 = 0.211324865; // (3-sqrt(3))/6;\n"
      "  vec2 i = floor(p + (p.x + p.y) * K1);\n"
      "  vec2 a = p - i + (i.x + i.y) * K2;\n"
      "  vec2 o =\n"
      "      (a.x > a.y)\n"
      "          ? vec2(1.0, 0.0)\n"
      "          : vec2(0.0, 1.0);\n"
      "  vec2 b = a - o + K2;\n"
      "  vec2 c = a - 1.0 + 2.0 * K2;\n"
      "  vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);\n"
      "  vec3 n = h * h * h * h * vec3(dot(a, hash(i + 0.0)),\n"
      "                                dot(b, hash(i + o)),\n"
      "                                dot(c, hash(i + 1.0)));\n"
      "  return dot(n, vec3(70.0));\n"
      "}\n"
      "float fbm(vec2 n) {\n"
      "  float total = 0.0, amplitude = 0.1;\n"
      "  for (int i = 0; i < 7; i++) {\n"
      "    total += noise(n) * amplitude;\n"
      "    n = m * n;\n"
      "    amplitude *= 0.4;\n"
      "  }\n"
      "  return total;\n"
      "}\n"
      "\n"
      "// -------------------------------------------------------------------\n"
      "\n"
      "void main(void) {\n"
      "  vec2 p = -1.0 + 2.0 * vUV.xy;\n"
      "  vec2 uv = vUV.xy;\n"
      "  float iTime = time * speed;\n"
      "  float q = fbm(uv * cloudscale * 0.5);\n"
      "\n"
      "  // ridged noise shape\n"
      "  float r = 0.0;\n"
      "  uv *= cloudscale;\n"
      "  uv -= q - iTime;\n"
      "  float weight = 0.8;\n"
      "  for (int i = 0; i < 8; i++) {\n"
      "    r += abs(weight * noise(uv));\n"
      "    uv = m * uv + iTime;\n"
      "    weight *= 0.7;\n"
      "  }\n"
      "\n"
      "  // noise shape\n"
      "  float f = 0.0;\n"
      "  uv = vUV.xy;\n"
      "  uv *= cloudscale;\n"
      "  uv -= q - iTime;\n"
      "  weight = 0.7;\n"
      "  for (int i = 0; i < 8; i++) {\n"
      "    f += weight * noise(uv);\n"
      "    uv = m * uv + iTime;\n"
      "    weight *= 0.6;\n"
      "  }\n"
      "\n"
      "  f *= r + f;\n"
      "\n"
      "  // noise colour\n"
      "  float c = 0.0;\n"
      "  iTime = time * speed * 2.0;\n"
      "  uv = vUV.xy;\n"
      "  uv *= cloudscale * 2.0;\n"
      "  uv -= q - iTime;\n"
      "  weight = 0.4;\n"
      "  for (int i = 0; i < 7; i++) {\n"
      "    c += weight * noise(uv);\n"
      "    uv = m * uv + iTime;\n"
      "    weight *= 0.6;\n"
      "  }\n"
      "\n"
      "  // noise ridge colour\n"
      "  float c1 = 0.0;\n"
      "  iTime = time * speed * 3.0;\n"
      "  uv = vUV.xy;\n"
      "  uv *= cloudscale * 3.0;\n"
      "  uv -= q - iTime;\n"
      "  weight = 0.4;\n"
      "  for (int i = 0; i < 7; i++) {\n"
      "    c1 += abs(weight * noise(uv));\n"
      "    uv = m * uv + iTime;\n"
      "    weight *= 0.6;\n"
      "  }\n"
      "\n"
      "  c += c1;\n"
      "\n"
      "  vec3 skycolour = mix(skycolour2, skycolour1, p.y);\n"
      "  vec3 cloudcolour =\n"
      "      vec3(1.1, 1.1, 0.9) * clamp((clouddark + cloudlight * c),\n"
      "           0.0, 1.0);\n"
      "\n"
      "  f = cloudcover + cloudalpha * f * r;\n"
      "\n"
      "  vec3 result =\n"
      "      mix(skycolour, clamp(skytint * skycolour + cloudcolour,\n"
      "          0.0, 1.0), clamp(f + c, 0.0, 1.0));\n"
      "\n"
      "  gl_FragColor = vec4(result, 1.0);\n"
      "}\n";

public:
  ShaderMaterialSkyboxCloudsScene(ICanvas* iCanvas);
  ~ShaderMaterialSkyboxCloudsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialSkyboxCloudsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_SKYBOX_CLOUDS_SCENE_H
