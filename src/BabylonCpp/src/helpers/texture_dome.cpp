#include <babylon/helpers/texture_dome.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/texture_dome_options.h>
#include <babylon/materials/background/background_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

TextureDome::TextureDome(
  const std::string& iName, const std::string& /*textureUrlOrElement*/,
  TextureDomeOptions& /*options*/, Scene* scene,
  const std::function<void(const std::string& message, const std::string& exception)>& iOnError)
    : TransformNode{iName, scene}
    , texture{this, &TextureDome::get_texture, &TextureDome::set_texture}
    , mesh{this, &TextureDome::get_mesh}
    , fovMultiplier{this, &TextureDome::get_fovMultiplier, &TextureDome::set_fovMultiplier}
    , textureMode{this, &TextureDome::get_textureMode, &TextureDome::set_textureMode}
    , halfDome{this, &TextureDome::get_halfDome, &TextureDome::set_halfDome}
    , crossEye{this, &TextureDome::get_crossEye, &TextureDome::set_crossEye}
    , material{this, &TextureDome::get_material}
    , _useDirectMapping{false}
    , _texture{nullptr}
    , _material{nullptr}
    , _mesh{nullptr}
    , _textureMode{TextureDome::MODE_MONOSCOPIC}
    , onError{iOnError}
    , _halfDome{false}
    , _crossEye{false}
    , _halfDomeMask{nullptr}
    , _onBeforeCameraRenderObserver{nullptr}
{
}

TextureDome::~TextureDome() = default;

void TextureDome::initializeTextureDome(const std::string& iName,
                                        const std::string& textureUrlOrElement,
                                        TextureDomeOptions& options, Scene* scene)
{
  scene = getScene();

  // set defaults and manage values
  name                = !iName.empty() ? iName : "textureDome";
  options.resolution  = options.resolution.value_or(32u);
  options.clickToPlay = options.clickToPlay.value_or(false);
  options.autoPlay    = options.autoPlay.value_or(true);
  options.loop        = options.loop.value_or(true);
  options.size
    = options.size.value_or(scene->activeCamera() ? scene->activeCamera()->maxZ * 0.48f : 1000.f);

  if (!options.useDirectMapping) {
    _useDirectMapping = true;
  }
  else {
    _useDirectMapping = *options.useDirectMapping;
  }

  if (!options.faceForward) {
    options.faceForward = true;
  }

  _setReady(false);
  _mesh = Mesh::CreateSphere(name + "_mesh", *options.resolution, *options.size, scene, false,
                             Mesh::BACKSIDE);
  // configure material
  auto backgroundMaterial = (_material = BackgroundMaterial::New(name + "_material", scene));
  backgroundMaterial->useEquirectangularFOV = true;
  backgroundMaterial->fovMultiplier         = 1.f;
  backgroundMaterial->opacityFresnel        = false;

  const auto iTexture = _initTexture(textureUrlOrElement, scene, options);
  texture             = iTexture;

  // configure mesh
  _mesh->material = backgroundMaterial;
  _mesh->parent   = this;

  // create a (disabled until needed) mask to cover unneeded segments of 180 texture.
  SphereOptions halfDomeMaskOptions;
  halfDomeMaskOptions.slice           = 0.5f;
  halfDomeMaskOptions.diameter        = *options.size * 0.98f;
  halfDomeMaskOptions.segments        = *options.resolution * 2u;
  halfDomeMaskOptions.sideOrientation = Mesh::BACKSIDE;
  _halfDomeMask                       = SphereBuilder::CreateSphere("", halfDomeMaskOptions, scene);
  _halfDomeMask->rotate(Axis::X(), -Math::PI_2);
  // set the parent, so it will always be positioned correctly AND will be disposed when the main
  // sphere is disposed
  _halfDomeMask->parent = _mesh.get();
  _halfDome             = options.halfDomeMode.value_or(false);
  // enable or disable according to the settings
  _halfDomeMask->setEnabled(_halfDome);
  _crossEye = options.crossEyeMode.value_or(false);

  // create
  _texture->anisotropicFilteringLevel = 1;
  _texture->onLoadObservable().addOnce(
    [this](Texture* /*textre*/, EventState& /*es*/) -> void { _setReady(true); });

  // Initial rotation
  if (options.faceForward.value_or(false) && scene->activeCamera()) {
    const auto camera = scene->activeCamera();

    auto iForward  = Vector3::Forward();
    auto direction = Vector3::TransformNormal(iForward, camera->getViewMatrix());
    direction.normalize();

    rotation().y = std::acos(Vector3::Dot(iForward, direction));
  }

  _changeTextureMode(_textureMode);
}

