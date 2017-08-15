#include <babylon/samples/mesh/extrusion_scene.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/path2.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/polygonmesh/polygon.h>

namespace BABYLON {
namespace Samples {

ExtrusionScene::ExtrusionScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

ExtrusionScene::~ExtrusionScene()
{
}

const char* ExtrusionScene::getName()
{
  return "Extrusion Scene";
}

void ExtrusionScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a ArcRotateCamera (non-mesh)
  auto camera = ArcRotateCamera::New("Camera", 3 * Math::PI_2, 0.8f, 80,
                                     Vector3::Zero(), scene);

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light      = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);
  light->diffuse  = Color3::FromInts(246, 135, 18); // "0xf68712"
  light->specular = Color3::FromInts(241, 71, 29);  // "0xf1471d"

  // 2D shape
  auto poly = Polygon::StartingAt(-10.f, -10.f)
                .addLineTo(10.f, -10.f)
                .addLineTo(10.f, -5.f)
                .addArcTo(17.f, 0.f, 10.f, 5.f)
                .addLineTo(10.f, 10.f)
                .addLineTo(5.f, 10.f)
                .addArcTo(0.f, 0.f, -5.f, 10.f)
                .addLineTo(-10.f, 10.f)
                .close();

  std::vector<Vector3> shape;
  for (auto& p : poly.getPoints()) {
    shape.emplace_back(Vector3(p.x, p.y, 0.f));
  }

  auto createLathe = [](const std::vector<Vector3>& _shape, float radius,
                        unsigned int tessellation, Scene* iscene) {
    float pi2 = Math::PI2;
    std::vector<Vector3> shapeLathe;
    unsigned int i = 0;
    while ((i < _shape.size()) && (stl_util::almost_equal(_shape[i].x, 0.f))) {
      ++i;
    }
    const Vector3& pt = _shape[i]; // first rotatable point
    for (auto& p : _shape) {
      shapeLathe.emplace_back(p.subtract(pt));
    }
    // Circle path
    float step = pi2 / static_cast<float>(tessellation);
    std::vector<Vector3> path;
    for (unsigned int ti = 0; ti < tessellation; ++ti) {
      // Rotate
      float _if = static_cast<float>(ti);
      path.emplace_back(Vector3(std::cos(_if * step) * radius, 0.f,
                                std::sin(_if * step) * radius));
    }
    // Extrusion
    auto scaleFunction  = [](float /*i*/, float /*distance*/) { return 1.f; };
    auto rotateFunction = [](float /*i*/, float /*distance*/) { return 0.f; };
    return Mesh::ExtrudeShapeCustom("lathe", shapeLathe, path, scaleFunction,
                                    rotateFunction, true, false, 0, iscene);
  };

  auto lathe = createLathe(shape, 1, 40, scene);
  Vector3 translateVector(0.f, 1.f, 0.f);
  lathe->translate(translateVector, -4, Space::LOCAL);
  lathe->setMaterial(StandardMaterial::New("mat", scene));
  lathe->material()->setBackFaceCulling(false);
}

} // end of namespace Samples
} // end of namespace BABYLON
