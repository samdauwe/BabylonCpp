#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_DOUBLE_HELIX_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_DOUBLE_HELIX_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/effect_shaders_store.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialDoubleHelixScene : public IRenderableScene {

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
  // Double Helix ( https://www.shadertoy.com/view/Md3XRM )
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
      "uniform float iTime;\n"
      "uniform float iAspectRatio;\n"
      "uniform vec2 iResolution;\n"
      "\n"
      "vec3 LIGHT = normalize(vec3(-0.3, 0.2, -0.1));\n"
      "\n"
      "float FULL_SIZE = 2.0;\n"
      "float EDGE_SIZE = 0.2;\n"
      "float PAIR_SIZE = 0.2;\n"
      "\n"
      "vec3 n3(vec3 n) {\n"
      "  return fract(cos(dot(n, vec3(813, 12, 376))) *\n"
      "         vec3(901.81, 827.46, 615.79));\n"
      "}\n"
      "\n"
      "vec3 model(vec3 p) {\n"
      "  float A = p.z / 3.0 + iTime * 0.25;\n"
      "  vec3 R = vec3(cos(A), sin(A), 0);\n"
      "  vec3 C =\n"
      "      vec3(mod(p.xy + 8., 16.) - 8. + R.yx * vec2(1, -1),\n"
      "           fract(p.z) - 0.5);\n"
      "\n"
      "  float H =\n"
      "      min(length(C.xy + R.xy * FULL_SIZE),\n"
      "          length(C.xy - R.xy * FULL_SIZE)) *\n"
      "          0.5 -\n"
      "      EDGE_SIZE;\n"
      "  float P = max(length(vec2(dot(C.xy, R.yx * vec2(1, -1)), C.z)) -\n"
      "                PAIR_SIZE,\n"
      "                length(C.xy) - FULL_SIZE);\n"
      "\n"
      "  float T = FULL_SIZE + 0.01 + 2. * EDGE_SIZE - length(C.xy);\n"
      "  return vec3(min(H, P), T, P);\n"
      "}\n"
      "\n"
      "vec3 normal(vec3 p) {\n"
      "  vec2 N = vec2(-0.04, 0.04);\n"
      "\n"
      "  return normalize(model(p + N.xyy).x * N.xyy +\n"
      "                   model(p + N.yxy).x * N.yxy +\n"
      "                   model(p + N.yyx).x * N.yyx +\n"
      "                   model(p + N.xxx).x * N.xxx);\n"
      "}\n"
      "\n"
      "vec4 raymarch(vec3 p, vec3 d) {\n"
      "  vec4 M = vec4(p + d * 2.0, 0);\n"
      "  for (int i = 0; i < 100; i++) {\n"
      "    float S = model(M.xyz).x;\n"
      "    M += vec4(d, 1) * S;\n"
      "    if (S < 0.01 || M.w > 50.0)\n"
      "      break;\n"
      "  }\n"
      "  return M;\n"
      "}\n"
      "\n"
      "vec3 sky(vec3 d) {\n"
      "  float L = dot(d, LIGHT);\n"
      "  return vec3(0.3, 0.5, 0.6) - 0.3 * (-L * 0.5 + 0.5) +\n"
      "         exp2(32.0 * (L - 1.0));\n"
      "}\n"
      "\n"
      "vec3 color(vec3 p, vec3 d) {\n"
      "  vec2 M = model(p).yz;\n"
      "  float A = atan(mod(p.y + 8., 16.) - 8., 8. - mod(p.x + 8., 16.));\n"
      "  float T1 = ceil(fract(cos(floor(p.z) * 274.63)) - 0.5);\n"
      "  float T2 = sign(fract(cos(floor(p.z - 80.0) * 982.51)) - 0.5);\n"
      "  float T3 = T2 * sign(cos(p.z / 3.0 + iTime * 0.25 + A));\n"
      "\n"
      "  float L = dot(normal(p), LIGHT) * 0.5 + 0.5;\n"
      "  float R = max(dot(reflect(d, normal(p)), LIGHT), 0.0);\n"
      "  vec3 C = mix(mix(vec3(0.9 - 0.8 * T3, 0.9 - 0.6 * T3, T3),\n"
      "                   vec3(1.0 - 0.6 * T3, 0.2 + 0.8 * T3, 0.1 * T3),\n"
      "                   T1), vec3(0.2), step(0.01, M.y));\n"
      "  C = mix(C, vec3(0.2, 0.5, 1.0), step(0.01, -M.x));\n"
      "  return C * L + pow(R, 16.0);\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 iMouse = vec2(0.0, 0.0);\n"
      "  vec2 A = iMouse.xy / iResolution.xy * vec2(2, 1) * 3.1416;\n"
      "  vec3 D = vec3(cos(A.x) * sin(A.y), sin(A.x) * sin(A.y), cos(A.y));\n"
      "  D = mix(vec3(1, 0, 0), D, ceil((A.x + A.y) / 10.0));\n"
      "  vec3 P = D * 12.0 - vec3(0, 0, iTime * 2.0);\n"
      "\n"
      "  vec3 X = normalize(-D);\n"
      "  vec3 Y = normalize(cross(X, vec3(0, 0, 1)));\n"
      "  vec3 Z = normalize(cross(X, Y));\n"
      "\n"
      "  vec2 UV = (-1.0 + 2.0 * vUV.xy) -\n"
      "            (iResolution.xy * 0.05 / iResolution.y);\n"
      "  vec3 R = normalize(mat3(X, Y, Z) * vec3(1, UV));\n"
      "\n"
      "  vec4 M = raymarch(P, R);\n"
      "  vec3 C = mix(color(M.xyz, R), sky(R),\n"
      "               smoothstep(0.5, 1.0, M.w / 50.0));\n"
      "  gl_FragColor = vec4(C, 1);\n"
      "}\n";

public:
  ShaderMaterialDoubleHelixScene(ICanvas* iCanvas);
  ~ShaderMaterialDoubleHelixScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  EffectShadersStore _effectShadersStore;
  float _time;
  ShaderMaterial* _shaderMaterial;

}; // end of class ShaderMaterialDoubleHelixScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_DOUBLE_HELIX_SCENE_H
