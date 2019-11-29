#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/maths/path2.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/polygonmesh/polygon.h>
#include <babylon/meshes/polygonmesh/polygon_mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

struct PolygonMeshStarsWithHolesScene : public IRenderableScene {

  PolygonMeshStarsWithHolesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PolygonMeshStarsWithHolesScene() override = default;

  const char* getName() override
  {
    return "Polygon Mesh Stars With Holes Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a ArcRotateCamera (non-mesh)
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI / 4.f, 25.f, Vector3::Zero(), scene);

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light = HemisphericLight::New("light1", Vector3(0, 10, 0), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.5f;

    const std::vector<Vector2> corners{
      Vector2(4, -4),  //
      Vector2(2, 0),   //
      Vector2(5, 2),   //
      Vector2(1, 2),   //
      Vector2(-5, 5),  //
      Vector2(-3, 1),  //
      Vector2(-4, -4), //
      Vector2(-2, -3), //
      Vector2(2, -3),  //
    };

    const std::vector<Vector2> hole{
      Vector2(1.f, -1.f),  //
      Vector2(1.5f, 0.f),  //
      Vector2(1.4f, 1.f),  //
      Vector2(0.5f, 1.5f), //
    };

    PolygonMeshBuilder polytri{"polytri", corners, scene};
    polytri.addHole(hole);
    auto polygon          = polytri.build(false, 0.5f);
    polygon->position().y = 4.f;

    Path2 polyPath{2.f, 0.f};
    polyPath.addLineTo(5, 2);
    polyPath.addLineTo(1, 2);
    polyPath.addLineTo(-5, 5);
    polyPath.addLineTo(-3, 1);
    polyPath.addLineTo(-4, -4);
    polyPath.addArcTo(0, -2, 4, -4, 100);

    PolygonMeshBuilder polytri2("polytri2", polyPath, scene);
    polytri2.addHole(hole);
    auto polygon2          = polytri2.build(false, 0.5f);
    polygon2->position().y = -4.f;
  }

}; // end of struct PolygonMeshStarsWithHolesScene

BABYLON_REGISTER_SAMPLE("Meshes", PolygonMeshStarsWithHolesScene)

} // end of namespace Samples
} // end of namespace BABYLON
