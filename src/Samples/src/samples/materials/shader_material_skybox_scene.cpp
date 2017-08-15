#include <babylon/samples/materials/shader_material_skybox_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

ShaderMaterialSkyboxScene::ShaderMaterialSkyboxScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _shaderMaterial{nullptr}
{
  // Vertex shader
  EffectShadersStore::Shaders["gradientVertexShader"] = gradientVertexShader;

  // Pixel (Fragment) Shader
  EffectShadersStore::Shaders["gradientFragmentShader"] = gradientPixelShader;
}

ShaderMaterialSkyboxScene::~ShaderMaterialSkyboxScene()
{
}

const char* ShaderMaterialSkyboxScene::getName()
{
  return "Shader Material Skybox Scene";
}

void ShaderMaterialSkyboxScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a FreeCamera
  auto camera = FreeCamera::New("cam", Vector3(0, 0, -500.f), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Hemispheric light to light the scene
  HemisphericLight::New("hemi", Vector3(0, 1, 0), scene);

  // The skybox creation
  auto skybox = Mesh::CreateSphere("skyBox", 10.f, 1000.f, scene);

  // Create shader material
  ShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes = {"position", "normal", "uv"};
  shaderMaterialOptions.uniforms   = {"worldViewProjection"};
  _shaderMaterial
    = ShaderMaterial::New("gradient", scene, "gradient", shaderMaterialOptions);
  _shaderMaterial->setFloat("offset", 0.f);
  _shaderMaterial->setColor3("topColor", Color3::FromInts(0, 119, 255));
  _shaderMaterial->setColor3("bottomColor", Color3::FromInts(240, 240, 255));
  _shaderMaterial->setBackFaceCulling(false);

  // box + sky = skybox !
  skybox->setMaterial(_shaderMaterial);
}

} // end of namespace Samples
} // end of namespace BABYLON
