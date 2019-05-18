#include <babylon/samples/meshes/ribbon_scene.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace Samples {

RibbonScene::RibbonScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

RibbonScene::~RibbonScene()
{
}

const char* RibbonScene::getName()
{
  return "Ribbon Scene";
}

void RibbonScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Change the scene clear color
  scene->clearColor = Color4(0.5f, 0.5f, 0.5f);

  // Create a Camera
  auto camera
    = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3::Zero(), scene);

  // Change the position of the camera
  camera->setPosition(Vector3(0.f, 50.f, -300.f));

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("light1", Vector3(1, 0.5f, 0), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.7f;

  // Create a material
  auto mat             = StandardMaterial::New("mat1", scene);
  mat->alpha           = 1.f;
  mat->diffuseColor    = Color3(0.5f, 0.5f, 1.f);
  mat->backFaceCulling = false;

  // Parabola
  std::vector<std::vector<Vector3>> paths;
  for (float t = 1; t < 10.f; ++t) {
    std::vector<Vector3> path;
    for (float k = -20.f; k <= 20.f; ++k) {
      auto x = k * 8.f;
      auto y = k * k / t * std::sin(k / 3.f) * std::cos(t);
      auto z = t * 50.f;
      path.emplace_back(Vector3(x, y, z));
    }
    paths.emplace_back(path);
    Mesh::CreateLines("par", path, scene);
  }

  auto ribbon      = Mesh::CreateRibbon("rib", paths, false, false, 0, scene);
  ribbon->material = mat;
}

} // end of namespace Samples
} // end of namespace BABYLON
