#include <babylon/samples/cameras/cameras_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

CamerasScene::CamerasScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

CamerasScene::~CamerasScene()
{
}

const char* CamerasScene::getName()
{
  return "Cameras Scene";
}

void CamerasScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Setup a simple environment
  auto light0 = PointLight::New("Omni", Vector3(0.f, 2.f, 8.f), scene);
  light0->position().x += 0.f;
  auto box1 = Mesh::CreateBox("b1", 1.f, scene);
  box1->position().x += 0.f;
  auto box2          = Mesh::CreateBox("b2", 1.f, scene);
  box2->position().x = -3;
  auto box3          = Mesh::CreateBox("b3", 1.f, scene);
  box3->position().x = 3;

  // ArcRotateCamera >> Camera rotating around a 3D point (here Vector zero)
  // Parameters : name, alpha, beta, radius, target, scene
  auto arcCamera = ArcRotateCamera::New("ArcRotateCamera", 1.f, 0.8f, 10.f,
                                        Vector3(0.f, 0.f, 0.f), scene);
  arcCamera->setPosition(Vector3(0.f, 0.f, 50.f));
  arcCamera->target = Vector3(3.f, 0.f, 0.f);

  // FreeCamera >> You can move around the world with mouse and keyboard
  // (LEFT/RIGHT/UP/DOWN) Parameters : name, position, scene
  auto freeCamera
    = FreeCamera::New("FreeCamera", Vector3(0.f, 0.f, 5.f), scene);
  freeCamera->setRotation(Vector3(0, Math::PI, 0));

  // Attach a camera to the scene and the canvas
  scene->activeCamera = freeCamera;
  freeCamera->attachControl(canvas, true);
}

} // end of namespace Samples
} // end of namespace BABYLON
