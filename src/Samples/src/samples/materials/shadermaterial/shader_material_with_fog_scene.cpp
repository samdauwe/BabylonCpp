#include <babylon/samples/materials/shadermaterial/shader_material_with_fog_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

ShaderMaterialWithFogScene::ShaderMaterialWithFogScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
  // Reference to the shaders contained in the shader store
  auto& shaders = _effectShadersStore.shaders();

  // Vertex shader
  shaders["customVertexShader"] = customVertexShader;

  // Fragment shader
  shaders["customFragmentShader"] = customFragmentShader;
}

ShaderMaterialWithFogScene::~ShaderMaterialWithFogScene()
{
}

const char* ShaderMaterialWithFogScene::getName()
{
  return "Shader Material With Fog Scene";
}

void ShaderMaterialWithFogScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a camera
  auto camera = ArcRotateCamera::New("camera1", Math::PI, Math::PI_2, 5.f,
                                     Vector3(0, 0, 0), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

  // Create a built-in "box" shape
  auto box = Mesh::CreateBox("box1", 1.f, scene);

  // Create shader material
  ShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.needAlphaBlending = true;
  shaderMaterialOptions.attributes        = {"position"};
  shaderMaterialOptions.uniforms
    = {"world", "view", "viewProjection", "vFogInfos", "vFogColor"};
  _boxMaterial
    = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);
  box->setMaterial(_boxMaterial);

  _boxMaterial->setOnBind([this](AbstractMesh*, EventState&) {
    auto effect = _boxMaterial->getEffect();
    effect->setFloat4("vFogInfos", _scene->fogMode(), _scene->fogStart,
                      _scene->fogEnd, _scene->fogDensity);
    effect->setColor3("vFogColor", _scene->fogColor);
  });

  scene->setFogMode(Scene::FOGMODE_LINEAR());
  scene->fogStart = 0.f;
  scene->fogEnd   = 10.f;
  scene->fogColor
    = Color3(scene->clearColor.r, scene->clearColor.g, scene->clearColor.b);
}

} // end of namespace Samples
} // end of namespace BABYLON