TexturePtr& TextureDome::get_texture()
{
  return _texture;
}

void TextureDome::set_texture(const TexturePtr& newTexture)
{
  if (_texture == newTexture) {
    return;
  }
  _texture = newTexture;
  if (_useDirectMapping) {
    _texture->wrapU           = TextureConstants::CLAMP_ADDRESSMODE;
    _texture->wrapV           = TextureConstants::CLAMP_ADDRESSMODE;
    _material->diffuseTexture = _texture;
  }
  else {
    _texture->coordinatesMode
      = TextureConstants::FIXED_EQUIRECTANGULAR_MIRRORED_MODE; // matches orientation
    _texture->wrapV              = TextureConstants::CLAMP_ADDRESSMODE;
    _material->reflectionTexture = _texture;
  }
  _changeTextureMode(_textureMode);
}

MeshPtr& TextureDome::get_mesh()
{
  return _mesh;
}

float TextureDome::get_fovMultiplier() const
{
  return _material->fovMultiplier();
}

void TextureDome::set_fovMultiplier(float value)
{
  _material->fovMultiplier = value;
}

unsigned int TextureDome::get_textureMode() const
{
  return _textureMode;
}

void TextureDome::set_textureMode(unsigned int value)
{
  if (_textureMode == value) {
    return;
  }

  _changeTextureMode(value);
}

bool TextureDome::get_halfDome() const
{
  return _halfDome;
}

void TextureDome::set_halfDome(bool enabled)
{
  _halfDome = enabled;
  _halfDomeMask->setEnabled(enabled);
}

void TextureDome::set_crossEye(bool enabled)
{
  _crossEye = enabled;
}

bool TextureDome::get_crossEye() const
{
  return _crossEye;
}

BackgroundMaterialPtr& TextureDome::get_material()
{
  return _material;
}

TexturePtr TextureDome::_initTexture(const std::string& /*urlsOrElement*/, Scene* /*scene*/,
                                     const TextureDomeOptions& /*options*/)
{
  return nullptr;
}

void TextureDome::_changeTextureMode(unsigned int value)
{
  _scene->onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);
  _textureMode = value;

  // Default Setup and Reset.
  _texture->uScale  = 1.f;
  _texture->vScale  = 1.f;
  _texture->uOffset = 0.f;
  _texture->vOffset = 0.f;
  _texture->vAng    = 0.f;

  switch (value) {
    case TextureDome::MODE_MONOSCOPIC: {
      if (_halfDome) {
        _texture->uScale  = 2.f;
        _texture->uOffset = -1.f;
      }
    } break;
    case TextureDome::MODE_SIDEBYSIDE: {
      // in half-dome mode the uScale should be double of 360 texture
      // Use 0.99999 to boost perf by not switching program
      _texture->uScale              = _halfDome ? 0.99999f : 0.5f;
      _onBeforeCameraRenderObserver = _scene->onBeforeCameraRenderObservable.add(
        [this](Camera* camera, EventState& /*es*/) -> void {
          const auto rightOffset = _halfDome ? 0.f : 0.5f;
          const auto leftOffset  = _halfDome ? -0.5f : 0.f;
          auto isRightCamera     = camera->isRightCamera();
          if (_crossEye) {
            isRightCamera = !isRightCamera;
          }
          if (isRightCamera) {
            _texture->uOffset = rightOffset;
          }
          else {
            _texture->uOffset = leftOffset;
          }
        });
    } break;
    case TextureDome::MODE_TOPBOTTOM: {
      // in half-dome mode the vScale should be double of 360 texture
      // Use 0.99999 to boost perf by not switching program
      _texture->vScale              = _halfDome ? 0.99999f : 0.5f;
      _onBeforeCameraRenderObserver = _scene->onBeforeCameraRenderObservable.add(
        [this](Camera* camera, EventState& /*es*/) -> void {
          auto isRightCamera = camera->isRightCamera();
          // allow "cross-eye" if left and right were switched in this mode
          if (_crossEye) {
            isRightCamera = !isRightCamera;
          }
          _texture->vOffset = camera->isRightCamera() ? 0.5f : 0.f;
        });
    } break;
  }
}

void TextureDome::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _texture->dispose();
  _mesh->dispose();
  _material->dispose();

  _scene->onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);
  onLoadErrorObservable.clear();

  TransformNode::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
