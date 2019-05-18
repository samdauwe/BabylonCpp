#include <babylon/samples/meshes/tiled_ground_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>

namespace BABYLON {
namespace Samples {

TiledGroundScene::TiledGroundScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

TiledGroundScene::~TiledGroundScene()
{
}

const char* TiledGroundScene::getName()
{
  return "Tiled Ground Scene";
}

void TiledGroundScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a rotating camera
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI / 3.f,
                                     12.f, Vector3::Zero(), scene);

  // Attach it to handle user inputs (keyboard, mouse, touch)
  camera->attachControl(canvas, false);

  // Add a light
  auto light = HemisphericLight::New("hemi", Vector3(0.f, 1.f, 0.f), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.95f;

  // Part 1 : Creation of Tiled Ground
  // Parameters
  float xmin = -3.f;
  float zmin = -3.f;
  float xmax = 3.f;
  float zmax = 3.f;
  ISize precision(2, 2);
  ISize subdivisions(8, 8);
  // Create the Tiled Ground
  auto tiledGround = Mesh::CreateTiledGround(
    "Tiled Ground", xmin, zmin, xmax, zmax, subdivisions, precision, scene);

  // Part 2 : Create the multi material
  // Create different materials
  auto whiteMaterial          = StandardMaterial::New("White", scene);
  whiteMaterial->diffuseColor = Color3::White();

  auto blackMaterial          = StandardMaterial::New("Black", scene);
  blackMaterial->diffuseColor = Color3::Black();

  // Create Multi Material
  auto multimat = MultiMaterial::New("multi", scene);
  multimat->subMaterials().emplace_back(whiteMaterial);
  multimat->subMaterials().emplace_back(blackMaterial);

  // Part 3 : Apply the multi material
  // Define multimat as material of the tiled ground
  tiledGround->material = multimat;

  // Needed variables to set subMeshes
  auto verticesCount = tiledGround->getTotalVertices();
  unsigned int tileIndicesLength
    = static_cast<unsigned int>(tiledGround->getIndices().size())
      / static_cast<unsigned int>(subdivisions.width * subdivisions.height);

  // Set subMeshes of the tiled ground
  tiledGround->subMeshes.clear();
  unsigned int base = 0;
  for (int row = 0; row < subdivisions.height; row++) {
    for (int col = 0; col < subdivisions.width; col++) {
      tiledGround->subMeshes.emplace_back(
        SubMesh::New(row % 2 ^ col % 2, 0, verticesCount, base,
                     tileIndicesLength, tiledGround));
      base += tileIndicesLength;
    }
  }
}

} // end of namespace Samples
} // end of namespace BABYLON
