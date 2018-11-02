#include <babylon/samples/cameras/multi_views_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

MultiViewsScene::MultiViewsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

MultiViewsScene::~MultiViewsScene()
{
}

const char* MultiViewsScene::getName()
{
  return "Multi-Views Scene";
}

void MultiViewsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Change clear color
  scene->clearColor = Color3(0.5f, 0.5f, 0.5f);

  // Camera 1
  auto camera1
    = ArcRotateCamera::New("camera1", 3.f * Math::PI / 8.f,
                           3.f * Math::PI / 8.f, 15, Vector3::Zero(), scene);
  camera1->setTarget(Vector3(0.f, 2.f, 0.f));
  camera1->attachControl(canvas, true);

  // Camera 2
  auto camera2
    = ArcRotateCamera::New("camera1", 5.f * Math::PI / 8.f,
                           5.f * Math::PI / 8.f, 30, Vector3::Zero(), scene);
  camera2->setTarget(Vector3(0.f, 2.f, 0.f));
  camera2->attachControl(canvas, true);

  // Create two viewports
  camera1->viewport = Viewport(0.f, 0.5f, 1.f, 1.f);
  camera2->viewport = Viewport(0.f, 0.f, 1.f, 0.5f);

  scene->activeCameras.emplace_back(camera1);
  scene->activeCameras.emplace_back(camera2);

  // Create two lights
  auto light1 = HemisphericLight::New("light1", Vector3(1.f, 0.5f, 0.f), scene);
  light1->intensity = 0.7f;
  auto light2
    = HemisphericLight::New("light2", Vector3(-1.f, -0.5f, 0.f), scene);
  light2->intensity = 0.8f;

  // Create Box
  const std::array<Color4, 6> faceColors{{
    Color3::Blue(),   // faceColors[0]
    Color3::White(),  // faceColors[1]
    Color3::Red(),    // faceColors[2]
    Color3::Black(),  // faceColors[3]
    Color3::Green(),  // faceColors[4]
    Color3::Yellow(), // faceColors[5]
  }};

  BoxOptions boxOptions(2.f);
  boxOptions.faceColors = std::move(faceColors);
  auto box              = MeshBuilder::CreateBox("Box", boxOptions, scene);
  box->material         = StandardMaterial::New("", scene);

  // Create and Draw Axes
  const auto showAxis = [scene](float size) {
    // X Axis
    {
      const std::vector<Vector3> axisXPoints{
        Vector3::Zero(), Vector3(size, 0.f, 0.f),
        Vector3(size * 0.95f, 0.05f * size, 0.f), Vector3(size, 0.f, 0.f),
        Vector3(size * 0.95f, -0.05f * size, 0.f)};
      auto axisX   = Mesh::CreateLines("axisY", axisXPoints, scene);
      axisX->color = Color3::Red();
    }
    // Y Axis
    {
      const std::vector<Vector3> axisYPoints{
        Vector3::Zero(), Vector3(0.f, size, 0.f),
        Vector3(-0.05f * size, size * 0.95f, 0.f), Vector3(0.f, size, 0.f),
        Vector3(0.05f * size, size * 0.95f, 0.f)};
      auto axisY   = Mesh::CreateLines("axisX", axisYPoints, scene);
      axisY->color = Color3::Green();
    }
    // Z Axis
    {
      const std::vector<Vector3> axisZPoints{
        Vector3::Zero(), Vector3(0.f, 0.f, size),
        Vector3(0.f, -0.05f * size, size * 0.95f), Vector3(0.f, 0.f, size),
        Vector3(0.f, 0.05f * size, size * 0.95f)};
      auto axisZ   = Mesh::CreateLines("axisZ", axisZPoints, scene);
      axisZ->color = Color3::Blue();
    }
  };

  showAxis(6.f);
}

} // end of namespace Samples
} // end of namespace BABYLON
