#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/particles/solid_particle_system.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {

class ArcRotateCamera;
class PointLight;
class SolidParticle;
class SolidParticleSystem;
using ArcRotateCameraPtr     = std::shared_ptr<ArcRotateCamera>;
using PointLightPtr          = std::shared_ptr<PointLight>;
using SolidParticleSystemPtr = std::shared_ptr<SolidParticleSystem>;

namespace Samples {

/**
 * @brief Triangles SPS Scene. A lot of triangles with SPS. Example demonstrating how to use solid
 * particle system to create a colorful cube.
 * @see https://www.babylonjs-playground.com/#2FPT1A#5
 * @see https://doc.babylonjs.com/how_to/solid_particle_system
 */
class TrianglesSpsScene : public IRenderableScene {

public:
  TrianglesSpsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _nb{60000} // nb of triangles
      , _fact{100} // cube size
      , _enableHemisphericLight{false}
      , _camera{nullptr}
      , _pl{nullptr}
      , _sps{nullptr}
  {
  }

  ~TrianglesSpsScene() override = default;

  const char* getName() override
  {
    return "Triangles SPS Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    scene->clearColor = Color3::Black();
    _camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3(0.f, 0.f, -0.f), scene);
    _camera->setPosition(Vector3(0.f, 50.f, -200.f));
    _camera->attachControl(canvas, true);

    if (_enableHemisphericLight) {
      auto light       = HemisphericLight::New("light1", Vector3(1.f, 0.f, 0.f), scene);
      light->intensity = 0.85f;
      light->specular  = Color3(0.95f, 0.95f, 0.81f);
    }

    _pl            = PointLight::New("pl", Vector3(0.f, 0.f, 0.f), scene);
    _pl->diffuse   = Color3(1.f, 1.f, 1.f);
    _pl->intensity = 1.f;

    // model : triangle
    DiscOptions discOptions;
    discOptions.tessellation    = 3;
    discOptions.sideOrientation = Mesh::DOUBLESIDE;
    auto triangle               = MeshBuilder::CreateDisc("t", discOptions, scene);

    // SPS creation : Immutable {updatable: false}
    SolidParticleSystemOptions spsOptions;
    spsOptions.updatable = false;
    _sps                 = SolidParticleSystem::New("SPS", scene, spsOptions);
    SolidParticleSystemMeshBuilderOptions spsMeshBuilderOptions;
    spsMeshBuilderOptions.positionFunction
      = [this](SolidParticle* particle, size_t i, size_t s) -> void {
      _positionFunction(particle, i, s);
    };
    _sps->addShape(triangle, _nb, spsMeshBuilderOptions);
    _sps->buildMesh();

    // dispose the model
    triangle->dispose();

    // SPS mesh animation
    scene->registerBeforeRender([this](Scene*, EventState&) {
      _pl->position = _camera->position;
      _sps->mesh->rotation().y += 0.01f;
      // _sps->mesh->rotation().z += 0.005f;
    });
  }

private:
  /**
   * @brief Custom position function for SPS creation.
   */
  void _positionFunction(SolidParticle* particle, size_t /*i*/, size_t /*s*/)
  {
    particle->position.x = (Math::random() - 0.5f) * _fact;
    particle->position.y = (Math::random() - 0.5f) * _fact;
    particle->position.z = (Math::random() - 0.5f) * _fact;
    particle->rotation.x = Math::random() * 3.15f;
    particle->rotation.y = Math::random() * 3.15f;
    particle->rotation.z = Math::random() * 1.5f;
    particle->color
      = Color4(particle->position.x / _fact + 0.5f, particle->position.y / _fact + 0.5f,
               particle->position.z / _fact + 0.5f, 1.f);
  }

private:
  size_t _nb;   // nb of triangles
  size_t _fact; // cube size
  bool _enableHemisphericLight;
  ArcRotateCameraPtr _camera;
  PointLightPtr _pl;
  SolidParticleSystemPtr _sps;

}; // end of class TrianglesSPSScene

BABYLON_REGISTER_SAMPLE("Particles", TrianglesSpsScene)

} // end of namespace Samples
} // end of namespace BABYLON
