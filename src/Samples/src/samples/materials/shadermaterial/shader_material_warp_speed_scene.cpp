#include <babylon/samples/materials/shadermaterial/shader_material_warp_speed_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

ShaderMaterialWarpSpeedScene::ShaderMaterialWarpSpeedScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
{
  // Vertex shader
  Effect::ShadersStore["customVertexShader"] = customVertexShader;

  // Fragment shader
  Effect::ShadersStore["customFragmentShader"] = customFragmentShader;
}

ShaderMaterialWarpSpeedScene::~ShaderMaterialWarpSpeedScene()
{
}

const char* ShaderMaterialWarpSpeedScene::getName()
{
  return "Shader Material Warp Speed Scene";
}

void ShaderMaterialWarpSpeedScene::initializeScene(ICanvas* canvas,
                                                   Scene* scene)
{
  // Create a camera
  auto camera = ArcRotateCamera::New("Camera", 0, Math::PI_2, 12.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, false);
  camera->lowerRadiusLimit = 1.f;
  camera->minZ             = 1.f;
  camera->setPosition(Vector3(-10.0, 0, -0.5f));

  // Create shader material
  ShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes = {"position", "normal", "uv"};
  shaderMaterialOptions.uniforms
    = {"world", "worldView", "worldViewProjection", "view", "projection"};
  _shaderMaterial
    = ShaderMaterial::New("shader", scene, "custom", shaderMaterialOptions);

  // Create plane mesh
  auto mesh       = Mesh::CreatePlane("mesh", 10.0, scene);
  auto rotateAxis = Axis::Y();
  mesh->rotate(rotateAxis, Math::PI * 0.5f);

  // Create texture
  auto refTexture = Texture::New("textures/warpSpeedNoise.png", scene);

  // Configure the shader material
  _shaderMaterial->setTexture("refSampler", refTexture);
  _shaderMaterial->setFloat("time", 0);
  _shaderMaterial->setVector3("cameraPosition", Vector3::Zero());
  _shaderMaterial->backFaceCulling = false;

  // Assign material
  mesh->material = _shaderMaterial;

  // Animation
  scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
    _shaderMaterial->setFloat("time", _time);
    _time += 0.02f;

    _shaderMaterial->setVector3("cameraPosition",
                                _scene->activeCamera->position);
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
