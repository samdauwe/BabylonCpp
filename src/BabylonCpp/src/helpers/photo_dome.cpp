#include <babylon/helpers/photo_dome.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/helpers/photo_dome_options.h>
#include <babylon/materials/background/background_material.h>
#include <babylon/materials/push_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

PhotoDome::PhotoDome(string_t iName, const string_t& urlOfPhoto,
                     PhotoDomeOptions options, Scene* scene)
    : Node{iName, scene}
    , photoTexture{this, &PhotoDome::get_photoTexture,
                   &PhotoDome::set_photoTexture}
    , fovMultiplier{this, &PhotoDome::get_fovMultiplier,
                    &PhotoDome::set_fovMultiplier}
    , _useDirectMapping{false}
{
  // set defaults and manage values
  name               = !iName.empty() ? iName : "photoDome";
  options.resolution = options.resolution ? *options.resolution : 32;
  options.size       = options.size ?
                   *options.size :
                   (scene->activeCamera ?
                      static_cast<unsigned>(scene->activeCamera->maxZ * 0.48f) :
                      1000u);

  if (!options.useDirectMapping.has_value()) {
    _useDirectMapping = true;
  }
  else {
    _useDirectMapping = *options.useDirectMapping;
  }

  _setReady(false);

  // create
  auto material = _material
    = BackgroundMaterial::New(name + "_material", scene);
  _mesh = Mesh::CreateSphere(name + "_mesh", *options.resolution, *options.size,
                             scene, false, Mesh::BACKSIDE());

  // configure material
  material->setOpacityFresnel(false);
  material->useEquirectangularFOV = true;
  material->setFovMultiplier(1.f);

  photoTexture = Texture::New(urlOfPhoto, scene, true, !_useDirectMapping);

  photoTexture()->onLoadObservable().addOnce(
    [this](Texture* /*texture*/, EventState& /*es*/) { _setReady(true); });

  // configure mesh
  _mesh->material = material;
  _mesh->parent   = this;
}

PhotoDome::~PhotoDome()
{
}

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
    _photoTexture->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
    _photoTexture->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
    // _material->setDiffuseTexture(_photoTexture);
  }
  else {
    _photoTexture->coordinatesMode
      = TextureConstants::FIXED_EQUIRECTANGULAR_MIRRORED_MODE; // matches
                                                               // orientation
    _photoTexture->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
    // _material->setReflectionTexture(_photoTexture);
  }
}

float PhotoDome::get_fovMultiplier() const
{
  return _material->fovMultiplier();
}

void PhotoDome::set_fovMultiplier(float value)
{
  _material->setFovMultiplier(value);
}

void PhotoDome::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _photoTexture->dispose();
  _mesh->dispose();
  _material->dispose();

  Node::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
