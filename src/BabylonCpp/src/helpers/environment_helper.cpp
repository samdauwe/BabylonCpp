#include <babylon/helpers/environment_helper.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/helpers/iscene_size.h>
#include <babylon/materials/background/background_material.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/mirror_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

constexpr const char* EnvironmentHelper::_groundTextureCDNUrl;
constexpr const char* EnvironmentHelper::_skyboxTextureCDNUrl;
constexpr const char* EnvironmentHelper::_environmentTextureCDNUrl;

EnvironmentHelper::EnvironmentHelper(Scene* scene)
    : EnvironmentHelper(EnvironmentHelper::_getDefaultOptions(), scene)

{
}

EnvironmentHelper::EnvironmentHelper(const IEnvironmentHelperOptions& options,
                                     Scene* scene)
    : _scene{scene}, _options{options}
{
  _setupBackground();
  _setupImageProcessing();
}

void EnvironmentHelper::updateOptions(
  const IEnvironmentHelperOptions& /*options*/)
{
}

void EnvironmentHelper::setMainColor(const Color3& color)
{
  if (groundMaterial()) {
    groundMaterial()->setPrimaryColor(color);
  }

  if (skyboxMaterial()) {
    skyboxMaterial()->setPrimaryColor(color);
  }

  if (groundMirror()) {
    groundMirror()->clearColor = Color4(color.r, color.g, color.b, 1.f);
  }
}

Mesh* EnvironmentHelper::rootMesh()
{
  return _rootMesh;
}

Mesh* EnvironmentHelper::skybox()
{
  return _skybox;
}

BaseTexture* EnvironmentHelper::skyboxTexture()
{
  return _skyboxTexture;
}

BackgroundMaterial* EnvironmentHelper::skyboxMaterial()
{
  return _skyboxMaterial;
}

Mesh* EnvironmentHelper::ground()
{
  return _ground;
}

BaseTexture* EnvironmentHelper::groundTexture()
{
  return _groundTexture;
}

MirrorTexture* EnvironmentHelper::groundMirror()
{
  return _groundMirror;
}

vector_t<AbstractMesh*>& EnvironmentHelper::groundMirrorRenderList()
{
  if (_groundMirror) {
    return _groundMirror->renderList;
  }
  return _emptyGroundMirrorRenderList;
}

BackgroundMaterial* EnvironmentHelper::groundMaterial()
{
  return _groundMaterial;
}

IEnvironmentHelperOptions EnvironmentHelper::_getDefaultOptions()
{
  IEnvironmentHelperOptions options;
  options.createGround = true;
  options.groundSize   = 15;
  options.groundTexture.set<string_t>(_groundTextureCDNUrl);
  options.groundColor   = Color3(0.2f, 0.2f, 0.3f).toLinearSpace().scale(3.f);
  options.groundOpacity = 0.9f;
  options.enableGroundShadow = true;
  options.groundShadowLevel  = 0.5f;

  options.enableGroundMirror          = false;
  options.groundMirrorSizeRatio       = 0.3f;
  options.groundMirrorBlurKernel      = 64;
  options.groundMirrorAmount          = 1;
  options.groundMirrorFresnelWeight   = 1;
  options.groundMirrorFallOffDistance = 0;
  options.groundMirrorTextureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT;

  options.createSkybox = true;
  options.skyboxSize   = 20;
  options.skyboxTexture.set<string_t>(_skyboxTextureCDNUrl);
  options.skyboxColor = Color3(0.2f, 0.2f, 0.3f).toLinearSpace().scale(3.f);

  options.backgroundYRotation = 0;
  options.sizeAuto            = true;
  options.rootPosition        = Vector3::Zero();

  options.setupImageProcessing = true;
  options.environmentTexture.set<string_t>(_environmentTextureCDNUrl);
  options.cameraExposure     = 0.8f;
  options.cameraContrast     = 1.2f;
  options.toneMappingEnabled = true;

  return options;
}

