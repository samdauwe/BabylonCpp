#include <babylon/samples/particles/buildings_sps_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_data_options.h>
#include <babylon/particles/solid_particle_system.h>

namespace BABYLON {
namespace Samples {

BuldingsSPSScene::BuldingsSPSScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _k{0.001f}   // nb of triangles
    , _fact{200.f} // cube size
    , _camera{nullptr}
    , _pl{nullptr}
    , _ground{nullptr}
    , _sps{nullptr}
{
}

BuldingsSPSScene::~BuldingsSPSScene()
{
}

const char* BuldingsSPSScene::getName()
{
  return "Buldings SPS Scene";
}

void BuldingsSPSScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  scene->clearColor = Color3(0.1f, 0.1f, 0.15f);

  // Create a camera
  _camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f,
                                 Vector3(0.f, 0.f, -0.f), scene);
  _camera->setPosition(Vector3(0.f, 5.f, -100.f));
  _camera->attachControl(canvas, true);

  // Create two lights
  auto light = HemisphericLight::New("light1", Vector3(1.f, 0.f, 0.f), scene);
  light->intensity = 0.75f;
  light->specular  = Color3(0.95f, 0.95f, 0.81f);
  _pl              = PointLight::New("pl", Vector3(0.f, 0.f, 0.f), scene);
  _pl->diffuse     = Color3(1.f, 1.f, 1.f);
  _pl->specular    = Color3(0.1f, 0.1f, 0.12f);
  _pl->intensity   = 0.75f;

  // Skybox
  auto skybox                     = Mesh::CreateBox("skyBox", 250.f, scene);
  auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->backFaceCulling = false;
  skyboxMaterial->reflectionTexture
    = CubeTexture::New("textures/skybox/skybox", scene);
  skyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  skyboxMaterial->diffuseColor    = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->specularColor   = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->disableLighting = true;
  skybox->material                = skyboxMaterial;

  // Fog
  scene->fogMode    = Scene::FOGMODE_LINEAR;
  scene->fogColor   = Color3(0.9f, 0.9f, 0.85f);
  scene->fogDensity = 0.01f;

  // Only if LINEAR
  scene->fogStart = 50.f;
  scene->fogEnd   = 120.f;

  auto nbBuildings = 10000ull;

  PlaneOptions planeOptions(_fact);
  _ground                 = MeshBuilder::CreatePlane("g", planeOptions, scene);
  _ground->rotation().x   = Math::PI_2;
  auto groundMat          = StandardMaterial::New("gm", scene);
  groundMat->diffuseColor = Color3(0.4f, 0.4f, 0.4f);
  _ground->material       = groundMat;
  groundMat->backFaceCulling = false;

  const std::string url = "textures/glassbuilding.jpg";
  auto mat              = StandardMaterial::New("mat1", scene);
  auto texture          = Texture::New(url, scene);
  mat->diffuseTexture   = texture;

  // Particle system creation : Immutable
  SolidParticleSystemOptions spsOptions;
  spsOptions.updatable = false;
  _sps                 = SolidParticleSystem::New("SPS", scene, spsOptions);
  BoxOptions boxOptions;
  auto model = MeshBuilder::CreateBox("m", boxOptions, scene);
  SolidParticleSystemMeshBuilderOptions spsMeshBuilderOptions;
  spsMeshBuilderOptions.positionFunction
    = [this](SolidParticle* particle, unsigned int i, unsigned int s) -> void {
    _positionFunction(particle, i, s);
  };
  _sps->addShape(model, nbBuildings, spsMeshBuilderOptions);
  auto mesh      = _sps->buildMesh();
  mesh->material = mat;
  // dispose the model
  model->dispose();

  // SPS mesh animation
  scene->registerBeforeRender([this](Scene*, EventState&) {
    _pl->position = _camera->position();
    _sps->mesh->rotation().y += _k;
    _ground->rotation().y = _sps->mesh->rotation().y;
  });
}

void BuldingsSPSScene::_positionFunction(SolidParticle* particle,
                                         unsigned int /*i*/, unsigned int /*s*/)
{
  auto scaleX = Math::random() * 2.f + 0.8f;
  auto scaleY = Math::random() * Math::random() * Math::random() * 6.f + 0.8f;
  auto scaleZ = Math::random() * 2.f + 0.8f;
  auto uvSize = Math::random() * 0.9f;
  particle->scaling.x  = scaleX;
  particle->scaling.y  = scaleY;
  particle->scaling.z  = scaleZ;
  particle->position.x = (Math::random() - 0.5f) * _fact;
  particle->position.y = particle->scaling.y / 2.f + 0.01f;
  particle->position.z = (Math::random() - 0.5f) * _fact;
  auto grey            = 1.f - Math::random() * 0.5f;
  auto grey2           = 1.f - Math::random() * 0.5f;
  auto grey3           = 1.f - Math::random() * 0.5f;
  particle->color      = Color4(grey + 0.1f, grey2 + 0.1f, grey3, 1.f);
  particle->uvs.x      = 0.f;
  particle->uvs.y      = 0.f;
  particle->uvs.z      = particle->uvs.x + uvSize;
  particle->uvs.w      = particle->uvs.y + uvSize;
}

} // end of namespace Samples
} // end of namespace BABYLON
