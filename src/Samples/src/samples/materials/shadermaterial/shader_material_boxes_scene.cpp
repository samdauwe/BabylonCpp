#include <babylon/samples/materials/shadermaterial/shader_material_boxes_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

ShaderMaterialBoxesScene::ShaderMaterialBoxesScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _time{0.f}
{
  // Reference to the shaders contained in the shader store
  auto& shaders = _effectShadersStore.shaders();

  // Vertex shaders
  shaders["custom1VertexShader"] = customVertexShader;
  shaders["custom2VertexShader"] = customVertexShader;
  shaders["custom3VertexShader"] = customVertexShader;
  shaders["custom4VertexShader"] = customVertexShader;
  shaders["custom5VertexShader"] = customVertexShader;
  shaders["custom6VertexShader"] = customVertexShader;
  shaders["custom7VertexShader"] = customVertexShader;
  shaders["custom8VertexShader"] = customVertexShader;
  shaders["custom9VertexShader"] = customVertexShader;

  // Fragment shaders
  shaders["custom1FragmentShader"] = custom1FragmentShader;
  shaders["custom2FragmentShader"] = custom2FragmentShader;
  shaders["custom3FragmentShader"] = custom3FragmentShader;
  shaders["custom4FragmentShader"] = custom4FragmentShader;
  shaders["custom5FragmentShader"] = custom5FragmentShader;
  shaders["custom6FragmentShader"] = custom6FragmentShader;
  shaders["custom7FragmentShader"] = custom7FragmentShader;
  shaders["custom8FragmentShader"] = custom8FragmentShader;
  shaders["custom9FragmentShader"] = custom9FragmentShader;
}

ShaderMaterialBoxesScene::~ShaderMaterialBoxesScene()
{
}

const char* ShaderMaterialBoxesScene::getName()
{
  return "Shader Material Boxes Scene";
}

void ShaderMaterialBoxesScene::initializeScene(ICanvas* canvas, Scene* scene)
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
    auto id       = std::to_string(i + 1);
    auto box      = Mesh::CreateBox("box" + id, 1.f, scene);
    box->position = boxPositions[i];
    // box->enableEdgesRendering(0.99f);
    box->edgesWidth = 2.f;
    // Create shader material
    ShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "normal", "uv"};
    shaderMaterialOptions.uniforms   = {"time", "worldViewProjection"};
    auto shaderMaterial              = ShaderMaterial::New(
      "shader" + id, scene, "custom" + id, shaderMaterialOptions);
    if (i == 1) {
      shaderMaterial->backFaceCulling = false;
    }
    box->material       = shaderMaterial;
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
