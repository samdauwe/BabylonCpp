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

class ShaderMaterialSkyboxCloudsScene : public IRenderableScene {

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
  // 2D Clouds ( https://www.shadertoy.com/view/4tdSWr )
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
uniform float time;

const float cloudscale = 1.1;
const float speed = 0.03;
const float clouddark = 0.5;
const float cloudlight = 0.3;
const float cloudcover = 0.2;
const float cloudalpha = 8.0;
const float skytint = 0.5;
const vec3 skycolour1 = vec3(0.2, 0.4, 0.6);
const vec3 skycolour2 = vec3(0.4, 0.7, 1.0);

const mat2 m = mat2(1.6, 1.2, -1.2, 1.6);

vec2 hash(vec2 p) {
  p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
  return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(in vec2 p) {
  const float K1 = 0.366025404; // (sqrt(3)-1)/2;
  const float K2 = 0.211324865; // (3-sqrt(3))/6;
  vec2 i = floor(p + (p.x + p.y) * K1);
  vec2 a = p - i + (i.x + i.y) * K2;
  vec2 o =
      (a.x > a.y)
          ? vec2(1.0, 0.0)
          : vec2(0.0, 1.0);
  vec2 b = a - o + K2;
  vec2 c = a - 1.0 + 2.0 * K2;
  vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
  vec3 n = h * h * h * h * vec3(dot(a, hash(i + 0.0)),
                                dot(b, hash(i + o)),
                                dot(c, hash(i + 1.0)));
  return dot(n, vec3(70.0));
}
float fbm(vec2 n) {
  float total = 0.0, amplitude = 0.1;
  for (int i = 0; i < 7; i++) {
    total += noise(n) * amplitude;
    n = m * n;
    amplitude *= 0.4;
  }
  return total;
}

// -------------------------------------------------------------------

void main(void) {
  vec2 p = -1.0 + 2.0 * vUV.xy;
  vec2 uv = vUV.xy;
  float iTime = time * speed;
  float q = fbm(uv * cloudscale * 0.5);

  // ridged noise shape
  float r = 0.0;
  uv *= cloudscale;
  uv -= q - iTime;
  float weight = 0.8;
  for (int i = 0; i < 8; i++) {
    r += abs(weight * noise(uv));
    uv = m * uv + iTime;
    weight *= 0.7;
  }

  // noise shape
  float f = 0.0;
  uv = vUV.xy;
  uv *= cloudscale;
  uv -= q - iTime;
  weight = 0.7;
  for (int i = 0; i < 8; i++) {
    f += weight * noise(uv);
    uv = m * uv + iTime;
    weight *= 0.6;
  }

  f *= r + f;

  // noise colour
  float c = 0.0;
  iTime = time * speed * 2.0;
  uv = vUV.xy;
  uv *= cloudscale * 2.0;
  uv -= q - iTime;
  weight = 0.4;
  for (int i = 0; i < 7; i++) {
    c += weight * noise(uv);
    uv = m * uv + iTime;
    weight *= 0.6;
  }

  // noise ridge colour
  float c1 = 0.0;
  iTime = time * speed * 3.0;
  uv = vUV.xy;
  uv *= cloudscale * 3.0;
  uv -= q - iTime;
  weight = 0.4;
  for (int i = 0; i < 7; i++) {
    c1 += abs(weight * noise(uv));
    uv = m * uv + iTime;
    weight *= 0.6;
  }

  c += c1;

  vec3 skycolour = mix(skycolour2, skycolour1, p.y);
  vec3 cloudcolour =
      vec3(1.1, 1.1, 0.9) * clamp((clouddark + cloudlight * c),
           0.0, 1.0);

  f = cloudcover + cloudalpha * f * r;

  vec3 result =
      mix(skycolour, clamp(skytint * skycolour + cloudcolour,
          0.0, 1.0), clamp(f + c, 0.0, 1.0));

  gl_FragColor = vec4(result, 1.0);
}
)ShaderCode";

public:
  ShaderMaterialSkyboxCloudsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialSkyboxCloudsScene() override = default;

  const char* getName() override
  {
    return "Shader Material Skybox Clouds Scene";
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
    shaderMaterialOptions.uniforms   = {"worldViewProjection", "time"};
    _shaderMaterial = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

    // box + sky = skybox !
    skybox->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      _shaderMaterial->setFloat("time", _time);
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialSkyboxCloudsScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialSkyboxCloudsScene)

} // end of namespace Samples
} // end of namespace BABYLON