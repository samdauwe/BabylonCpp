#include <babylon/materials/textures/base_texture.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/tools/hdr/cube_map_to_spherical_polynomial_tools.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

unsigned int BaseTexture::DEFAULT_ANISOTROPIC_FILTERING_LEVEL = 4;

BaseTexture::BaseTexture(Scene* scene)
    : getAlphaFromRGB{false}
    , level{1.f}
    , coordinatesIndex{0}
    , wrapU{TextureConstants::WRAP_ADDRESSMODE}
    , wrapV{TextureConstants::WRAP_ADDRESSMODE}
    , anisotropicFilteringLevel{BaseTexture::
                                  DEFAULT_ANISOTROPIC_FILTERING_LEVEL}
    , isCube{false}
    , gammaSpace{true}
    , invertZ{false}
    , lodLevelInAlpha{false}
    , lodGenerationOffset{0.f}
    , lodGenerationScale{0.8f}
    , isRenderTarget{false}
    , delayLoadState{EngineConstants::DELAYLOADSTATE_NONE}
    , _cachedAnisotropicFilteringLevel{0}
    , _texture{nullptr}
    , _hasAlpha{false}
    , _coordinatesMode{TextureConstants::EXPLICIT_MODE}
    , _scene{scene}
    , _onDisposeObserver{nullptr}
{
}

BaseTexture::~BaseTexture()
{
}

IReflect::Type BaseTexture::type() const
{
  return IReflect::Type::BASETEXTURE;
}

void BaseTexture::addToScene(std::unique_ptr<BaseTexture>&& newTexture)
{
  _scene->textures.emplace_back(std::move(newTexture));
}

bool BaseTexture::hasAlpha() const
{
  return _hasAlpha;
}

void BaseTexture::setHasAlpha(bool value)
{
  if (_hasAlpha == value) {
    return;
  }
  _hasAlpha = value;
  _scene->markAllMaterialsAsDirty(Material::TextureDirtyFlag);
}

unsigned int BaseTexture::coordinatesMode() const
{
  return _coordinatesMode;
}

void BaseTexture::setCoordinatesMode(unsigned int value)
{
  if (_coordinatesMode == value) {
    return;
  }
  _coordinatesMode = value;
  _scene->markAllMaterialsAsDirty(Material::TextureDirtyFlag);
}

std::string BaseTexture::uid()
{
  if (_uid.empty()) {
    _uid = Tools::RandomId();
  }
  return _uid;
}

std::string BaseTexture::toString() const
{
  return name;
}