void EnvironmentHelper::_setupImageProcessing()
{
  if (_options.setupImageProcessing) {
    _scene->imageProcessingConfiguration()->setContrast(
      _options.cameraContrast);
    _scene->imageProcessingConfiguration()->setExposure(
      _options.cameraExposure);
    _scene->imageProcessingConfiguration()->setToneMappingEnabled(
      _options.toneMappingEnabled);
    _setupEnvironmentTexture();
  }
}

void EnvironmentHelper::_setupEnvironmentTexture()
{
}

void EnvironmentHelper::_setupBackground()
{
  if (!_rootMesh) {
    _rootMesh = Mesh::New("BackgroundHelper", _scene);
  }
  _rootMesh->rotation().y = _options.backgroundYRotation;

  const auto sceneSize = _getSceneSize();
  if (_options.createGround) {
    _setupGround(sceneSize);
    _setupGroundMaterial();
    _setupGroundDiffuseTexture();

    if (_options.enableGroundMirror) {
      // _setupGroundMirrorTexture(sceneSize);
    }
    _setupMirrorInGroundMaterial();
  }

  if (_options.createSkybox) {
    _setupSkybox(sceneSize);
    _setupSkyboxMaterial();
    _setupSkyboxReflectionTexture();
  }

  _rootMesh->position().x = sceneSize.rootPosition.x;
  _rootMesh->position().z = sceneSize.rootPosition.z;
  _rootMesh->position().y = sceneSize.rootPosition.y;
}

ISceneSize EnvironmentHelper::_getSceneSize()
{
  auto groundSize   = _options.groundSize;
  auto skyboxSize   = _options.skyboxSize;
  auto rootPosition = _options.rootPosition;
  if (_scene->meshes.empty() || _scene->meshes.size() == 1) {
    // 1 only means the root of the helper.
    return ISceneSize{groundSize, skyboxSize, rootPosition};
  }

  const auto sceneExtends  = _scene->getWorldExtends();
  const auto sceneDiagonal = sceneExtends.max.subtract(sceneExtends.min);
  auto bias                = 0.0001f;

  if (_options.sizeAuto) {
    if (_scene->activeCamera->type() == IReflect::Type::ARCROTATECAMERA) {
      auto activecamera = static_cast<ArcRotateCamera*>(_scene->activeCamera);
      groundSize = static_cast<int>(activecamera->upperRadiusLimit * 2.f);
    }

    if (_scene->activeCamera) {
      bias
        = (_scene->activeCamera->maxZ - _scene->activeCamera->minZ) / 10000.f;
    }

    const auto sceneDiagonalLenght = sceneDiagonal.length();
    if (sceneDiagonalLenght > groundSize) {
      groundSize = static_cast<int>(sceneDiagonalLenght * 2.f);
    }

    // 10 % bigger.
    groundSize *= 1.1f;
    skyboxSize *= 1.5f;
    rootPosition   = sceneExtends.min.add(sceneDiagonal.scale(0.5f));
    rootPosition.y = sceneExtends.min.y - bias;
  }

  return ISceneSize{groundSize, skyboxSize, rootPosition};
}

void EnvironmentHelper::_setupGround(const ISceneSize& sceneSize)
{
  if (!_ground) {
    _ground
      = Mesh::CreatePlane("BackgroundPlane", sceneSize.groundSize, _scene);
    _ground->rotation().x = Math::PI_2; // Face up by default.
    _ground->setParent(_rootMesh);
    _ground->onDisposeObservable.add(
      [this](Node* /*node*/, EventState& /*es*/) { _ground = nullptr; });
  }

  _ground->setReceiveShadows(_options.enableGroundShadow);
}

