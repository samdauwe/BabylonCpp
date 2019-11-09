#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialMengerSpongeScene : public IRenderableScene {

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
  // Menger Sponge ( https://www.shadertoy.com/view/4sX3Rn )
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
      "uniform float aspectRatio;\n"
      "\n"
      "// The MIT License\n"
      "// Copyright © 2013 Inigo Quilez\n"
      "\n"
      "// http://www.iquilezles.org/www/articles/menger/menger.htm\n"
      "\n"
      "float maxcomp(in vec3 p) {\n"
      "  return max(p.x, max(p.y, p.z));\n"
      "}\n"
      "\n"
      "float sdBox(vec3 p, vec3 b) {\n"
      "  vec3 di = abs(p) - b;\n"
      "  float mc = maxcomp(di);\n"
      "  return min(mc, length(max(di, 0.0)));\n"
      "}\n"
      "\n"
      "const mat3 ma = mat3( 0.60, 0.00, 0.80,\n"
      "                      0.00, 1.00, 0.00,\n"
      "                     -0.80, 0.00, 0.60 );\n"
      "\n"
      "vec4 map(in vec3 p) {\n"
      "  float d = sdBox(p, vec3(1.0));\n"
      "  vec4 res = vec4(d, 1.0, 0.0, 0.0);\n"
      "\n"
      "  float ani = smoothstep(-0.2, 0.2, -cos(0.5 * iTime));\n"
      "  float off = 1.5 * sin(0.01 * iTime);\n"
      "\n"
      "  float s = 1.0;\n"
      "  for (int m = 0; m < 4; m++) {\n"
      "    p = mix(p, ma * (p + off), ani);\n"
      "\n"
      "    vec3 a = mod(p * s, 2.0) - 1.0;\n"
      "    s *= 3.0;\n"
      "    vec3 r = abs(1.0 - 3.0 * abs(a));\n"
      "    float da = max(r.x, r.y);\n"
      "    float db = max(r.y, r.z);\n"
      "    float dc = max(r.z, r.x);\n"
      "    float c = (min(da, min(db, dc)) - 1.0) / s;\n"
      "\n"
      "    if (c > d) {\n"
      "      d = c;\n"
      "      res =\n"
      "          vec4(d, min(res.y, 0.2 * da * db * dc),\n"
      "                      (1.0 + float(m)) / 4.0,\n"
      "               0.0);\n"
      "    }\n"
      "  }\n"
      "\n"
      "  return res;\n"
      "}\n"
      "\n"
      "vec4 intersect(in vec3 ro, in vec3 rd) {\n"
      "  float t = 0.0;\n"
      "  vec4 res = vec4(-1.0);\n"
      "  vec4 h = vec4(1.0);\n"
      "  for (int i = 0; i < 64; i++) {\n"
      "    if (h.x < 0.002 || t > 10.0)\n"
      "      break;\n"
      "    h = map(ro + rd * t);\n"
      "    res = vec4(t, h.yzw);\n"
      "    t += h.x;\n"
      "  }\n"
      "  if (t > 10.0)\n"
      "    res = vec4(-1.0);\n"
      "  return res;\n"
      "}\n"
      "\n"
      "float softshadow(in vec3 ro, in vec3 rd, float mint, float k) {\n"
      "  float res = 1.0;\n"
      "  float t = mint;\n"
      "  float h = 1.0;\n"
      "  for (int i = 0; i < 32; i++) {\n"
      "    h = map(ro + rd * t).x;\n"
      "    res = min(res, k * h / t);\n"
      "    t += clamp(h, 0.005, 0.1);\n"
      "  }\n"
      "  return clamp(res, 0.0, 1.0);\n"
      "}\n"
      "\n"
      "vec3 calcNormal(in vec3 pos) {\n"
      "  vec3 eps = vec3(.001, 0.0, 0.0);\n"
      "  vec3 nor;\n"
      "  nor.x = map(pos + eps.xyy).x - map(pos - eps.xyy).x;\n"
      "  nor.y = map(pos + eps.yxy).x - map(pos - eps.yxy).x;\n"
      "  nor.z = map(pos + eps.yyx).x - map(pos - eps.yyx).x;\n"
      "  return normalize(nor);\n"
      "}\n"
      "\n"
      "// light\n"
      "vec3 light = normalize(vec3(1.0, 0.9, 0.3));\n"
      "\n"
      "vec3 render(in vec3 ro, in vec3 rd) {\n"
      "  // background color\n"
      "  vec3 col =\n"
      "      mix(vec3(0.3, 0.2, 0.1) * 0.5,\n"
      "          vec3(0.7, 0.9, 1.0),\n"
      "          0.5 + 0.5 * rd.y);\n"
      "\n"
      "  vec4 tmat = intersect(ro, rd);\n"
      "  if (tmat.x > 0.0) {\n"
      "    vec3 pos = ro + tmat.x * rd;\n"
      "    vec3 nor = calcNormal(pos);\n"
      "\n"
      "    float occ = tmat.y;\n"
      "    float sha = softshadow(pos, light, 0.01, 64.0);\n"
      "\n"
      "    float dif = max(0.1 + 0.9 * dot(nor, light), 0.0);\n"
      "    float sky = 0.5 + 0.5 * nor.y;\n"
      "    float bac =\n"
      "        max(0.4 + 0.6 * dot(nor, vec3(-light.x, light.y, -light.z)),\n"
      "            0.0);\n"
      "\n"
      "    vec3 lin = vec3(0.0);\n"
      "    lin += 1.00 * dif * vec3(1.10, 0.85, 0.60) * sha;\n"
      "    lin += 0.50 * sky * vec3(0.10, 0.20, 0.40) * occ;\n"
      "    lin += 0.10 * bac * vec3(1.00, 1.00, 1.00) * (0.5 + 0.5 * occ);\n"
      "    lin += 0.25 * occ * vec3(0.15, 0.17, 0.20);\n"
      "\n"
      "    vec3 matcol = vec3(0.5 + 0.5 * cos(0.0 + 2.0 * tmat.z),\n"
      "                       0.5 + 0.5 * cos(1.0 + 2.0 * tmat.z),\n"
      "                       0.5 + 0.5 * cos(2.0 + 2.0 * tmat.z));\n"
      "    col = matcol * lin;\n"
      "  }\n"
      "\n"
      "  return pow(col, vec3(0.4545));\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 p = -1.5 + 3.0 * vUV.xy;\n"
      "  p.x *= aspectRatio;\n"
      "\n"
      "  float ctime = iTime;\n"
      "  // camera\n"
      "  vec3 ro = 1.1 * vec3(2.5 * sin(0.25 * ctime),\n"
      "                       1.0 + 1.0 * cos(ctime * .13),\n"
      "                       2.5 * cos(0.25 * ctime));\n"
      "  vec3 ww = normalize(vec3(0.0) - ro);\n"
      "  vec3 uu = normalize(cross(vec3(0.0, 1.0, 0.0), ww));\n"
      "  vec3 vv = normalize(cross(ww, uu));\n"
      "  vec3 rd = normalize(p.x * uu + p.y * vv + 2.5 * ww);\n"
      "\n"
      "  vec3 col = render(ro, rd);\n"
      "\n"
      "  gl_FragColor = vec4(col, 1.0);\n"
      "}\n";

