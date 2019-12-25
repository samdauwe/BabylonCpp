#include <babylon/helpers/photo_dome.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/photo_dome_options.h>
#include <babylon/materials/background/background_material.h>
#include <babylon/materials/push_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

PhotoDome::PhotoDome(std::string iName, const std::string& urlOfPhoto, PhotoDomeOptions options,
                     Scene* scene,
                     const std::function<void(const std::string& message)>& /*onError*/)
    : TransformNode{iName, scene}
    , photoTexture{this, &PhotoDome::get_photoTexture, &PhotoDome::set_photoTexture}
    , mesh{this, &PhotoDome::get_mesh}
    , fovMultiplier{this, &PhotoDome::get_fovMultiplier, &PhotoDome::set_fovMultiplier}
    , imageMode{this, &PhotoDome::get_imageMode, &PhotoDome::set_imageMode}
    , _useDirectMapping{false}
    , _imageMode{PhotoDome::MODE_MONOSCOPIC}
    , _onBeforeCameraRenderObserver{nullptr}
{
  // set defaults and manage values
  name               = !iName.empty() ? iName : "photoDome";
  options.resolution = options.resolution.has_value() ? *options.resolution : 32u;
  options.size
    = options.size.has_value() ?
        *options.size :
        (scene->activeCamera() ? static_cast<unsigned>(scene->activeCamera()->maxZ * 0.48f) :
                                 1000u);

  if (!options.useDirectMapping.has_value()) {
    _useDirectMapping = true;
  }
  else {
    _useDirectMapping = *options.useDirectMapping;
  }

  if (!options.faceForward.has_value()) {
    options.faceForward = true;
  }

  _setReady(false);

  // create
  auto material = _material = BackgroundMaterial::New(name + "_material", scene);
  _mesh = Mesh::CreateSphere(name + "_mesh", *options.resolution, static_cast<float>(*options.size),
                             scene, false, Mesh::BACKSIDE);

  // configure material
  material->opacityFresnel        = false;
  material->useEquirectangularFOV = true;
  material->fovMultiplier         = 1.f;

  photoTexture = Texture::New(urlOfPhoto, scene, true, !_useDirectMapping);
  photoTexture()->anisotropicFilteringLevel = 1;

  photoTexture()->onLoadObservable().addOnce(
    [this](Texture* /*texture*/, EventState& /*es*/) { _setReady(true); });

  // configure mesh
  _mesh->material = material;
  _mesh->parent   = this;

  // Initial rotation
  if (*options.faceForward && scene->activeCamera()) {
    auto& camera = scene->activeCamera();

    auto iForward  = Vector3::Forward();
    auto direction = Vector3::TransformNormal(iForward, camera->getViewMatrix());
    direction.normalize();

    rotation().y = std::acos(Vector3::Dot(iForward, direction));
  }
}

PhotoDome::~PhotoDome() = default;

TexturePtr& PhotoDome::get_photoTexture()
{
  return _photoTexture;
}

void PhotoDome::set_photoTexture(const TexturePtr& value)
{
  if (_photoTexture == value) {
    return;
  }
  _photoTexture = value;
  if (_useDirectMapping) {
    _photoTexture->wrapU      = TextureConstants::CLAMP_ADDRESSMODE;
    _photoTexture->wrapV      = TextureConstants::CLAMP_ADDRESSMODE;
    _material->diffuseTexture = _photoTexture;
  }
  else {
    _photoTexture->coordinatesMode
      = TextureConstants::FIXED_EQUIRECTANGULAR_MIRRORED_MODE; // matches
                                                               // orientation
    _photoTexture->wrapV         = TextureConstants::CLAMP_ADDRESSMODE;
    _material->reflectionTexture = _photoTexture;
  }
}

MeshPtr& PhotoDome::get_mesh()
{
  return _mesh;
}

float PhotoDome::get_fovMultiplier() const
{
  return _material->fovMultiplier();
}

void PhotoDome::set_fovMultiplier(float value)
{
  _material->fovMultiplier = value;
}

unsigned int PhotoDome::get_imageMode() const
{
  return _imageMode;
}

void PhotoDome::set_imageMode(unsigned int value)
{
  if (_imageMode == value) {
    return;
  }

  _changeImageMode(value);
}

void PhotoDome::_changeImageMode(unsigned int value)
{
  _scene->onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);
  _imageMode = value;

  // Default Setup and Reset.
  _photoTexture->uScale  = 1.f;
  _photoTexture->vScale  = 1.f;
  _photoTexture->uOffset = 0.f;
  _photoTexture->vOffset = 0.f;

  if (value == PhotoDome::MODE_SIDEBYSIDE) {
    _photoTexture->uScale         = 0.5f;
    _onBeforeCameraRenderObserver = _scene->onBeforeCameraRenderObservable.add(
      [this](Camera* camera, EventState & /*es*/) -> void {
        _photoTexture->uOffset = camera->isRightCamera ? 0.5f : 0.f;
      });
  }
  else if (value == PhotoDome::MODE_TOPBOTTOM) {
    _photoTexture->vScale         = 0.5f;
    _onBeforeCameraRenderObserver = _scene->onBeforeCameraRenderObservable.add(
      [this](Camera* camera, EventState & /*es*/) -> void {
        _photoTexture->vOffset = camera->isRightCamera ? 0.5f : 0.f;
      });
  }
}

void PhotoDome::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _photoTexture->dispose();
  _mesh->dispose();
  _material->dispose();

  onLoadErrorObservable.clear();

  _scene->onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);

  TransformNode::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
