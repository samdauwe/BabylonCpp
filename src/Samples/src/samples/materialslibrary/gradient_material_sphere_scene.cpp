#include <babylon/samples/materialslibrary/gradient_material_sphere_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materialslibrary/gradient/gradient_material.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

GradientMaterialSphereScene::GradientMaterialSphereScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _sphere{nullptr}, _alpha{Math::PI}
{
}

GradientMaterialSphereScene::~GradientMaterialSphereScene()
{
}

const char* GradientMaterialSphereScene::getName()
{
  return "Gradient Material Sphere Scene";
}

void GradientMaterialSphereScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = FreeCamera::New("camera1", Vector3(0.f, 5.f, -10.f), scene);
  camera->setTarget(Vector3::Zero());
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  light->groundColor = Color3(1.f, 1.f, 1.f);
  light->intensity   = 1.f;

  _sphere               = Mesh::CreateSphere("sphere1", 16, 5, scene);
  _sphere->position().y = 0;

  auto gradientMaterial
    = MaterialsLibrary::GradientMaterial::New("grad", scene);
  gradientMaterial->topColor    = Color3::Green();
  gradientMaterial->bottomColor = Color3::Blue();
  gradientMaterial->offset      = 0.f;
  gradientMaterial->smoothness  = 1.3f;
  _sphere->material             = gradientMaterial;

  scene->registerBeforeRender([this](Scene*, EventState&) {
    _sphere->position().y += std::cos(_alpha) / 100.f;
    _alpha += 0.01f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
