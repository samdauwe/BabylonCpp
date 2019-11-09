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

class ShaderMaterialKifsScene : public IRenderableScene {

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
  // KIFS ( https://www.shadertoy.com/view/MdlSRM )
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


// Try KIFS...

float stime, ctime;
void ry(inout vec3 p, float a) {
  float c, s;
  vec3 q = p;
  c = cos(a);
  s = sin(a);
  p.x = c * q.x + s * q.z;
  p.z = -s * q.x + c * q.z;
}
void rx(inout vec3 p, float a) {
  float c, s;
  vec3 q = p;
  c = cos(a);
  s = sin(a);
  p.y = c * q.y - s * q.z;
  p.z = s * q.y + c * q.z;
}

void rz(inout vec3 p, float a) {
  float c, s;
  vec3 q = p;
  c = cos(a);
  s = sin(a);
  p.x = c * q.x - s * q.y;
  p.y = s * q.x + c * q.y;
}
float plane(vec3 p, float y) { return length(vec3(p.x, y, p.z) - p); }

vec3 mat = vec3(0.0);
bool bcolor = false;

float menger_spone(in vec3 z0) {
  vec4 z = vec4(z0, 1.0);
  vec3 offset = vec3(0.785, 1.1, 0.46);
  // vec3 offset=vec3(1.0);
  float scale = 2.48;
  for (int n = 0; n < 4; n++) {
    z = abs(z);
    if (z.x < z.y)
      z.xy = z.yx;
    if (z.x < z.z)
      z.xz = z.zx;
    if (z.y < z.z)
      z.yz = z.zy;
    z = z * scale;
    z.xyz -= offset * (scale - 1.0);
    if (bcolor && n == 2)
      mat += vec3(0.5) + sin(z.xyz) * vec3(1.0, 0.24, 0.245);
    // translate the middle cube to top right corner
    if (z.z < -0.5 * offset.z * (scale - 1.0))
      z.z += offset.z * (scale - 1.0);
  }
  return (length(max(abs(z.xyz) - vec3(1.0), 0.0)) - 0.05) / z.w;
}

vec3 f(vec3 p) {
  ry(p, stime);
  float d1 = plane(p, -1.0);
  float d2 = menger_spone(p);
  if (d1 < d2) {
    return vec3(d1, 0.0, 0.0);
  } else {
    return vec3(d2, 1.0, 0.0);
  }
}

float ao(vec3 p, vec3 n) {
  float ao = 0.0, sca = 1.0;
  for (float i = 0.0; i < 20.0; ++i) {
    float hr = 0.05 + 0.015 * i * i;
    ao += (hr - f(n * hr + p).x) * sca;
    sca *= 0.75;
  }
  return 1.0 - clamp(ao, 0.0, 1.0);
}

float rand(vec2 t) {
  return fract(sin(dot(t * 0.123, vec2(12.9898, 78.233))) *
               43758.5453);
}
float softshadow(vec3 ro, vec3 rd, float k) {
  float akuma = 1.0, h = 0.0;
  float t = 0.01;
  for (int i = 0; i < 50; ++i) {
    h = f(ro + rd * t).x;
    if (h < 0.001) {
      akuma = 0.0;
      break;
    }
    akuma = min(akuma, k * h / t);
    t += abs(h);
  }
  return clamp(akuma * 0.8 + 0.2, 0.0, 1.0);
}
vec3 nor(vec3 p) {
  vec3 e = vec3(0.0001, 0.0, 0.0);
  return normalize(vec3(f(p + e.xyy).x - f(p - e.xyy).x,
                        f(p + e.yxy).x - f(p - e.yxy).x,
                        f(p + e.yyx).x - f(p - e.yyx).x));
}

vec3 intersect(in vec3 ro, in vec3 rd) {
  float t = 0.0;
  vec3 res = vec3(-1.0);
  vec3 h = vec3(1.0);
  for (int i = 0; i < 64; i++) {
    if (h.x < 0.003 || t > 30.0) {
    } else {
      h = f(ro + rd * t);
      res = vec3(t, h.yz);
      t += abs(h.x);
    }
  }
  if (t > 30.0)
    res = vec3(-1.0);
  return res;
}

void main(void) {
  vec2 q = vUV.xy;
  vec2 uv = -1.0 + 2.0 * q;
  uv.x *= iResolution.x / iResolution.y;
  // camera
  stime = sin(iTime * 0.2);
  ctime = cos(iTime * 0.2);

  vec3 ta = vec3(.0, 0.0, 0.0);
  // vec3 ro=vec3(3.0*stime,stime*2.0+1.0, 3.0*ctime);
  vec3 ro = vec3(3.5 * ctime, 2.2 + 0.5 * ctime, 5.8 + 1.0 * stime) *
                 0.5;

  vec3 cf = normalize(ta - ro);
  vec3 cs = normalize(cross(cf, vec3(0.0, 1.0, 0.0)));
  vec3 cu = normalize(cross(cs, cf));
  vec3 rd = normalize(uv.x * cs + uv.y * cu +
                      2.8 * cf); // transform from view to world

  vec3 sundir = normalize(vec3(-3.5, 7.0, 2.8));
  vec3 sun = vec3(1.64, 1.27, 0.99);
  vec3 skycolor = vec3(0.6, 1.5, 1.0);

  vec3 bg = exp(uv.y - 1.0) * vec3(0.6, 0.9, 1.4) * 1.3;

  float sc = clamp(dot(sundir, rd), 0.0, 1.0);
  vec3 col = bg + vec3(1.0, 0.8, 0.4) * pow(sc, 50.0);

  float t = 0.0;
  vec3 p = ro;

  vec3 res = intersect(ro, rd);
  if (res.x > 0.0) {
    p = ro + res.x * rd;
    bcolor = true;
    mat = vec3(0.0);
    vec3 n = nor(p);
    mat /= 6.0;
    bcolor = false;
    float occ = ao(p, n);
    float shadow = softshadow(p, sundir, 10.0);

    float dif = max(0.0, dot(n, sundir));
    float sky = 0.6 + 0.4 * max(0.0, dot(n, vec3(0.0, 1.0, 0.0)));
    float bac = max(0.3 + 0.7 *
                    dot(vec3(-sundir.x, -1.0, -sundir.z), n), 0.0);
    float spe =
        max(0.0,
            pow(clamp(dot(sundir, reflect(rd, n)), 0.0, 1.0), 10.0));

    vec3 lin = 5.5 * sun * dif * shadow;
    lin += 0.8 * bac * sun * occ;
    lin += 0.6 * sky * skycolor * occ;
    lin += 3.0 * spe * occ;

    col = lin * (vec3(0.9, 0.8, 0.6) * (1.0 - res.y) + mat * res.y) *
                 0.2;
    col = mix(col, bg, 1.0 - exp(-0.02 * res.x * res.x));
  }

  // post
  col = pow(clamp(col, 0.0, 1.0), vec3(0.45));
  col = col * 0.6 + 0.4 * col * col * (3.0 - 2.0 * col); // contrast
  col = mix(col, vec3(dot(col, vec3(0.33))), -0.5);      // satuation
  col *=
      0.5 +
      // vigneting
      0.5 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.7);
  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

public:
  ShaderMaterialKifsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialKifsScene() override = default;

  const char* getName() override
  {
    return "Shader Material KIFS Scene";
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

}; // end of class ShaderMaterialKIFSScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialKifsScene)

} // end of namespace Samples
} // end of namespace BABYLON