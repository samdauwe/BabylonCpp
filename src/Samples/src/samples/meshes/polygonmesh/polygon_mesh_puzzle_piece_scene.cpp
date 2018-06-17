#include <babylon/samples/meshes/polygonmesh/polygon_mesh_puzzle_piece_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/path2.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/polygonmesh/polygon.h>
#include <babylon/mesh/polygonmesh/polygon_mesh_builder.h>

namespace BABYLON {
namespace Samples {

PolygonMeshPuzzlePieceScene::PolygonMeshPuzzlePieceScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PolygonMeshPuzzlePieceScene::~PolygonMeshPuzzlePieceScene()
{
}

const char* PolygonMeshPuzzlePieceScene::getName()
{
  return "Polygon Mesh Puzzle Piece Scene";
}

void PolygonMeshPuzzlePieceScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a ArcRotateCamera (non-mesh)
  auto camera = ArcRotateCamera::New("Camera", 3.f * Math::PI_2, Math::PI / 8.f,
                                     50.f, Vector3::Zero(), scene);

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 1.f;

  // 2D shape
  auto polygon = Polygon::StartingAt(-10.f, -10.f)
                   .addLineTo(10.f, -10.f)
                   .addLineTo(10.f, -5.f)
                   .addArcTo(17.f, 0.f, 10.f, 5.f)
                   .addLineTo(10.f, 10.f)
                   .addLineTo(5.f, 10.f)
                   .addArcTo(0.f, 0.f, -5.f, 10.f)
                   .addLineTo(-10.f, 10.f)
                   .close();

  auto ground = PolygonMeshBuilder("ground1", polygon, scene).build();
  ground->setMaterial(scene->defaultMaterial());
  ground->material()->setBackFaceCulling(false);
  camera->setTarget(ground->position());
}

} // end of namespace Samples
} // end of namespace BABYLON
