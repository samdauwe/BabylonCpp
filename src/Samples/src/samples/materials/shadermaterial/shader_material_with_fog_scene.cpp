#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ShaderMaterial;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;

namespace Samples {

class ShaderMaterialWithFogScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Attributes
attribute vec3 position;

// Uniforms
uniform mat4 world;
uniform mat4 view;
uniform mat4 viewProjection;

// Varying
 varying float fFogDistance;

void main(void) {
  vec4 worldPosition = world * vec4(position, 1.0);
  fFogDistance = (view * worldPosition).z;
  gl_Position =  viewProjection * worldPosition;
}
)ShaderCode";

  /** Pixel (Fragment) Shader **/
  static constexpr const char* customFragmentShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

#define FOGMODE_NONE 0.
#define FOGMODE_EXP 1.
#define FOGMODE_EXP2 2.
#define FOGMODE_LINEAR 3.
#define E 2.71828

// Uniforms
uniform vec4 vFogInfos;
uniform vec3 vFogColor;
uniform sampler2D textureSampler; 

// Varying
varying float fFogDistance;

float CalcFogFactor()
{
  float fogCoeff = 1.0;
  float fogStart = vFogInfos.y;
  float fogEnd = vFogInfos.z;
  float fogDensity = vFogInfos.w;

  if (FOGMODE_LINEAR == vFogInfos.x)
  {
    fogCoeff = (fogEnd - fFogDistance) / (fogEnd - fogStart);
  }
  else if (FOGMODE_EXP == vFogInfos.x)
  {
    fogCoeff = 1.0 / pow(E, fFogDistance * fogDensity);
  }
  else if (FOGMODE_EXP2 == vFogInfos.x)
  {
    fogCoeff = 1.0 / pow(E, fFogDistance * fFogDistance * fogDensity * 
                            fogDensity);
  }
  return clamp(fogCoeff, 0.0, 1.0);
}

void main(void) {
  float fog = CalcFogFactor();
  vec3 color = vec3(1.0, 0., 1.0);
  color = fog * color + (1.0 - fog) * vFogColor;
  gl_FragColor = vec4(color, 1.);
}
)ShaderCode";

public:
  ShaderMaterialWithFogScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;
  
    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }
  ~ShaderMaterialWithFogScene()
  {
  }

  const char* getName() override
  {
    return "Shader Material With Fog Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera = ArcRotateCamera::New("camera1", Math::PI, Math::PI_2, 5.f,
                                       Vector3(0.f, 0.f, 0.f), scene);
  
    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());
  
    // Attach the camera to the canvas
    camera->attachControl(canvas, true);
  
    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  
    // Create a built-in "box" shape
    auto box = Mesh::CreateBox("box1", 1.f, scene);
  
    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.needAlphaBlending = true;
    shaderMaterialOptions.attributes        = {"position"};
    shaderMaterialOptions.uniforms
      = {"world", "view", "viewProjection", "vFogInfos", "vFogColor"};
    _boxMaterial
      = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);
    box->material = _boxMaterial;
  
    _boxMaterial->onBind = [this](AbstractMesh*, EventState&) {
      auto effect = _boxMaterial->getEffect();
      effect->setFloat4("vFogInfos", static_cast<float>(_scene->fogMode()),
                        _scene->fogStart, _scene->fogEnd, _scene->fogDensity);
      effect->setColor3("vFogColor", _scene->fogColor);
    };
  
    scene->fogMode  = Scene::FOGMODE_LINEAR;
    scene->fogStart = 0.f;
    scene->fogEnd   = 10.f;
    scene->fogColor
      = Color3(scene->clearColor.r, scene->clearColor.g, scene->clearColor.b);
  }

private:
  ShaderMaterialPtr _boxMaterial;

}; // end of class ShaderMaterialWithFogScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialWithFogScene)
} // end of namespace Samples
} // end of namespace BABYLON