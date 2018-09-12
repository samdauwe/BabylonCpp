#include <babylon/samples/materialslibrary/grid_material_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/materialslibrary/grid/grid_material.h>
#include <babylon/mesh/ground_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

GridMaterialScene::GridMaterialScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _camera{nullptr}
{
}

GridMaterialScene::~GridMaterialScene()
{
}

const char* GridMaterialScene::getName()
{
  return "Grid Material Scene";
}

void GridMaterialScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // This creates a basic Babylon Scene object (non-mesh)
  scene->clearColor = Color3::Black();

  // This creates and positions a free camera (non-mesh)
  _camera
    = ArcRotateCamera::New("camera1", -Math::PI / 3.f, 5.f * Math::PI / 12.f,
                           50, Vector3::Zero(), scene);
  _camera->setTarget(Vector3(0.f, 5.f, 0.f));

  // This attaches the camera to the canvas
  _camera->attachControl(canvas, true);

  auto defaultGridMaterial
    = MaterialsLibrary::GridMaterial::New("default", scene);
  defaultGridMaterial->majorUnitFrequency = 5.f;
  defaultGridMaterial->gridRatio          = 0.5f;

  auto sphere          = Mesh::CreateSphere("sphere", 20, 9, scene);
  sphere->position().y = 12.f;
  sphere->position().x = -6.f;
  sphere->material     = defaultGridMaterial;

  auto knotMaterial
    = MaterialsLibrary::GridMaterial::New("knotMaterial", scene);
  knotMaterial->majorUnitFrequency  = 8.f;
  knotMaterial->minorUnitVisibility = 0.45f;
  knotMaterial->gridRatio           = 0.3f;
  knotMaterial->mainColor           = Color3(0.f, 0.f, 0.f);
  knotMaterial->lineColor           = Color3(0.f, 1.f, 0.f);

  auto knot = Mesh::CreateTorusKnot("knot", 3.f, 1, 128, 64, 2, 3, scene);
  knot->position().y = 12.f;
  knot->position().x = 6.f;
  knot->material     = knotMaterial;

  auto groundMaterial
    = MaterialsLibrary::GridMaterial::New("groundMaterial", scene);
  groundMaterial->majorUnitFrequency  = 5.f;
  groundMaterial->minorUnitVisibility = 0.45f;
  groundMaterial->gridRatio           = 2.f;
  groundMaterial->backFaceCulling     = false;
  groundMaterial->mainColor           = Color3(1.f, 1.f, 1.f);
  groundMaterial->lineColor           = Color3(1.f, 1.f, 1.f);
  groundMaterial->opacity             = 0.98f;

  auto ground = Mesh::CreateGroundFromHeightMap(
    "ground", "textures/heightMap.png", 100, 100, 100, 0, 10, scene, false);
  ground->material = groundMaterial;

  auto skyMaterial = MaterialsLibrary::GridMaterial::New("skyMaterial", scene);
  skyMaterial->majorUnitFrequency  = 6.f;
  skyMaterial->minorUnitVisibility = 0.43f;
  skyMaterial->gridRatio           = 0.5f;
  skyMaterial->mainColor           = Color3(0.f, 0.05f, 0.2f);
  skyMaterial->lineColor           = Color3(0.f, 1.0f, 1.0f);
  skyMaterial->backFaceCulling     = false;

  auto skySphere      = Mesh::CreateSphere("skySphere", 30, 110, scene);
  skySphere->material = skyMaterial;

  scene->registerBeforeRender(
    [this](Scene*, EventState&) { _camera->alpha += 0.003f; });
}

} // end of namespace Samples
} // end of namespace BABYLON
