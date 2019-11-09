#include <babylon/samples/samples_index.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ShaderMaterial;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;

namespace Samples {

class ShaderMaterialSimplicityScene : public IRenderableScene {

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
  // Simplicity ( https://www.shadertoy.com/view/lslGWr )
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

// 
// http://www.fractalforums.com/new-theories-and-research/
// very-simple-formula-for-fractal-patterns/
float field(in vec3 p) {
  float strength = 7. + .03 * log(1.e-6 + fract(sin(iTime) *
                                  4373.11));
  float accum = 0.;
  float prev = 0.;
  float tw = 0.;
  for (int i = 0; i < 32; ++i) {
    float mag = dot(p, p);
    p = abs(p) / mag + vec3(-.5, -.4, -1.5);
    float w = exp(-float(i) / 7.);
    accum += w * exp(-strength * pow(abs(mag - prev), 2.3));
    tw += w;
    prev = mag;
  }
  return max(0., 5. * accum / tw - .7);
}

void main(void) {
  vec2 uv = -1.0 + 2.0 * vUV.xy;
  vec2 uvs = uv * iResolution.xy / max(iResolution.x, iResolution.y);
  vec3 p = vec3(uvs / 4., 0) + vec3(1., -1.3, 0.);
  p += .2 * vec3(sin(iTime / 16.), sin(iTime / 12.),
                 sin(iTime / 128.));
  float t = field(p);
  float v =
      (1. - exp((abs(uv.x) - 1.) * 6.)) *
      (1. - exp((abs(uv.y) - 1.) * 6.));
  gl_FragColor = mix(.4, 1., v) *
                 vec4(1.8 * t * t * t, 1.4 * t * t, t, 1.0);
}
)ShaderCode";

public:
  ShaderMaterialSimplicityScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;
  
    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }
  ~ShaderMaterialSimplicityScene()
  {
  }

  const char* getName() override
  {
    return "Shader Material Simplicity Scene";
  }
  void initializeScene(ICanvas* canvas,
                                                      Scene* scene) override
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
      = {"iTime", "worldViewProjection", "iAspectRatio", "iResolution"};
    _shaderMaterial
      = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);
  
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

}; // end of class ShaderMaterialSimplicityScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialSimplicityScene)
} // end of namespace Samples
} // end of namespace BABYLON