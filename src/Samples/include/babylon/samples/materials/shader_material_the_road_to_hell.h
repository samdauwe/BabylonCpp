#ifndef BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_THE_ROAD_TO_HELL_SCENE_H
#define BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_THE_ROAD_TO_HELL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/effect_shaders_store.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialTheRoadToHellScene : public IRenderableScene {

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
  // The road to Hell ( https://www.shadertoy.com/view/Mds3Rn )
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
      "const float PI = 3.14159265358979323846;\n"
      "\n"
      "#define speed (time * 0.2975)\n"
      "#define ground_x (1.0 - 0.325 * sin(PI * speed * 0.25))\n"
      "\n"
      "float ground_y = 1.0;\n"
      "float ground_z = 0.5;\n"
      "\n"
      "vec2 rotate(vec2 k, float t) {\n"
      "  return vec2(cos(t) * k.x - sin(t) * k.y,\n"
      "              sin(t) * k.x + cos(t) * k.y);\n"
      "}\n"
      "\n"
      "float draw_scene(vec3 p) {\n"
      "  float tunnel_m = 0.125 * cos(PI * p.z * 1.0 + speed * 4.0 - PI);\n"
      "  float tunnel1_p = 2.0;\n"
      "  float tunnel1_w = tunnel1_p * 0.225;\n"
      "  // tunnel1\n"
      "  float tunnel1 =\n"
      "      length(mod(p.xy, tunnel1_p) - tunnel1_p * 0.5) - tunnel1_w;"
      "  float tunnel2_p = 2.0;\n"
      "  float tunnel2_w = tunnel2_p * 0.2125 +\n"
      "                    tunnel2_p * 0.0125 * cos(PI * p.y * 8.0) +\n"
      "                    tunnel2_p * 0.0125 * cos(PI * p.z * 8.0);\n"
      "  // tunnel2\n"
      "  float tunnel2 =\n"
      "      length(mod(p.xy, tunnel2_p) - tunnel2_p * 0.5) - tunnel2_w;"
      "  float hole1_p = 1.0;\n"
      "  float hole1_w = hole1_p * 0.5;\n"
      "  // hole1\n"
      "  float hole1 =\n"
      "      length(mod(p.xz, hole1_p).xy - hole1_p * 0.5) - hole1_w;\n"
      "  float hole2_p = 0.25;\n"
      "  float hole2_w = hole2_p * 0.375;\n"
      "  // hole2\n"
      "  float hole2 =\n"
      "      length(mod(p.yz, hole2_p).xy - hole2_p * 0.5) - hole2_w;\n"
      "  float hole3_p = 0.5;\n"
      "  float hole3_w = hole3_p * 0.25 + 0.125 * sin(PI * p.z * 2.0);\n"
      "  // hole3\n"
      "  float hole3 =\n"
      "      length(mod(p.xy, hole3_p).xy - hole3_p * 0.5) - hole3_w;\n"
      "  float tube_m = 0.075 * sin(PI * p.z * 1.0);\n"
      "  float tube_p = 0.5 + tube_m;\n"
      "  float tube_w = tube_p * 0.025 + 0.00125 * cos(PI * p.z * 128.0);\n"
      "  // tube\n"
      "  float tube = length(mod(p.xy, tube_p) - tube_p * 0.5) - tube_w;\n"
      "  float bubble_p = 0.05;\n"
      "  float bubble_w = bubble_p * 0.5 + 0.025 * cos(PI * p.z * 2.0);\n"
      "  // bubble\n"
      "  float bubble =\n"
      "      length(mod(p.yz, bubble_p) - bubble_p * 0.5) - bubble_w;\n"
      "  return max(min(min(-tunnel1, mix(tunnel2, -bubble, 0.375)),\n"
      "                 max(min(-hole1, hole2), -hole3)),\n"
      "             -tube);\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 position = -1.0 + 2.0 * vUV.xy;\n"
      "  vec2 p = -1.0 + 2.0 * position;\n"
      "  // screen ratio (x,y) fov (z)\n"
      "  vec3 dir =\n"
      "      normalize(vec3(p * vec2(1.77, 1.0), 1.0));\n"
      "  // dir.yz=rotate(dir.yz,PI*0.5*sin(PI*speed*0.125)); // rotation xv\n"
      "  dir.zx = rotate(dir.zx, -PI * speed * 0.25); // rotation y\n"
      "  dir.xy = rotate(dir.xy, -speed * 0.5);       // rotation z\n"
      "  vec3 ray = vec3(ground_x, ground_y, ground_z - speed * 2.5);\n"
      "  float t = 0.0;\n"
      "  const int ray_n = 96;\n"
      "  for (int i = 0; i < ray_n; i++) {\n"
      "    float k = draw_scene(ray + dir * t);\n"
      "    t += k * 0.75;\n"
      "  }\n"
      "  vec3 hit = ray + dir * t;\n"
      "  vec2 h = vec2(-0.0025, 0.002); // light\n"
      "  vec3 n = normalize(vec3(draw_scene(hit + h.xyx),\n"
      "                          draw_scene(hit + h.yxy),\n"
      "                          draw_scene(hit + h.yyx)));\n"
      "  float c = (n.x + n.y + n.z) * 0.35;\n"
      "  vec3 color = vec3(c, c, c) + t * 0.0625;\n"
      "  gl_FragColor =\n"
      "      vec4(vec3(c - t * 0.0375 + p.y * 0.05,\n"
      "                c - t * 0.025 - p.y * 0.0625,\n"
      "                c + t * 0.025 - p.y * 0.025) +\n"
      "               color * color,\n"
      "           1.0);\n"
      "}\n";

public:
  ShaderMaterialTheRoadToHellScene(ICanvas* iCanvas);
  ~ShaderMaterialTheRoadToHellScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  EffectShadersStore _effectShadersStore;
  float _time;
  ShaderMaterial* _shaderMaterial;

}; // end of class ShaderMaterialTheRoadToHellScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_MATERIALS_SHADER_MATERIAL_THE_ROAD_TO_HELL_SCENE_H
