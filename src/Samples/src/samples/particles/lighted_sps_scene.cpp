#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/particles/solid_particle_system.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Lighted SPS Scene. Solid particle system with a light going through it.
 * @see https://playground.babylonjs.com/#2FPT1A#183
 * @see https://doc.babylonjs.com/how_to/solid_particle_system
 */
class LightedSpsScene : public IRenderableScene {

public:
  LightedSpsScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _time{0.f}
      , _nb{static_cast<size_t>(std::pow(65, 3))}
      , _pl{nullptr}
  {
  }

  ~LightedSpsScene() override = default;

  const char* getName() override
  {
    return "Lighted SPS Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {

    scene->clearColor = Color3::Black();
    auto camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3(0.f, 0.f, -0.f), scene);
    camera->setPosition(Vector3(0.f, 50.f, -200.f));
    camera->attachControl(canvas, true);

    _pl            = PointLight::New("pl", Vector3(0.f, 0.f, 0.f), scene);
    _pl->diffuse   = Color3(1.f, 1.f, 1.f);
    _pl->intensity = 0.3f;

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
      = [this](SolidParticle* particle, unsigned int i, unsigned int s) -> void {
      _positionFunction(particle, i, s);
    };
    _sps->addShape(triangle, _nb, spsMeshBuilderOptions);
    _sps->buildMesh();

    // dispose the model
    triangle->dispose();

    // SPS mesh animation
    scene->registerBeforeRender([this](Scene*, EventState&) {
      ++_time;
      _pl->position().x = std::sin(_time / 50.f) * 100.f;
    });
  }

private:
  /**
   * @brief Custom position function for SPS creation.
   */
  void _positionFunction(SolidParticle* particle, unsigned int i, unsigned int /*s*/)
  {
    const auto cbrt      = static_cast<float>(std::cbrt(_nb));
    const auto z         = std::floor(i / (cbrt * cbrt));
    const auto y         = std::floor((i - z * cbrt * cbrt) / cbrt);
    const auto x         = i - cbrt * (y + cbrt * z);
    particle->position.x = x * 0.9f - cbrt / 2.f;
    particle->position.y = y * 0.9f - cbrt / 2.f;
    particle->position.z = z * 0.9f - cbrt / 2.f;
    particle->rotation.x = Math::random() * 3.15f;
    particle->rotation.y = Math::random() * 3.15f;
    particle->rotation.z = Math::random() * 1.5f;
  }

private:
  float _time;
  size_t _nb; // nb of triangles
  PointLightPtr _pl;
  SolidParticleSystemPtr _sps;

}; // end of class LightedSPSScene

BABYLON_REGISTER_SAMPLE("Particles", LightedSpsScene)

} // end of namespace Samples
} // end of namespace BABYLON
