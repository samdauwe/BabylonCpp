#include <babylon/samples/materials/shadermaterial/shader_material_chains_and_gears_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

ShaderMaterialChainsAndGearsScene::ShaderMaterialChainsAndGearsScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
{
  // Vertex shader
  Effect::ShadersStore()["customVertexShader"] = customVertexShader;

  // Fragment shader
  Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
}

ShaderMaterialChainsAndGearsScene::~ShaderMaterialChainsAndGearsScene()
{
}

const char* ShaderMaterialChainsAndGearsScene::getName()
{
  return "Shader Material Chains and Gears Scene";
}

void ShaderMaterialChainsAndGearsScene::initializeScene(ICanvas* canvas,
                                                        Scene* scene)
{
  // Create a FreeCamera, and set its position to (x:0, y:0, z:-10)
  auto camera = FreeCamera::New("camera1", Vector3(0, 0, -8), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

  // Create a built-in "box" shape
  const float ratio = static_cast<float>(getEngine()->getRenderWidth())
                      / static_cast<float>(getEngine()->getRenderHeight());
  BoxOptions options(5.f);
  options.sideOrientation = Mesh::DEFAULTSIDE;
  options.updatable       = false;
  options.width           = options.width * ratio;
  auto skybox             = MeshBuilder::CreateBox("skybox", options, scene);

  // Create shader material
  IShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes = {"position", "uv"};
  shaderMaterialOptions.uniforms
    = {"iTime", "worldViewProjection", "iAspectRatio", "iResolution", "iMouse"};
  _shaderMaterial
    = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

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

} // end of namespace Samples
} // end of namespace BABYLON