void BaseTexture::setOnDispose(const std::function<void()>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

bool BaseTexture::isBlocking() const
{
  return true;
}

Scene* BaseTexture::getScene() const
{
  return _scene;
}

Matrix* BaseTexture::getTextureMatrix()
{
  return nullptr;
}

Matrix* BaseTexture::getReflectionTextureMatrix()
{
  return nullptr;
}

GL::IGLTexture* BaseTexture::getInternalTexture()
{
  return _texture;
}

bool BaseTexture::isReadyOrNotBlocking()
{
  return !isBlocking() || isReady();
}

bool BaseTexture::isReady()
{
  if (delayLoadState == EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    delayLoad();
    return false;
  }

  if (_texture) {
    return _texture->isReady;
  }

  return false;
}

ISize BaseTexture::getSize() const
{
  if (_texture->_width) {
    return Size(_texture->_width, _texture->_height);
  }

  if (_texture->_size) {
    return Size(_texture->_size, _texture->_size);
  }

  return Size::Zero();
}

ISize BaseTexture::getBaseSize()
{
  if (!isReady() || !_texture) {
    return Size::Zero();
  }

  if (_texture->_size) {
    return Size(_texture->_size, _texture->_size);
  }

  return Size(_texture->_baseWidth, _texture->_baseHeight);
}

void BaseTexture::scale(float /*ratio*/)
{
}

bool BaseTexture::canRescale()
{
  return false;
}

void BaseTexture::_removeFromCache(const std::string& url, bool noMipmap)
{
  auto& texturesCache = _scene->getEngine()->getLoadedTexturesCache();
  for (auto& texturesCacheEntry : texturesCache) {
    if ((texturesCacheEntry->url.compare(url) == 0)
        && texturesCacheEntry->noMipmap == noMipmap) {
      auto it = std::find(texturesCache.begin(), texturesCache.end(),
                          texturesCacheEntry);
      if (it != texturesCache.end()) {
        texturesCache.erase(it);
        return;
      }
    }
  }
}

GL::IGLTexture* BaseTexture::_getFromCache(const std::string& url,
                                           bool noMipmap, unsigned int sampling)
{
  auto& texturesCache = _scene->getEngine()->getLoadedTexturesCache();
  for (auto& texturesCacheEntry : texturesCache) {
    if ((texturesCacheEntry->url.compare(url) == 0)
        && texturesCacheEntry->noMipmap == noMipmap) {
      if (!sampling || sampling == texturesCacheEntry->samplingMode) {
        ++texturesCacheEntry->references;
        return texturesCacheEntry.get();
      }
    }
  }

  return nullptr;
}

void BaseTexture::delayLoad()
{
}

std::vector<Animation*> BaseTexture::getAnimations()
{
  return animations;
}

std::unique_ptr<BaseTexture> BaseTexture::clone() const
{
  return nullptr;
}

unsigned int BaseTexture::textureType() const
{
  if (!_texture) {
    return EngineConstants::TEXTURETYPE_UNSIGNED_INT;
  }

  return _texture->type ? *_texture->type :
                          EngineConstants::TEXTURETYPE_UNSIGNED_INT;
}

unsigned int BaseTexture::textureFormat() const
{
  if (!_texture) {
    return EngineConstants::TEXTUREFORMAT_RGBA;
  }

  return _texture->format ? *_texture->format :
                            EngineConstants::TEXTUREFORMAT_RGBA;
}

Uint8Array BaseTexture::readPixels(unsigned int faceIndex)
{
  if (!_texture) {
    return {};
  }

  auto size   = getSize();
  auto engine = getScene()->getEngine();

  if (_texture->isCube) {
    return engine->_readTexturePixels(_texture, size.width, size.height,
                                      static_cast<int>(faceIndex));
  }

  return engine->_readTexturePixels(_texture, size.width, size.height, -1);
}

void BaseTexture::releaseInternalTexture()
{
  if (_texture) {
    _scene->getEngine()->releaseInternalTexture(_texture);
    _texture = nullptr;
  }
}

SphericalPolynomial* BaseTexture::sphericalPolynomial()
{
#if 0
  if (!_texture || !isReady()) {
    return nullptr;
  }

  if (!_texture->_sphericalPolynomial) {
    _texture->_sphericalPolynomial
      = Internals::CubeMapToSphericalPolynomialTools::
        ConvertCubeMapTextureToSphericalPolynomial(this);
  }

  return _texture->_sphericalPolynomial.get();
#else
  return nullptr;
#endif
}

void BaseTexture::setSphericalPolynomial(const SphericalPolynomial& /*value*/)
{
#if 0
  if (_texture) {
    _texture->_sphericalPolynomial
      = std::make_unique<SphericalPolynomial>(value);
  }
#endif
}

BaseTexture* BaseTexture::_lodTextureHigh() const
{
  if (_texture) {
    return _texture->_lodTextureHigh;
  }
  return nullptr;
}

BaseTexture* BaseTexture::_lodTextureMid() const
{
  if (_texture) {
    return _texture->_lodTextureMid;
  }
  return nullptr;
}

BaseTexture* BaseTexture::_lodTextureLow() const
{
  if (_texture) {
    return _texture->_lodTextureLow;
  }
  return nullptr;
}

void BaseTexture::dispose(bool /*doNotRecurse*/)
{
  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  _scene->textures.erase(
    std::remove_if(_scene->textures.begin(), _scene->textures.end(),
                   [this](const std::unique_ptr<BaseTexture>& baseTexture) {
                     return baseTexture.get() == this;
                   }),
    _scene->textures.end());

  if (_texture == nullptr) {
    return;
  }

  // Release
  releaseInternalTexture();

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

Json::object BaseTexture::serialize() const
{
  return Json::object();
}

void BaseTexture::WhenAllReady(const std::vector<BaseTexture*>& textures,
                               const std::function<void()>& onLoad)
{
  static std::size_t numReady = 0;

  for (auto& texture : textures) {
    if (texture->isReady()) {
      if (++numReady == textures.size()) {
        onLoad();
      }
    }
    else {
      const std::function<void()> callback = [&]() {
        static_cast<Texture*>(texture)->onLoadObservable().removeCallback(
          callback);
        if (++numReady == textures.size()) {
          onLoad();
        }
      };

      static_cast<Texture*>(texture)->onLoadObservable().add(callback);
    }
  }
}

} // end of namespace BABYLON
