#include <babylon/samples/meshes/basic_elements_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

BasicElementsScene::BasicElementsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

BasicElementsScene::~BasicElementsScene()
{
}

const char* BasicElementsScene::getName()
{
  return "Basic Elements Scene";
}

void BasicElementsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a camera
  auto camera = ArcRotateCamera::New("Camera", 3.f * Math::PI_2, Math::PI / 8.f,
                                     50.f, Vector3::Zero(), scene);

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("hemi", Vector3(0, 1, 0), scene);
  // light->diffuse   = Color3::FromInts(246, 135, 18); // "0xf68712"
  // light->specular  = Color3::FromInts(241, 71, 29);  // "0xf1471d"
  light->intensity = 1.f;

  // Creation of a box
  // (name of the box, size, scene)
  auto box = Mesh::CreateBox("box", 6.f, scene);

  // Creation of a sphere
  // (name of the sphere, segments, diameter, scene)
  auto sphere = Mesh::CreateSphere("sphere", 10u, 10.f, scene);

  // Creation of a plane
  // (name of the plane, size, scene)
  auto plane = Mesh::CreatePlane("plane", 10.f, scene);

  // Creation of a cylinder
  // (name, height, diameter, tessellation, scene, updatable)
  auto cylinder = Mesh::CreateCylinder("cylinder", 3, 3, 3, 6, 1, scene, false);

  // Creation of a torus
  // (name, diameter, thickness, tessellation, scene, updatable)
  auto torus = Mesh::CreateTorus("torus", 5, 1, 10, scene, false);

  // Creation of a knot
  // (name, radius, tube, radialSegments, tubularSegments, p, q, scene,
  // updatable)
  auto knot = Mesh::CreateTorusKnot("knot", 2, 0.5f, 128, 64, 2, 3, scene);

  // Creation of a lines mesh
  Mesh::CreateLines("lines",
                    {Vector3(-10.f, 0.f, 0.f), //
                     Vector3(10.f, 0.f, 0.f),  //
                     Vector3(0.f, 0.f, -10.f), //
                     Vector3(0.f, 0.f, 10.f)}, //
                    scene);

  // Creation of a ribbon
  // let's first create many paths along a maths exponential function as an
  // example
  auto exponentialPath = [](std::vector<Vector3>& path, float p) {
    for (float i = -10.f; i < 10.f; ++i) {
      path.emplace_back(
        Vector3(p, i,
                std::sin(p / 3.f) * 5.f * std::exp(-(i - p) * (i - p) / 60.f)
                  + i / 3.f));
    }
  };
  // let's populate arrayOfPaths with all these different paths
  std::vector<std::vector<Vector3>> arrayOfPaths;
  arrayOfPaths.resize(20);
  for (unsigned int p = 0; p < 20; ++p) {
    exponentialPath(arrayOfPaths[p], static_cast<float>(p));
  }
  // (name, array of paths, closeArray, closePath, offset, scene)
  auto ribbon
    = Mesh::CreateRibbon("ribbon", arrayOfPaths, false, false, 0, scene);

  // Moving elements
  box->setPosition(Vector3(-10.f, 0.f, 0.f));   // Using a vector
  sphere->setPosition(Vector3(0.f, 10.f, 0.f)); // Using a vector
  plane->position().z    = 10.f; // Using a single coordinate component
  cylinder->position().z = -10.f;
  torus->position().x    = 10.f;
  knot->position().y     = -10.f;
  ribbon->setPosition(Vector3(-10, -10, 20));
}

} // end of namespace Samples
} // end of namespace BABYLON
