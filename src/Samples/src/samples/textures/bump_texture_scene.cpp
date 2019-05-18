#include <babylon/samples/textures/bump_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

BumpTextureScene::BumpTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _sphere{nullptr}
{
}

BumpTextureScene::~BumpTextureScene()
{
}

const char* BumpTextureScene::getName()
{
  return "Bump Texture Scene";
}

void BumpTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
  camera->setPosition(Vector3(-5.f, 5.f, 0.f));
  camera->attachControl(canvas, true);

  auto light       = PointLight::New("Omni", Vector3(20, 100, 2), scene);
  light->intensity = 0.97f;

  auto material         = StandardMaterial::New("normalMap", scene);
  material->bumpTexture = Texture::New("textures/normalMap.jpg", scene);
  material->bumpTexture()->level = 1.f;
  material->diffuseColor         = Color3(1.f, 0.f, 0.f);

  _sphere           = Mesh::CreateSphere("Sphere", 16, 3, scene);
  _sphere->material = material;

  // Animations
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    _sphere->rotation().y += 0.02f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
