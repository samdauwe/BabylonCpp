#include <babylon/samples/meshes/curved_helix_meshes_scene.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace Samples {

CurvedHelixMeshesScene::CurvedHelixMeshesScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

CurvedHelixMeshesScene::~CurvedHelixMeshesScene()
{
}

const char* CurvedHelixMeshesScene::getName()
{
  return "Curved Helix Meshes Scene";
}

void CurvedHelixMeshesScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Change the scene clear color
  scene->clearColor = Color4(0.5f, 0.5f, 0.5f);

  // Create a camera
  auto camera
    = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3(0, 0, -0), scene);
  camera->setPosition(Vector3(5.f, 5.f, -12.f));

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light
  auto light = HemisphericLight::New("light1", Vector3(1, 0.5f, 0), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.7f;

  // Create a second light
  auto spot       = SpotLight::New("spot", Vector3(25.f, 15.f, -10.f),
                             Vector3(-1.f, -0.8f, 1.f), 15.f, 1.f, scene);
  spot->diffuse   = Color3(1.f, 1.f, 1.f);
  spot->specular  = Color3(0.f, 0.f, 0.f);
  spot->intensity = 0.8f;

  // Create a material for the mesh
  auto mat             = StandardMaterial::New("mat1", scene);
  mat->alpha           = 1.f;
  mat->diffuseColor    = Color3(0.5f, 0.5f, 1.f);
  mat->backFaceCulling = false;

  // Create the curved helix meshes
  std::vector<Vector3> shape{
    Vector3(1.f, 1.f, 0.f),    //
    Vector3(0.2f, 1.3f, 0.f),  //
    Vector3(0.f, 1.f, 0.f),    //
    Vector3(-0.2f, 1.3f, 0.f), //
    Vector3(-1.f, 1.f, 0.f)    //
  };

  std::vector<Vector3> path;
  path.reserve(100);
  for (unsigned int i = 0; i < 100; ++i) {
    path.emplace_back(Vector3(i / 5.f - 10.f, i / 5.f - 10.f, 0.f));
  }

  auto pathline   = Mesh::CreateLines("pl", path, scene);
  pathline->color = Color3::Magenta();

  auto extruded      = Mesh::ExtrudeShape("extruded", shape, path, 0.5f,
                                     Math::PI / 5.f, 0, scene);
  extruded->material = mat;

  // Show the axis
  _showAxis(5.f, scene);
}

void CurvedHelixMeshesScene::_showAxis(float size, Scene* scene)
{
  // X-Axis
  auto axisX   = Mesh::CreateLines("axisX",
                                 {Vector3::Zero(), Vector3(size, 0.f, 0.f),
                                  Vector3(size * 0.95f, 0.05f * size, 0.f),
                                  Vector3(size, 0.f, 0.f),
                                  Vector3(size * 0.95f, -0.05f * size, 0.f)},
                                 scene);
  axisX->color = Color3(1.f, 0.f, 0.f);

  // Y-Axis
  auto axisY   = Mesh::CreateLines("axisY",
                                 {Vector3::Zero(), Vector3(0.f, size, 0.f),
                                  Vector3(-0.05f * size, size * 0.95f, 0.f),
                                  Vector3(0.f, size, 0.f),
                                  Vector3(0.05f * size, size * 0.95f, 0.f)},
                                 scene);
  axisY->color = Color3(0.f, 1.f, 0.f);
  // Z-Axis
  auto axisZ   = Mesh::CreateLines("axisZ",
                                 {Vector3::Zero(), Vector3(0.f, 0.f, size),
                                  Vector3(0.f, -0.05f * size, size * 0.95f),
                                  Vector3(0.f, 0.f, size),
                                  Vector3(0.f, 0.05f * size, size * 0.95f)},
                                 scene);
  axisZ->color = Color3(0.f, 0.f, 1.f);
}

} // end of namespace Samples
} // end of namespace BABYLON
