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

class ShaderMaterialServerRoomScene : public IRenderableScene {

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
  // Server Room ( https://www.shadertoy.com/view/MdySzc )
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
uniform float iAspectRatio;
uniform vec2 iMouse;
uniform vec2 iResolution;

const float tmax = 20.0;

float hash(float n) { return fract(sin(n) * 43758.5453); }

vec3 hash(vec3 p) {

  float n = sin(dot(p, vec3(7, 157, 113)));
  return fract(vec3(2097152, 262144, 32768) * n);
}

float noise(float g) {
  float p = floor(g);
  float f = fract(g);

  return mix(hash(p), hash(p + 1.0), f);
}

float voronoi(vec3 x) {
  vec3 p = floor(x);
  vec3 f = fract(x);

  vec2 res = vec2(8.0);

  for (int i = -1; i <= 1; i++)
    for (int j = -1; j <= 1; j++)
      for (int k = -1; k <= 1; k++) {
        vec3 g = vec3(float(i), float(j), float(k));
        vec3 r = g + hash(p + g) - f;

        float d = max(abs(r.x), max(abs(r.y), abs(r.z)));

        if (d < res.x) {
          res.y = res.x;
          res.x = d;
        } else if (d < res.y) {
          res.y = d;
        }
      }

  return res.y - res.x;
}

vec2 path(float z) {
  return vec2(cos(z / 8.0) * sin(z / 12.0) * 12.0, 0.0);
}

float map(vec3 p) {
  vec4 q = vec4(p, 1.0);
  q.x += 1.0;

  for (int i = 0; i < 6; i++) {
    q.xyz = -1.0 + 2.0 * fract(0.5 + 0.5 * q.xyz);
    q = 1.2 * q / max(dot(q.xyz, q.xyz), 0.1);
  }

  vec2 tun = abs(p.xy - path(p.z)) * vec2(0.6, 0.5);

  return min(0.25 * abs(q.y) / q.w, 1.0 - max(tun.x, tun.y));
}

float march(vec3 ro, vec3 rd, float mx) {
  float t = 0.0;

  for (int i = 0; i < 200; i++) {
    float d = map(ro + rd * t);
    if (d < 0.001 || t >= mx)
      break;
    t += d * 0.75;
  }

  return t;
}

vec3 normal(vec3 p) {
  vec2 h = vec2(0.001, 0.0);
  vec3 n =
      vec3(map(p + h.xyy) - map(p - h.xyy),
           map(p + h.yxy) - map(p - h.yxy),
           map(p + h.yyx) - map(p - h.yyx));
  return normalize(n);
}

float ao(vec3 p, vec3 n) {
  float o = 0.0, s = 0.005;
  for (int i = 0; i < 15; i++) {
    float d = map(p + n * s);
    o += (s - d);
    s += s / float(i + 1);
  }

  return 1.0 - clamp(o, 0.0, 1.0);
}

vec3 material(vec3 p) {
  float v = 0.0;
  float a = 0.7, f = 1.0;

  for (int i = 0; i < 4; i++) {
    float v1 = voronoi(p * f + 5.0);
    float v2 = 0.0;

    if (i > 0) {
      v2 = voronoi(p * f * 0.1 + 50.0 + 0.15 * iTime);

      float va = 0.0, vb = 0.0;
      va = 1.0 - smoothstep(0.0, 0.1, v1);
      vb = 1.0 - smoothstep(0.0, 0.08, v2);
      v += a * pow(va * (0.5 + vb), 4.0);
    }

    v1 = 1.0 - smoothstep(0.0, 0.3, v1);
    v2 = a * noise(v1 * 5.5 + 0.1);

    v += v2;

    f *= 3.0;
    a *= 0.5;
  }

  return vec3(pow(v, 6.0), pow(v, 4.0), pow(v, 2.0)) * 2.0;
}

mat3 camera(vec3 eye, vec3 lat) {
  vec3 ww = normalize(lat - eye);
  vec3 uu = normalize(cross(vec3(0, 1, 0), ww));
  vec3 vv = normalize(cross(ww, uu));

  return mat3(uu, vv, ww);
}

void main(void) {
  vec2 uv = (-1.0 + 2.0 * vUV.xy);
  uv.x *= iResolution.x / iResolution.y;

  vec3 col = vec3(0);

  vec3 ro = vec3(0.63 * cos(iTime * 0.1), 0.67, iTime * 0.5);
  vec3 la = ro + vec3(0, 0, 0.3);

  ro.xy += path(ro.z);
  la.xy += path(la.z);
  vec3 rd = normalize(camera(ro, la) * vec3(uv, 1.97));

  float i = march(ro, rd, tmax);
  if (i < tmax) {
    vec3 pos = ro + rd * i;
    vec3 nor = normal(pos);
    vec3 ref = normalize(reflect(rd, nor));

    float occ = ao(pos, nor);

    col = material(pos) * occ;
    col += 2.0 * pow(clamp(dot(-rd, ref), 0.0, 1.0), 16.0) * occ;
  }

  col = mix(col, vec3(0), 1.0 - exp(-0.5 * i));

  col = 1.0 - exp(-0.5 * col);
  col = pow(abs(col), vec3(1.0 / 2.2));

  gl_FragColor = vec4(col, 1);
}
)ShaderCode";

public:
  ShaderMaterialServerRoomScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialServerRoomScene() override = default;

  const char* getName() override
  {
    return "Shader Material Server Room Scene";
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
    shaderMaterialOptions.uniforms
      = {"iTime", "worldViewProjection", "iAspectRatio", "iResolution", "iMouse"};
    _shaderMaterial = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

    // box + sky = skybox !
    skybox->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      const Vector2 resolution{static_cast<float>(_engine->getRenderWidth()),
                               static_cast<float>(_engine->getRenderHeight())};
      const Vector2 mouse{0.f, 0.f};
      const float aspectRatio = resolution.x / resolution.y;
      _shaderMaterial->setFloat("iTime", _time);
      _shaderMaterial->setFloat("iAspectRatio", aspectRatio);
      _shaderMaterial->setVector2("iResolution", resolution);
      _shaderMaterial->setVector2("iMouse", mouse);
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialServerRoomScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialServerRoomScene)

} // end of namespace Samples
} // end of namespace BABYLON