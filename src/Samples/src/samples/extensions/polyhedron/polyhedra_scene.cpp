#include <babylon/babylon_common.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/extensions/polyhedron/polyhedra.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {

namespace Samples {

class PolyhedraScene : public IRenderableScene {

public:
  PolyhedraScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~PolyhedraScene() override = default;

  const char* getName() override
  {
    return "Polyhedra Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    _camera = ArcRotateCamera::New("Camera", 0.f, 0.f, 0.f, Vector3::Zero(), scene);
    _camera->setPosition(Vector3(0, 0, -100));

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

    unsigned int col = 0, row = 0, counter = 0;
    for (const auto& polyhedron : Extensions::Polyhedra().models()) {
      PolyhedronOptions options;
      options.size   = 2.f;
      options.custom = polyhedron;
      auto polygon   = Mesh::CreatePolyhedron(polyhedron.name, options, scene);
      // polygon->convertToFlatShadedMesh();
      polygon->material = mat;
      col               = counter % 18;
      if (col == 0) {
        ++row;
      }
      polygon->position().x = 10.f + (static_cast<float>(col) - 10.f) * 8.f;
      polygon->position().y = 32.f - static_cast<float>(row) * 8.f;

      _polygons.emplace_back(polygon);
      _rotations.emplace_back((0.5f - Math::random()) / 8.f);
      ++counter;
    }

    _scene->registerBeforeRender([this](Scene*, EventState&) {
      for (unsigned int p = 0; p < _polygons.size(); ++p) {
        _polygons[p]->rotation().y += _rotations[p];
        _polygons[p]->rotation().x += _rotations[p] / 4.f;
      }
      _pointLight->position = _camera->position;
    });
  }

private:
  ArcRotateCameraPtr _camera;
  PointLightPtr _pointLight;
  std::vector<MeshPtr> _polygons;
  Float32Array _rotations;

}; // end of struct PolyhedraScene

BABYLON_REGISTER_SAMPLE("Extensions", PolyhedraScene)

} // end of namespace Samples
} // end of namespace BABYLON
