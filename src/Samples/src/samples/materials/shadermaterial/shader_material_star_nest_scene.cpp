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

class ShaderMaterialStarNestScene : public IRenderableScene {

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
  // Star Nest ( https://www.shadertoy.com/view/XlfGRj )
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
uniform vec2 iResolution;

// Star Nest by Pablo RomÃ¡n Andrioli

// This content is under the MIT License.

#define iterations 17
#define formuparam 0.53

#define volsteps 20
#define stepsize 0.1

#define zoom 0.800
#define tile 0.850
#define speed 0.010

#define brightness 0.0015
#define darkmatter 0.300
#define distfading 0.730
#define saturation 0.850

void main(void) {
  // get coords and direction
  vec2 uv = -1.0 + 2.0 * vUV.xy;
  uv.y *= iResolution.y / iResolution.x;
  vec3 dir = vec3(uv * zoom, 1.);
  vec2 iMouse = vec2(0.0, 0.0);
  float time = iTime * speed + .25;

  // mouse rotation
  float a1 = .5 + iMouse.x / iResolution.x * 2.;
  float a2 = .8 + iMouse.y / iResolution.y * 2.;
  mat2 rot1 = mat2(cos(a1), sin(a1), -sin(a1), cos(a1));
  mat2 rot2 = mat2(cos(a2), sin(a2), -sin(a2), cos(a2));
  dir.xz *= rot1;
  dir.xy *= rot2;
  vec3 from = vec3(1., .5, 0.5);
  from += vec3(time * 2., time, -2.);
  from.xz *= rot1;
  from.xy *= rot2;

  // volumetric rendering
  float s = 0.1, fade = 1.;
  vec3 v = vec3(0.);
  for (int r = 0; r < volsteps; r++) {
    vec3 p = from + s * dir * .5;
    // tiling fold
    p = abs(vec3(tile) - mod(p, vec3(tile * 2.)));
    float pa, a = pa = 0.;
    for (int i = 0; i < iterations; i++) {
      // the magic formula
      p = abs(p) / dot(p, p) - formuparam;
      // absolute sum of average change
      a += abs(length(p) - pa);
      pa = length(p);
    }
    // dark matter
    float dm = max(0., darkmatter - a * a * .001);
    // add contrast
    a *= a * a;
    if (r > 6) {
      // dark matter, don't render near
      fade *= 1. - dm;
    }
    // v+=vec3(dm,dm*.5,0.);
    v += fade;
    // coloring based on distance
    v += vec3(s, s * s, s * s * s * s) * a * brightness * fade;
    // distance fading
    fade *= distfading;
    s += stepsize;
  }
  v = mix(vec3(length(v)), v, saturation); // color adjust
  gl_FragColor = vec4(v * .01, 1.);
}
)ShaderCode";

public:
  ShaderMaterialStarNestScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialStarNestScene() override = default;

  const char* getName() override
  {
    return "Shader Material Star Nest Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera, and set its position to (x:0, y:0, z:-10)
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
      = {"iTime", "worldViewProjection", "iAspectRatio", "iResolution"};
    _shaderMaterial = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

    // box + sky = skybox !
    skybox->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      const Vector2 resolution{static_cast<float>(_engine->getRenderWidth()),
                               static_cast<float>(_engine->getRenderHeight())};
      const float aspectRatio = resolution.x / resolution.y;
      _shaderMaterial->setFloat("iTime", _time);
      _shaderMaterial->setFloat("iAspectRatio", aspectRatio);
      _shaderMaterial->setVector2("iResolution", resolution);
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialStarNestScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialStarNestScene)

} // end of namespace Samples
} // end of namespace BABYLON