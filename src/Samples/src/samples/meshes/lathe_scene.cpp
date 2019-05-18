#include <babylon/samples/meshes/lathe_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/curve3.h>
#include <babylon/math/vector3.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

LatheScene::LatheScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

LatheScene::~LatheScene()
{
}

const char* LatheScene::getName()
{
  return "Lathe Scene";
}

void LatheScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // camera
  auto camera = FreeCamera::New("camera1", Vector3(0, 1.25, -4), scene);
  camera->setTarget(Vector3(0.f, 1.25f, 0.f));
  camera->attachControl(canvas, true);

  auto light       = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);
  light->intensity = 0.7f;

  // Array of [point, tangent] values
  std::vector<std::vector<Vector3>> points{
    {Vector3(0.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f)},
    {Vector3(0.75f, 1.f, 0.f), Vector3(0.f, 1.5f, 0.f)},
    {Vector3(0.25f, 2.f, 0.f), Vector3(0.f, 1.f, 0.f)},
    {Vector3(0.5f, 2.5f, 0.f), Vector3(1.f, 0.f, 0.f).scaleInPlace(0.25f)}};

  // Create a sphere at each point
  unsigned int i = 1;
  for (auto& point : points) {
    auto sphere = Mesh::CreateSphere("sphere" + std::to_string(i++), 8, 0.15f,
                                     scene, true);
    sphere->position = point[0];
  }

  // Function for creating a spline segment from the point-tangent array
  auto createSegment = [&points](unsigned int _i) {
    return Curve3::CreateHermiteSpline(
      points[_i][0], points[_i][1], points[_i + 1][0], points[_i + 1][1], 32);
  };

  // Create a spline from the segments
  auto spline = createSegment(0);
  for (unsigned int s = 1; s < points.size() - 1; ++s) {
    auto segment = createSegment(s);
    spline       = spline.continueCurve3(segment);
  }

  // Radius function for the tube
  auto radius = [](unsigned int /*i*/, float /*distance*/) { return 0.025f; };

  // Add a tube to visualize the spline
  auto tube = Mesh::CreateTube("tube", spline.getPoints(), 0.025f, 16, radius,
                               1, scene, false);
  tube->position = Vector3(0.0125f, 0.f, 0.f);

  // Create a lathe from the spline
  auto lathe
    = Mesh::CreateLathe("tube", spline.getPoints(), 1, 16, scene, false);

  // Materials
  auto material1          = StandardMaterial::New("material1", scene);
  material1->diffuseColor = Color3(1.f, 0.5f, 0.5f);
  auto material2          = StandardMaterial::New("material2", scene);
  material2->diffuseColor = Color3(0.f, 0.5f, 1.f);
  material2->wireframe    = true;

  // Set object materials
  tube->material  = material1;
  lathe->material = material2;
}

} // end of namespace Samples
} // end of namespace BABYLON
