#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/path2.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/polygonmesh/polygon.h>
#include <babylon/meshes/polygonmesh/polygon_mesh_builder.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct PolygonMeshPuzzlePieceScene : public IRenderableScene {

  PolygonMeshPuzzlePieceScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
  }
  ~PolygonMeshPuzzlePieceScene()
  {
  }

  const char* getName() override
  {
    return "Polygon Mesh Puzzle Piece Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
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
  
    auto ground      = PolygonMeshBuilder("ground1", polygon, scene).build();
    ground->material = scene->defaultMaterial();
    ground->material()->backFaceCulling = false;
    camera->setTarget(ground->position());
  }

}; // end of struct PolygonMeshPuzzlePieceScene


BABYLON_REGISTER_SAMPLE("Meshes", PolygonMeshPuzzlePieceScene)
} // end of namespace Samples
} // end of namespace BABYLON