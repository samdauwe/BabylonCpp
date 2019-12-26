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
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialMengerSpongeScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Attributes
attribute vec3 position;
attribute vec2 uv;

// Uniforms
uniform mat4 worldViewProjection;

// Varying
varying vec2 vUV;

void main(void) {
    gl_Position = worldViewProjection * vec4(position, 1.0);
    vUV = uv;
}
)ShaderCode";

  /** Pixel (Fragment) Shader **/
  // Menger Sponge ( https://www.shadertoy.com/view/4sX3Rn )
  static constexpr const char* customFragmentShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Varying
varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

// Uniforms
uniform mat4 worldViewProjection;
uniform float iTime;
uniform float aspectRatio;

// The MIT License
// Copyright © 2013 Inigo Quilez

// http://www.iquilezles.org/www/articles/menger/menger.htm

float maxcomp(in vec3 p) {
  return max(p.x, max(p.y, p.z));
}

float sdBox(vec3 p, vec3 b) {
  vec3 di = abs(p) - b;
  float mc = maxcomp(di);
  return min(mc, length(max(di, 0.0)));
}

const mat3 ma = mat3( 0.60, 0.00, 0.80,
                      0.00, 1.00, 0.00,
                     -0.80, 0.00, 0.60 );

vec4 map(in vec3 p) {
  float d = sdBox(p, vec3(1.0));
  vec4 res = vec4(d, 1.0, 0.0, 0.0);

  float ani = smoothstep(-0.2, 0.2, -cos(0.5 * iTime));
  float off = 1.5 * sin(0.01 * iTime);

  float s = 1.0;
  for (int m = 0; m < 4; m++) {
    p = mix(p, ma * (p + off), ani);

    vec3 a = mod(p * s, 2.0) - 1.0;
    s *= 3.0;
    vec3 r = abs(1.0 - 3.0 * abs(a));
    float da = max(r.x, r.y);
    float db = max(r.y, r.z);
    float dc = max(r.z, r.x);
    float c = (min(da, min(db, dc)) - 1.0) / s;

    if (c > d) {
      d = c;
      res =
          vec4(d, min(res.y, 0.2 * da * db * dc),
                      (1.0 + float(m)) / 4.0,
               0.0);
    }
  }

  return res;
}

vec4 intersect(in vec3 ro, in vec3 rd) {
  float t = 0.0;
  vec4 res = vec4(-1.0);
  vec4 h = vec4(1.0);
  for (int i = 0; i < 64; i++) {
    if (h.x < 0.002 || t > 10.0)
      break;
    h = map(ro + rd * t);
    res = vec4(t, h.yzw);
    t += h.x;
  }
  if (t > 10.0)
    res = vec4(-1.0);
  return res;
}

float softshadow(in vec3 ro, in vec3 rd, float mint, float k) {
  float res = 1.0;
  float t = mint;
  float h = 1.0;
  for (int i = 0; i < 32; i++) {
    h = map(ro + rd * t).x;
    res = min(res, k * h / t);
    t += clamp(h, 0.005, 0.1);
  }
  return clamp(res, 0.0, 1.0);
}

vec3 calcNormal(in vec3 pos) {
  vec3 eps = vec3(.001, 0.0, 0.0);
  vec3 nor;
  nor.x = map(pos + eps.xyy).x - map(pos - eps.xyy).x;
  nor.y = map(pos + eps.yxy).x - map(pos - eps.yxy).x;
  nor.z = map(pos + eps.yyx).x - map(pos - eps.yyx).x;
  return normalize(nor);
}

// light
vec3 light = normalize(vec3(1.0, 0.9, 0.3));

vec3 render(in vec3 ro, in vec3 rd) {
  // background color
  vec3 col =
      mix(vec3(0.3, 0.2, 0.1) * 0.5,
          vec3(0.7, 0.9, 1.0),
          0.5 + 0.5 * rd.y);

  vec4 tmat = intersect(ro, rd);
  if (tmat.x > 0.0) {
    vec3 pos = ro + tmat.x * rd;
    vec3 nor = calcNormal(pos);

    float occ = tmat.y;
    float sha = softshadow(pos, light, 0.01, 64.0);

    float dif = max(0.1 + 0.9 * dot(nor, light), 0.0);
    float sky = 0.5 + 0.5 * nor.y;
    float bac =
        max(0.4 + 0.6 * dot(nor, vec3(-light.x, light.y, -light.z)),
            0.0);

    vec3 lin = vec3(0.0);
    lin += 1.00 * dif * vec3(1.10, 0.85, 0.60) * sha;
    lin += 0.50 * sky * vec3(0.10, 0.20, 0.40) * occ;
    lin += 0.10 * bac * vec3(1.00, 1.00, 1.00) * (0.5 + 0.5 * occ);
    lin += 0.25 * occ * vec3(0.15, 0.17, 0.20);

    vec3 matcol = vec3(0.5 + 0.5 * cos(0.0 + 2.0 * tmat.z),
                       0.5 + 0.5 * cos(1.0 + 2.0 * tmat.z),
                       0.5 + 0.5 * cos(2.0 + 2.0 * tmat.z));
    col = matcol * lin;
  }

  return pow(col, vec3(0.4545));
}

void main(void) {
  vec2 p = -1.5 + 3.0 * vUV.xy;
  p.x *= aspectRatio;

  float ctime = iTime;
  // camera
  vec3 ro = 1.1 * vec3(2.5 * sin(0.25 * ctime),
                       1.0 + 1.0 * cos(ctime * .13),
                       2.5 * cos(0.25 * ctime));
  vec3 ww = normalize(vec3(0.0) - ro);
  vec3 uu = normalize(cross(vec3(0.0, 1.0, 0.0), ww));
  vec3 vv = normalize(cross(ww, uu));
  vec3 rd = normalize(p.x * uu + p.y * vv + 2.5 * ww);

  vec3 col = render(ro, rd);

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

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

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialMengerSpongeScene)

} // end of namespace Samples
} // end of namespace BABYLON