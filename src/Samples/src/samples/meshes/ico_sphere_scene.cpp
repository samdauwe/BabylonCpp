#include <babylon/samples/meshes/ico_sphere_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

IcoSphereScene::IcoSphereScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

IcoSphereScene::~IcoSphereScene()
{
}

const char* IcoSphereScene::getName()
{
  return "Icosphere Scene";
}

void IcoSphereScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a FreeCamera (non-mesh), and set its position to (x:0, y:5, z:-5)
  auto camera = FreeCamera::New("camera1", Vector3(0, 5, -5), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.7f;

  // Create an icosphere
  IcoSphereOptions options;
  options.radius       = 2.f;
  options.flat         = true;
  options.subdivisions = 16;
  Mesh::CreateIcoSphere("icosphere", options, scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