void EnvironmentHelper::_setupGroundMaterial()
{
  if (!_groundMaterial) {
    _groundMaterial = new BackgroundMaterial("BackgroundPlaneMaterial", _scene);
  }
  _groundMaterial->alpha = _options.groundOpacity;
  _groundMaterial->setAlphaMode(
    EngineConstants::ALPHA_PREMULTIPLIED_PORTERDUFF);
  _groundMaterial->setShadowLevel(_options.groundShadowLevel);
  _groundMaterial->setPrimaryLevel(1);
  _groundMaterial->setPrimaryColor(_options.groundColor);
  _groundMaterial->setSecondaryLevel(0);
  _groundMaterial->setTertiaryLevel(0);
  _groundMaterial->setUseRGBColor(false);
  _groundMaterial->setEnableNoise(true);

  if (_ground) {
    _ground->setMaterial(_groundMaterial);
  }
}

void EnvironmentHelper::_setupGroundDiffuseTexture()
{
  if (!_groundMaterial) {
    return;
  }

  if (_groundTexture) {
    return;
  }

  if (_options.skyboxTexture.is<BaseTexture*>()) {
    // _groundMaterial->setDiffuseTexture ( _options.groundTexture);
    return;
  }

  const auto diffuseTexture
    = Texture::New(_options.groundTexture.get<string_t>(), _scene);
  diffuseTexture->gammaSpace = false;
  diffuseTexture->setHasAlpha(true);
  // _groundMaterial->setDiffuseTexture(diffuseTexture);
}

void EnvironmentHelper::_setupGroundMirrorTexture(ISceneSize* /*sceneSize*/)
{
}

void EnvironmentHelper::_setupMirrorInGroundMaterial()
{
  if (_groundMaterial) {
    // _groundMaterial->setReflectionTexture(_groundMirror);
    _groundMaterial->setReflectionFresnel(true);
    _groundMaterial->setReflectionAmount(_options.groundMirrorAmount);
    _groundMaterial->setReflectionStandardFresnelWeight(
      _options.groundMirrorFresnelWeight);
    _groundMaterial->setReflectionFalloffDistance(
      _options.groundMirrorFallOffDistance);
  }
}

void EnvironmentHelper::_setupSkybox(const ISceneSize& sceneSize)
{
  if (!_skybox) {
    _skybox = Mesh::CreateBox("BackgroundSkybox", sceneSize.skyboxSize, _scene,
                              false, Mesh::BACKSIDE);
    _skybox->onDisposeObservable.add(
      [this](Node* /*node*/, EventState& /*es*/) { _skybox = nullptr; });
  }
  _skybox->setParent(_rootMesh);
}

void EnvironmentHelper::_setupSkyboxMaterial()
{
  if (!_skybox) {
    return;
  }

  if (!_skyboxMaterial) {
    _skyboxMaterial
      = new BackgroundMaterial("BackgroundSkyboxMaterial", _scene);
  }
  _skyboxMaterial->setUseRGBColor(false);
  _skyboxMaterial->setPrimaryLevel(1);
  _skyboxMaterial->setPrimaryColor(_options.skyboxColor);
  _skyboxMaterial->setSecondaryLevel(0);
  _skyboxMaterial->setTertiaryLevel(0);
  _skyboxMaterial->setEnableNoise(true);

  _skybox->setMaterial(_skyboxMaterial);
}

void EnvironmentHelper::_setupSkyboxReflectionTexture()
{
  if (!_skyboxMaterial) {
    return;
  }

  if (_skyboxTexture) {
    return;
  }

  if (_options.skyboxTexture.is<BaseTexture*>()) {
    _skyboxMaterial->setReflectionTexture(_skyboxTexture);
    return;
  }

  // _skyboxTexture
  //  = new CubeTexture(_options.skyboxTexture.get<string_t>(), _scene);
  _skyboxTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
  _skyboxTexture->gammaSpace      = false;
  _skyboxMaterial->setReflectionTexture(_skyboxTexture);
}

void EnvironmentHelper::dispose()
{
  if (_groundMaterial) {
    _groundMaterial->dispose(true, true);
  }
  if (_skyboxMaterial) {
    _skyboxMaterial->dispose(true, true);
  }
  _rootMesh->dispose(false);
}

} // end of namespace BABYLON