public:
  ShaderMaterialMengerSpongeScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialMengerSpongeScene() override = default;

  const char* getName() override
  {
    return "Shader Material Menger Sponge Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera, and set its position to (x:0, y:0, z:-8)
    auto camera = FreeCamera::New("camera1", Vector3(0.f, 0.f, -8.f), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

    // Create a built-in "box" shape
    const float ratio = static_cast<float>(getEngine()->getRenderWidth())
                        / static_cast<float>(getEngine()->getRenderHeight());
    BoxOptions options;
    options.size            = 5.f;
    options.sideOrientation = Mesh::DEFAULTSIDE;
    options.updatable       = false;
    options.width           = *options.size * ratio;
    auto skybox             = MeshBuilder::CreateBox("skybox", options, scene);

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "uv"};
    shaderMaterialOptions.uniforms   = {"worldViewProjection", "iTime", "aspectRatio"};
    _shaderMaterial = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

    // box + sky = skybox !
    skybox->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      const float aspectRatio = static_cast<float>(getEngine()->getRenderWidth())
                                / static_cast<float>(getEngine()->getRenderHeight());
      _shaderMaterial->setFloat("aspectRatio", aspectRatio);
      _shaderMaterial->setFloat("iTime", _time);
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialMengerSpongeScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialMengerSpongeScene)

} // end of namespace Samples
} // end of namespace BABYLON
