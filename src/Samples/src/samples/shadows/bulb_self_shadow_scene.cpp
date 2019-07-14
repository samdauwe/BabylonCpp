#include <babylon/samples/shadows/bulb_self_shadow_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>

namespace BABYLON {
namespace Samples {

BulbSelfShadowScene::BulbSelfShadowScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _i{0.f}
    , _lightPos{Vector3(0.f, 8.f, 0.f)}
    , _lightDiffuse{Color3(1.f, 1.f, 1.f)}
    , _light0{nullptr}
    , _light1{nullptr}
    , _torus{nullptr}
    , _bulb{nullptr}
{
}

BulbSelfShadowScene::~BulbSelfShadowScene()
{
}

const char* BulbSelfShadowScene::getName()
{
  return "Bulf & Self Shadow Scene";
}

void BulbSelfShadowScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // mats
  auto matEmit             = StandardMaterial::New("matEmit", scene);
  matEmit->emissiveColor   = _lightDiffuse;
  matEmit->disableLighting = true;

  auto matMetal       = PBRMetallicRoughnessMaterial::New("matMetal", scene);
  matMetal->metallic  = 0.9f;
  matMetal->roughness = 0.1f;
  matMetal->baseColor = Color3(1.f, 1.f, 1.f);

  auto matWall       = PBRMetallicRoughnessMaterial::New("matWall", scene);
  matWall->metallic  = 0.f;
  matWall->roughness = 1.f;
  matWall->baseColor = Color3(0.9f, 0.1f, 0.1f);

  // meshes
  TorusKnotOptions torusKnotOptions;
  torusKnotOptions.radialSegments  = 200;
  torusKnotOptions.tubularSegments = 50;
  torusKnotOptions.sideOrientation = Mesh::FRONTSIDE;
  _torus = MeshBuilder::CreateTorusKnot("torus", torusKnotOptions, scene);
  _torus->position().y = 3.3f;
  _torus->material     = matMetal;

  SphereOptions bulbOptions;
  bulbOptions.diameter = 2.f;
  _bulb                = MeshBuilder::CreateSphere("bulb", bulbOptions, scene);
  _bulb->position      = _lightPos;
  _bulb->material      = matEmit;

  BoxOptions roomOptions;
  roomOptions.width           = 20.f;
  roomOptions.height          = 20.f;
  roomOptions.depth           = 20.f;
  roomOptions.sideOrientation = Mesh::BACKSIDE;
  auto room      = MeshBuilder::CreateBox("room", roomOptions, scene);
  room->material = matWall;

  BoxOptions groundOptions;
  groundOptions.width  = 6.f;
  groundOptions.height = 10.f;
  groundOptions.depth  = 6.f;
  auto ground          = MeshBuilder::CreateBox("ground", groundOptions, scene);
  ground->material     = matWall;
  ground->position().y = -5.f;

  //
  // "pointlight" does not cast self-shadow but "spotlight" does
  //
  // spotlight that only lits the torus
  {
    _light0            = SpotLight::New("light0", _lightPos,
                             _torus->position().subtract(_lightPos),
                             Math::PI * 2.f / 3.f, 0.f, scene);
    _light0->diffuse   = _lightDiffuse;
    _light0->intensity = 20.f;
    _light0->includedOnlyMeshes().emplace_back(_torus); // <<<<<<<<<<<<<<<<<<<<<
    auto shadowGen = ShadowGenerator::New(2048, _light0);
    shadowGen->useCloseExponentialShadowMap = true;
    shadowGen->addShadowCaster(_torus); // caster
    _torus->receiveShadows = true;      // receiver
    shadowGen->setDarkness(0.f);
  }
  // pointlight that lits all meshes excepts the bulb
  {
    _light1            = PointLight::New("light1", _lightPos, scene);
    _light1->diffuse   = _lightDiffuse;
    _light1->intensity = 200.f;
    _light1->excludedMeshes().emplace_back(_bulb); // <<<<<<<<<<<<<<<<<<<<<<<<<<
    auto shadowGen  = ShadowGenerator::New(1024, _light1);
    shadowGen->bias = 0.0005f;
    shadowGen->usePercentageCloserFiltering = true;
    shadowGen->setDarkness(0.3f);
    shadowGen->addShadowCaster(_torus); // caster
    shadowGen->addShadowCaster(ground);
    _torus->receiveShadows = true; // receiver
    room->receiveShadows   = true; // receiver
    ground->receiveShadows = true; // receiver
  }

  // camera
  auto camera
    = ArcRotateCamera::New("camera", Math::PI / 3.f, Math::PI * 3.f / 5.f, 9.f,
                           _torus->position(), scene, true);
  camera->attachControl(canvas, true);
  camera->upperBetaLimit   = Math::PI * 3.f / 4.f;
  camera->wheelPrecision   = 10.f;
  camera->upperRadiusLimit = 11.f;
  camera->lowerRadiusLimit = 5.f;

  // update
  scene->registerBeforeRender([this](Scene*, EventState&) {
    _i += 0.008f;

    _lightPos.x = -std::cos(_i) * 8.f;
    _lightPos.z = std::sin(_i) * 8.f;

    _light0->position  = _lightPos;
    _light0->direction = _torus->position().subtract(_lightPos);

    _light1->position = _lightPos;

    _bulb->position = _lightPos;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
