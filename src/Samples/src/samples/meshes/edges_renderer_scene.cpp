#include <babylon/samples/meshes/edges_renderer_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

EdgesRenderScene::EdgesRenderScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

EdgesRenderScene::~EdgesRenderScene()
{
}

const char* EdgesRenderScene::getName()
{
  return "Edges Render Scene";
}

void EdgesRenderScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI / 3.f, 10.f,
                                     Vector3::Zero(), scene);
  camera->setTarget(Vector3::Zero());
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity = 0.98f;

  auto box = Mesh::CreateBox("box1", 2.f, scene);
  box->enableEdgesRendering();
  box->edgesWidth   = 4.f;
  box->edgesColor   = Color4(0.f, 0.f, 1.f, 1.f);
  box->position().y = 1.2f;

  auto ground = Mesh::CreateGround("ground1", 6, 6, 1, scene);
  ground->enableEdgesRendering();
  ground->edgesWidth = 3.f;
}

} // end of namespace Samples
} // end of namespace BABYLON
