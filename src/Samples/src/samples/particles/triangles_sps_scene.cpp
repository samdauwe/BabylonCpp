#include <babylon/samples/particles/triangles_sps_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_data_options.h>
#include <babylon/particles/solid_particle_system.h>

namespace BABYLON {
namespace Samples {

TrianglesSPSScene::TrianglesSPSScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _nb{60000} // nb of triangles
    , _fact{100}  // cube size
    , _enableHemisphericLight{false}
    , _camera{nullptr}
    , _pl{nullptr}
    , _sps{nullptr}
{
}

TrianglesSPSScene::~TrianglesSPSScene()
{
}

const char* TrianglesSPSScene::getName()
{
  return "Triangles SPS Scene";
}

void TrianglesSPSScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  scene->clearColor = Color3::Black();
  _camera           = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f,
                                 Vector3(0.f, 0.f, -0.f), scene);
  _camera->setPosition(Vector3(0.f, 50.f, -200.f));
  _camera->attachControl(canvas, true);

  if (_enableHemisphericLight) {
    auto light = HemisphericLight::New("light1", Vector3(1.f, 0.f, 0.f), scene);
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
  auto triangle = MeshBuilder::CreateDisc("t", discOptions, scene);

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
    _pl->position = _camera->position;
    _sps->mesh->rotation().y += 0.01f;
    // _sps->mesh->rotation().z += 0.005f;
  });
}

void TrianglesSPSScene::_positionFunction(SolidParticle* particle,
                                          unsigned int /*i*/,
                                          unsigned int /*s*/)
{
  particle->position.x = (Math::random() - 0.5f) * _fact;
  particle->position.y = (Math::random() - 0.5f) * _fact;
  particle->position.z = (Math::random() - 0.5f) * _fact;
  particle->rotation.x = Math::random() * 3.15f;
  particle->rotation.y = Math::random() * 3.15f;
  particle->rotation.z = Math::random() * 1.5f;
  particle->color      = Color4(particle->position.x / _fact + 0.5f,
                           particle->position.y / _fact + 0.5f,
                           particle->position.z / _fact + 0.5f, 1.f);
}

} // end of namespace Samples
} // end of namespace BABYLON
