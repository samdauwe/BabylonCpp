#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_STAR_NEST_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_STAR_NEST_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/effect_shaders_store.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialStarNestScene : public IRenderableScene {

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
  // Star Nest ( https://www.shadertoy.com/view/XlfGRj )
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
      "// Star Nest by Pablo RomÃ¡n Andrioli\n"
      "\n"
      "// This content is under the MIT License.\n"
      "\n"
      "#define iterations 17\n"
      "#define formuparam 0.53\n"
      "\n"
      "#define volsteps 20\n"
      "#define stepsize 0.1\n"
      "\n"
      "#define zoom 0.800\n"
      "#define tile 0.850\n"
      "#define speed 0.010\n"
      "\n"
      "#define brightness 0.0015\n"
      "#define darkmatter 0.300\n"
      "#define distfading 0.730\n"
      "#define saturation 0.850\n"
      "\n"
      "void main(void) {\n"
      "  // get coords and direction\n"
      "  vec2 uv = -1.0 + 2.0 * vUV.xy;\n"
      "  uv.y *= iResolution.y / iResolution.x;\n"
      "  vec3 dir = vec3(uv * zoom, 1.);\n"
      "  vec2 iMouse = vec2(0.0, 0.0);\n"
      "  float time = iTime * speed + .25;\n"
      "\n"
      "  // mouse rotation\n"
      "  float a1 = .5 + iMouse.x / iResolution.x * 2.;\n"
      "  float a2 = .8 + iMouse.y / iResolution.y * 2.;\n"
      "  mat2 rot1 = mat2(cos(a1), sin(a1), -sin(a1), cos(a1));\n"
      "  mat2 rot2 = mat2(cos(a2), sin(a2), -sin(a2), cos(a2));\n"
      "  dir.xz *= rot1;\n"
      "  dir.xy *= rot2;\n"
      "  vec3 from = vec3(1., .5, 0.5);\n"
      "  from += vec3(time * 2., time, -2.);\n"
      "  from.xz *= rot1;\n"
      "  from.xy *= rot2;\n"
      "\n"
      "  // volumetric rendering\n"
      "  float s = 0.1, fade = 1.;\n"
      "  vec3 v = vec3(0.);\n"
      "  for (int r = 0; r < volsteps; r++) {\n"
      "    vec3 p = from + s * dir * .5;\n"
      "    // tiling fold\n"
      "    p = abs(vec3(tile) - mod(p, vec3(tile * 2.)));\n"
      "    float pa, a = pa = 0.;\n"
      "    for (int i = 0; i < iterations; i++) {\n"
      "      // the magic formula\n"
      "      p = abs(p) / dot(p, p) - formuparam;\n"
      "      // absolute sum of average change\n"
      "      a += abs(length(p) - pa);\n"
      "      pa = length(p);\n"
      "    }\n"
      "    // dark matter\n"
      "    float dm = max(0., darkmatter - a * a * .001);\n"
      "    // add contrast\n"
      "    a *= a * a;\n"
      "    if (r > 6) {\n"
      "      // dark matter, don't render near\n"
      "      fade *= 1. - dm;\n"
      "    }\n"
      "    // v+=vec3(dm,dm*.5,0.);\n"
      "    v += fade;\n"
      "    // coloring based on distance\n"
      "    v += vec3(s, s * s, s * s * s * s) * a * brightness * fade;\n"
      "    // distance fading\n"
      "    fade *= distfading;\n"
      "    s += stepsize;\n"
      "  }\n"
      "  v = mix(vec3(length(v)), v, saturation); // color adjust\n"
      "  gl_FragColor = vec4(v * .01, 1.);\n"
      "}\n";

public:
  ShaderMaterialStarNestScene(ICanvas* iCanvas);
  ~ShaderMaterialStarNestScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  EffectShadersStore _effectShadersStore;
  float _time;
  ShaderMaterial* _shaderMaterial;

}; // end of class ShaderMaterialStarNestScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_STAR_NEST_SCENE_H
