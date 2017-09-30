#include <babylon/samples/materials/shader_material_box_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

ShaderMaterialBoxScene::ShaderMaterialBoxScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
{
  // Reference to the shaders contained in the shader store
  auto& shaders = _effectShadersStore.shaders();

  // Vertex shader
  shaders["customVertexShader"] = customVertexShader;

  // Fragment shader
  shaders["customFragmentShader"] = customFragmentShader;
}

ShaderMaterialBoxScene::~ShaderMaterialBoxScene()
{
}

const char* ShaderMaterialBoxScene::getName()
{
  return "Shader Material Box Scene";
}

void ShaderMaterialBoxScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a FreeCamera, and set its position to (x:0, y:0, z:-10)
  auto camera = FreeCamera::New("camera1", Vector3(0, 0, -10), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

  // Create a built-in "box" shape
  auto box = Mesh::CreateBox("box1", 5.f, scene);

  // Create shader material
  ShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes = {"position", "uv"};
  shaderMaterialOptions.uniforms   = {"worldViewProjection", "time"};
  _shaderMaterial
    = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);
  box->setMaterial(_shaderMaterial);

  // Animation
  scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
    _shaderMaterial->setFloat("time", _time);
    _time += 0.01f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
