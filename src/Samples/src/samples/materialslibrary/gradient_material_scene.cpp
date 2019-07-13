#include <babylon/samples/materialslibrary/gradient_material_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materialslibrary/gradient/gradient_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>

namespace BABYLON {
namespace Samples {

GradientMaterialScene::GradientMaterialScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

GradientMaterialScene::~GradientMaterialScene()
{
}

const char* GradientMaterialScene::getName()
{
  return "Gradient Material Scene";
}

void GradientMaterialScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = FreeCamera::New("camera1", Vector3(0.f, 0.f, -2.f), scene);
  camera->setTarget(Vector3::Zero());
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  light->groundColor = Color3(1.f, 1.f, 1.f);
  light->intensity   = 1.f;

  PlaneOptions options;
  options.size        = 1.f;
  auto plane          = MeshBuilder::CreatePlane("plane", options, scene);
  plane->position().y = 0.f;

  auto gradientMaterial
    = MaterialsLibrary::GradientMaterial::New("grad", scene);
  gradientMaterial->topColor    = Color3(0.f, 0.f, 1.f);
  gradientMaterial->bottomColor = Color3(0.f, 0.f, 0.f);
  gradientMaterial->offset      = 0.5f;
  gradientMaterial->smoothness  = 1.f;
  plane->material               = gradientMaterial;
}

} // end of namespace Samples
} // end of namespace BABYLON
