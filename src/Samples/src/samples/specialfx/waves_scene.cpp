#include <babylon/babylon_common.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

class WavesScene : public IRenderableScene {

public:
  WavesScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _ground{nullptr}, _ground2{nullptr}, _alpha{0.f}
  {
  }

  ~WavesScene() override = default;

  const char* getName() override
  {
    return "Waves Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);

    // Target the camera position
    camera->setPosition(Vector3(-10.f, 10.f, 0.f));

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.7f;

    // Create ground meshes
    _ground                        = Mesh::CreateGround("ground", 10, 10, 10, scene, true);
    auto _groundMaterial           = StandardMaterial::New("gmat", scene);
    _groundMaterial->diffuseColor  = Color3(0.4f, 0.4f, 0.8f);
    _ground->material              = _groundMaterial;
    _ground2                       = Mesh::CreateGround("ground", 10, 10, 10, scene, true);
    auto _ground2Material          = StandardMaterial::New("gmat2", scene);
    _ground2Material->diffuseColor = Color3(1.f, 1.f, 1.f);
    _ground2Material->wireframe    = true;
    _ground2->material             = _ground2Material;

    _positions = _ground->getVerticesData(VertexBuffer::PositionKind);
    _indices   = _ground->getIndices();

    // Animations
    _scene->registerBeforeRender([this](Scene*, EventState&) {
      float beta = _alpha;
      for (unsigned int index = 0; index < _positions.size(); index += 3) {
        _positions[index + 1] = std::sin(beta);
        beta += 0.1f;
      }
      _ground->updateVerticesData(VertexBuffer::PositionKind, _positions, false, true);
      _ground2->updateVerticesData(VertexBuffer::PositionKind, _positions, false, true);
      Float32Array normals;
      VertexData::ComputeNormals(_positions, _indices, normals);

      _ground->updateVerticesData(VertexBuffer::NormalKind, normals, false, false);
      _ground2->updateVerticesData(VertexBuffer::NormalKind, normals, false, false);

      _alpha += 0.1f;
    });
  }

private:
  MeshPtr _ground;
  MeshPtr _ground2;
  Float32Array _positions;
  Uint32Array _indices;
  float _alpha;

}; // end of class WavesScene

BABYLON_REGISTER_SAMPLE("Special FX", WavesScene)

} // end of namespace Samples
} // end of namespace BABYLON
