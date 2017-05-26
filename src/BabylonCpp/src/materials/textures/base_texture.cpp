#include <babylon/materials/textures/base_texture.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/material.h>
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

void BaseTexture::releaseInternalTexture()
{
  if (_texture) {
    _scene->getEngine()->releaseInternalTexture(_texture);
    _texture = nullptr;
  }
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

} // end of namespace BABYLON
