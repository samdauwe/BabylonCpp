#include <babylon/samples/specialfx/fireworks_with_shader_code_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/engine/engine.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

FireworksWithShaderCodeScene::FireworksWithShaderCodeScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
{
  // Reference to the shaders contained in the shader store
  auto& shaders = _effectShadersStore.shaders();

  // Vertex shader
  shaders["customVertexShader"] = customVertexShader;

  // Fragment shader
  shaders["customFragmentShader"] = customFragmentShader;
}

FireworksWithShaderCodeScene::~FireworksWithShaderCodeScene()
{
}

const char* FireworksWithShaderCodeScene::getName()
{
  return "Fireworks with Shader Code Scene";
}

void FireworksWithShaderCodeScene::initializeScene(ICanvas* canvas,
                                                   Scene* scene)
{
  // Create a FreeCamera
  auto camera = FreeCamera::New("camera1", Vector3(0, 100, -200), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create 2 lights
  DirectionalLight::New("DirectionalLight", Vector3(0, -1, 1), scene);
  auto light2 = HemisphericLight::New("HemiLight", Vector3(0, 1, 0), scene);
  light2->intensity = 0.5f;

  // Create shader material
  ShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes = {"position", "normal", "uv"};
  shaderMaterialOptions.uniforms
    = {"world", "worldView", "worldViewProjection", "view", "projection"};
  shaderMaterialOptions.needAlphaBlending = true;
  _shaderMaterial
    = ShaderMaterial::New("shader", scene, "custom", shaderMaterialOptions);
  _shaderMaterial->setBackFaceCulling(false);

  // Create a built-in "sphere" shape
  SphereOptions sphereOptions(10.f);
  auto sphere = MeshBuilder::CreateSphere("sphere", sphereOptions, scene);
  sphere->convertToFlatShadedMesh();
  sphere->setMaterial(_shaderMaterial);

  // Animation
  scene->registerBeforeRender([this](Scene*, EventState&) {
    if (_time < 8.f) {
      _shaderMaterial->setFloat("time", _time);
      _time += 0.05f * getScene()->getAnimationRatio();
    }
    else {
      _time = 0.f;
    }
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
