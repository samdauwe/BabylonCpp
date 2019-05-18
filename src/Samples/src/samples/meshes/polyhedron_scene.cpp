#include <babylon/samples/meshes/polyhedron_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

PolyhedronScene::PolyhedronScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

PolyhedronScene::~PolyhedronScene()
{
}

const char* PolyhedronScene::getName()
{
  return "Polyhedron Scene";
}

void PolyhedronScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a camera
  _camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, 0.f, Vector3::Zero(), scene);
  _camera->setPosition(Vector3(10, 0, -40));

  // Attach the camera to the canvas
  _camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.7f;

  // Create a point light
  _pointLight            = PointLight::New("pl", Vector3::Zero(), scene);
  _pointLight->diffuse   = Color3(1.f, 1.f, 1.f);
  _pointLight->specular  = Color3(1.f, 1.f, 1.f);
  _pointLight->intensity = 0.8f;

  // Create new material
  auto mat          = StandardMaterial::New("mat1", scene);
  mat->alpha        = 1.f;
  mat->diffuseColor = Color3(0.73f, 0.04f, 0.12f); // Ruby red

  int col = 0, row = 0;
  for (unsigned int type = 0; type < 15; ++type) {
    PolyhedronOptions options(2.f);
    options.type = type;
    auto polygon = MeshBuilder::CreatePolyhedron(
      String::concat("Polyhedron", type), options, scene);
    polygon->material = mat;
    col               = type % 5;
    if (col == 0) {
      ++row;
    }
    polygon->position().x = 62.f + (static_cast<float>(col) - 10.f) * 8.f;
    polygon->position().y = 16.f - static_cast<float>(row) * 8.f;

    _polygons.emplace_back(polygon);
    _rotations.emplace_back((0.5f - Math::random()) / 8.f);
  }

  _scene->registerBeforeRender([this](Scene*, EventState&) {
    for (unsigned int p = 0; p < _polygons.size(); ++p) {
      _polygons[p]->rotation().y += _rotations[p];
      _polygons[p]->rotation().x += _rotations[p] / 4.f;
    }
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
