#include <babylon/samples/materials/shadermaterial/shader_material_cylinders_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

ShaderMaterialCylindersScene::ShaderMaterialCylindersScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _time{0.f}
{
  // Vertex shaders
  Effect::ShadersStore()["custom1VertexShader"] = customVertexShader;
  Effect::ShadersStore()["custom2VertexShader"] = customVertexShader;
  Effect::ShadersStore()["custom3VertexShader"] = customVertexShader;
  Effect::ShadersStore()["custom4VertexShader"] = customVertexShader;
  Effect::ShadersStore()["custom5VertexShader"] = customVertexShader;
  Effect::ShadersStore()["custom6VertexShader"] = customVertexShader;
  Effect::ShadersStore()["custom7VertexShader"] = customVertexShader;
  Effect::ShadersStore()["custom8VertexShader"] = customVertexShader;
  Effect::ShadersStore()["custom9VertexShader"] = customVertexShader;

  // Fragment shaders
  Effect::ShadersStore()["custom1FragmentShader"] = custom1FragmentShader;
  Effect::ShadersStore()["custom2FragmentShader"] = custom2FragmentShader;
  Effect::ShadersStore()["custom3FragmentShader"] = custom3FragmentShader;
  Effect::ShadersStore()["custom4FragmentShader"] = custom4FragmentShader;
  Effect::ShadersStore()["custom5FragmentShader"] = custom5FragmentShader;
  Effect::ShadersStore()["custom6FragmentShader"] = custom6FragmentShader;
  Effect::ShadersStore()["custom7FragmentShader"] = custom7FragmentShader;
  Effect::ShadersStore()["custom8FragmentShader"] = custom8FragmentShader;
  Effect::ShadersStore()["custom9FragmentShader"] = custom9FragmentShader;
}

ShaderMaterialCylindersScene::~ShaderMaterialCylindersScene()
{
}

const char* ShaderMaterialCylindersScene::getName()
{
  return "Shader Material Cylinders Scene";
}

void ShaderMaterialCylindersScene::initializeScene(ICanvas* canvas,
                                                   Scene* scene)
{

  // Render camera
  auto camera1 = ArcRotateCamera::New("ArcRotateCamera", 1.f, 0.8f, 6.f,
                                      Vector3::Zero(), scene);
  camera1->attachControl(canvas, false);
  camera1->lowerRadiusLimit = 1;
  scene->activeCamera       = camera1;

  // Create a light
  PointLight::New("Omni", Vector3(-60.f, 60.f, 80.f), scene);

  // Box positions
  const std::array<Vector3, 9> boxPositions{
    {Vector3(-1.5f, 1.f, -1.5f), Vector3(0.f, 1.f, -1.5f),
     Vector3(1.5f, 1.f, -1.5f), Vector3(-1.5f, 1.f, 0.f),
     Vector3(0.f, 1.f, 0.f), Vector3(1.5, 1.f, 0.f), Vector3(-1.5, 1.f, 1.5),
     Vector3(0.f, 1.f, 1.5f), Vector3(1.5f, 1.f, 1.5f)}};

  // Create boxes
  for (unsigned int i = 0; i < boxPositions.size(); ++i) {
    // Create box
    auto id = std::to_string(i + 1);
    auto cylinder
      = Mesh::CreateCylinder("cylinder", 1, 1, 1, 6, 1, scene, false);
    cylinder->position = boxPositions[i];
    // box->enableEdgesRendering(0.99f);
    cylinder->edgesWidth = 2.f;
    // Create shader material
    ShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "normal", "uv"};
    shaderMaterialOptions.uniforms   = {"time", "worldViewProjection"};
    auto shaderMaterial              = ShaderMaterial::New(
      "shader" + id, scene, "custom" + id, shaderMaterialOptions);
    if (i == 1) {
      shaderMaterial->backFaceCulling = false;
    }
    cylinder->material = shaderMaterial;
    cylinder->rotation().y += Math::PI;
    _shaderMaterials[i] = shaderMaterial;
  }

  // Animation
  scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
    for (auto& shaderMaterial : _shaderMaterials) {
      shaderMaterial->setFloat("time", _time);
    }
    _time += 0.01f * getScene()->getAnimationRatio();
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
