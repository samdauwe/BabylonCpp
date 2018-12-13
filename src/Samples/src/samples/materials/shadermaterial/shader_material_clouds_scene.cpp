#include <babylon/samples/materials/shadermaterial/shader_material_clouds_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/core/random.h>
#include <babylon/core/time.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

ShaderMaterialCloudsScene::ShaderMaterialCloudsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _camera{nullptr}
    , _cloudMaterial{nullptr}
    , _startTime{Time::unixtimeInMs()}
{
  // Vertex shader
  Effect::ShadersStore()["cloudVertexShader"] = cloudVertexShader;

  // Fragment shader
  Effect::ShadersStore()["cloudFragmentShader"] = cloudFragmentShader;
}

ShaderMaterialCloudsScene::~ShaderMaterialCloudsScene()
{
}

const char* ShaderMaterialCloudsScene::getName()
{
  return "Shader Material Clouds Scene";
}

void ShaderMaterialCloudsScene::initializeScene(ICanvas* /*canvas*/,
                                                Scene* scene)
{
  // Change scene clear color
  auto clearColor   = Color3::FromHexString("#4584b4");
  scene->clearColor = Color4(clearColor.r, clearColor.g, clearColor.b);

  // Create a camera
  _camera       = FreeCamera::New("camera", Vector3(0.f, -128.f, 0.f), scene);
  _camera->fov  = 30.f;
  _camera->minZ = 1.f;
  _camera->maxZ = 3000.f;

  // Create shader material
  IShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.needAlphaBlending = true;
  shaderMaterialOptions.attributes        = {"position", "uv"};
  shaderMaterialOptions.uniforms          = {"worldViewProjection"};
  shaderMaterialOptions.samplers          = {"textureSampler"};
  _cloudMaterial
    = ShaderMaterial::New("cloud", scene, "cloud", shaderMaterialOptions);
  _cloudMaterial->setTexture("textureSampler",
                             Texture::New("/textures/cloud.png", scene));
  _cloudMaterial->setFloat("fogNear", -100);
  _cloudMaterial->setFloat("fogFar", 3000);
  _cloudMaterial->setColor3("fogColor", Color3::FromInts(69, 132, 180));

  // Create merged planes
  auto size                                    = 128.f;
  size_t count                                 = 8000;
  std::unique_ptr<VertexData> globalVertexData = nullptr;

  for (size_t i = 0; i < count; ++i) {
    PlaneOptions planeOptions(size);
    auto planeVertexData = VertexData::CreatePlane(planeOptions);

    planeVertexData->normals.clear(); // We do not need normals

    // Transform
    float randomScaling  = Math::random() * Math::random() * 1.5f + 0.5f;
    auto transformMatrix = Matrix::Scaling(randomScaling, randomScaling, 1.f);
    transformMatrix
      = transformMatrix.multiply(Matrix::RotationZ(Math::random() * Math::PI));
    transformMatrix = transformMatrix.multiply(
      Matrix::Translation(Math::random() * 1000.f - 500.f,
                          -Math::random() * Math::random() * 100, count - i));

    planeVertexData->transform(transformMatrix);

    // Merge
    if (!globalVertexData) {
      globalVertexData = std::move(planeVertexData);
    }
    else {
      globalVertexData->merge(*planeVertexData);
    }
  }

  auto clouds1 = Mesh::New("Clouds1", scene);
  globalVertexData->applyToMesh(*clouds1);
  clouds1->material = _cloudMaterial;

  auto clouds2 = Mesh::New("Clouds2", scene);
  globalVertexData->applyToMesh(*clouds2);
  clouds2->material     = _cloudMaterial;
  clouds2->position().z = -500.f;

  // Animation
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    float cameraDepth
      = std::fmod((Time::unixtimeInMs() - _startTime) * 0.03f, 8000.f);
    _camera->position.z = cameraDepth;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
