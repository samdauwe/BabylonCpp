#include <babylon/materials/textures/base_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/engine_store.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/spherical_polynomial.h>
#include <babylon/misc/guid.h>
#include <babylon/misc/highdynamicrange/cube_map_to_spherical_polynomial_tools.h>

namespace BABYLON {

BaseTexture::BaseTexture(const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine)
    : ThinTexture{nullptr}
    , hasAlpha{this, &BaseTexture::get_hasAlpha, &BaseTexture::set_hasAlpha}
    , getAlphaFromRGB{false}
    , level{1.f}
    , coordinatesIndex{0}
    , anisotropicFilteringLevel{BaseTexture::DEFAULT_ANISOTROPIC_FILTERING_LEVEL}
    , gammaSpace{this, &BaseTexture::get_gammaSpace, &BaseTexture::set_gammaSpace}
    , isRGBD{this, &BaseTexture::get_isRGBD, &BaseTexture::set_isRGBD}
    , invertZ{false}
    , noMipmap{this, &BaseTexture::get_noMipmap}
    , lodLevelInAlpha{false}
    , lodGenerationOffset{this, &BaseTexture::get_lodGenerationOffset,
                          &BaseTexture::set_lodGenerationOffset}
    , lodGenerationScale{this, &BaseTexture::get_lodGenerationScale,
                         &BaseTexture::set_lodGenerationScale}
    , linearSpecularLOD{this, &BaseTexture::get_linearSpecularLOD,
                        &BaseTexture::set_linearSpecularLOD}
    , irradianceTexture{this, &BaseTexture::get_irradianceTexture,
                        &BaseTexture::set_irradianceTexture}
    , isRenderTarget{false}
    , _prefiltered{false}
    , uid{this, &BaseTexture::get_uid}
    , onDispose{this, &BaseTexture::set_onDispose}
    , isBlocking{this, &BaseTexture::get_isBlocking, &BaseTexture::set_isBlocking}
    , boundingBoxSize{this, &BaseTexture::get_boundingBoxSize, &BaseTexture::set_boundingBoxSize}
    , textureType{this, &BaseTexture::get_textureType}
    , textureFormat{this, &BaseTexture::get_textureFormat}
    , sphericalPolynomial{this, &BaseTexture::get_sphericalPolynomial,
                          &BaseTexture::set_sphericalPolynomial}
    , _lodTextureHigh{this, &BaseTexture::get__lodTextureHigh}
    , _lodTextureMid{this, &BaseTexture::get__lodTextureMid}
    , _lodTextureLow{this, &BaseTexture::get__lodTextureLow}
    , _coordinatesMode{Constants::TEXTURE_EXPLICIT_MODE}
    , _scene{nullptr}
    , _hasAlpha{false}
    , _isCube{false}
    , _gammaSpace{true}
    , _uid{GUID::RandomId()}
    , _onDisposeObserver{nullptr}
    , _textureMatrix{Matrix::IdentityReadOnly()}
    , _reflectionTextureMatrix{Matrix::IdentityReadOnly()}
    , emptyVector3{std::nullopt}
    , _nullSphericalPolynomial{nullptr}
    , _nullBaseTexture{nullptr}
{
  wrapR = Constants::TEXTURE_WRAP_ADDRESSMODE;

  if (sceneOrEngine) {
    if (BaseTexture::_isScene(*sceneOrEngine)) {
      _scene = std::get<Scene*>(*sceneOrEngine);
    }
    else {
      _engine = std::get<ThinEngine*>(*sceneOrEngine);
    }
  }
  else {
    _scene = EngineStore::LastCreatedScene();
  }

  if (_scene) {
    uniqueId = _scene->getUniqueId();
    _engine  = _scene->getEngine();
  }
}

BaseTexture::~BaseTexture() = default;

Type BaseTexture::type() const
{
  return Type::BASETEXTURE;
}

void BaseTexture::addToScene(const BaseTexturePtr& newTexture)
{
  if (_scene) {
    _scene->textures.emplace_back(newTexture);
  }
}

void BaseTexture::set_hasAlpha(bool value)
{
  if (_hasAlpha == value) {
    return;
  }
  _hasAlpha = value;
  if (_scene) {
    _scene->markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag
                                    | Constants::MATERIAL_MiscDirtyFlag);
  }
}

bool BaseTexture::get_hasAlpha() const
{
  return _hasAlpha;
}

void BaseTexture::set_coordinatesMode(unsigned int value)
{
  if (_coordinatesMode == value) {
    return;
  }
  _coordinatesMode = value;
  if (_scene) {
    _scene->markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
  }
}

unsigned int BaseTexture::get_coordinatesMode() const
{
  return _coordinatesMode;
}

unsigned int BaseTexture::get_wrapU() const
{
  return _wrapU;
}

void BaseTexture::set_wrapU(unsigned int value)
{
  _wrapU = value;
}

unsigned int BaseTexture::get_wrapV() const
{
  return _wrapV;
}

void BaseTexture::set_wrapV(unsigned int value)
{
  _wrapV = value;
}

bool BaseTexture::get_isCube() const
{
  if (!_texture) {
    return _isCube;
  }

  return _texture->isCube;
}

void BaseTexture::set_isCube(bool value)
{
  if (!_texture) {
    _isCube = value;
  }
  else {
    _texture->isCube = value;
  }
}

bool BaseTexture::get_is3D() const
{
  if (!_texture) {
    return false;
  }

  return _texture->is3D;
}

void BaseTexture::set_is3D(bool value)
{
  if (!_texture) {
    return;
  }

  _texture->is3D = value;
}

bool BaseTexture::get_is2DArray() const
{
  if (!_texture) {
    return false;
  }

  return _texture->is2DArray;
}

void BaseTexture::set_is2DArray(bool value)
{
  if (!_texture) {
    return;
  }

  _texture->is2DArray = value;
}

bool BaseTexture::get_gammaSpace() const
{
  if (!_texture) {
    return _gammaSpace;
  }
  else {
    if (!_texture->_gammaSpace.has_value()) {
      _texture->_gammaSpace = _gammaSpace;
    }
  }

  return *_texture->_gammaSpace;
}

void BaseTexture::set_gammaSpace(bool gamma)
{
  if (!_texture) {
    if (_gammaSpace == gamma) {
      return;
    }

    _gammaSpace = gamma;
  }
  else {
    if (_texture->_gammaSpace.has_value() && *_texture->_gammaSpace == gamma) {
      return;
    }
    _texture->_gammaSpace = gamma;
  }

  _markAllSubMeshesAsTexturesDirty();
}

bool BaseTexture::get_noMipmap() const
{
  return false;
}

bool BaseTexture::get_isRGBD() const
{
  return _texture != nullptr && _texture->_isRGBD;
}

void BaseTexture::set_isRGBD(bool value)
{
  if (_texture) {
    _texture->_isRGBD = value;
  }
}

float BaseTexture::get_lodGenerationOffset() const
{
  if (_texture) {
    return _texture->_lodGenerationOffset;
  }

  return 0.f;
}

void BaseTexture::set_lodGenerationOffset(float value)
{
  if (_texture) {
    _texture->_lodGenerationOffset = value;
  }
}

float BaseTexture::get_lodGenerationScale() const
{
  if (_texture) {
    return _texture->_lodGenerationScale;
  }

  return 0.f;
}

void BaseTexture::set_lodGenerationScale(float value)
{
  if (_texture) {
    _texture->_lodGenerationScale = value;
  }
}

bool BaseTexture::get_linearSpecularLOD() const
{
  if (_texture) {
    return _texture->_linearSpecularLOD;
  }

  return false;
}

void BaseTexture::set_linearSpecularLOD(bool value)
{
  if (_texture) {
    _texture->_linearSpecularLOD = value;
  }
}

BaseTexturePtr& BaseTexture::get_irradianceTexture()
{
  if (_texture) {
    return _texture->_irradianceTexture;
  }

  return _nullBaseTexture;
}

void BaseTexture::set_irradianceTexture(const BaseTexturePtr& value)
{
  if (_texture) {
    _texture->_irradianceTexture = value;
  }
}

std::string BaseTexture::get_uid() const
{
  return _uid;
}

std::string BaseTexture::toString() const
{
  return name;
}

std::string BaseTexture::getClassName() const
{
  return "BaseTexture";
}

void BaseTexture::set_onDispose(const std::function<void(BaseTexture*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

bool BaseTexture::get_isBlocking() const
{
  return true;
}

void BaseTexture::set_isBlocking(bool /*value*/)
{
}

Scene* BaseTexture::getScene() const
{
  return _scene;
}

ThinEngine* BaseTexture::_getEngine() const
{
  return _engine;
}

bool BaseTexture::checkTransformsAreIdentical(const BaseTexturePtr& texture) const
{
  return texture != nullptr;
}

Matrix* BaseTexture::getTextureMatrix(int /*uBase*/)
{
  return &_textureMatrix;
}

Matrix* BaseTexture::getReflectionTextureMatrix()
{
  return &_reflectionTextureMatrix;
}

bool BaseTexture::isReadyOrNotBlocking()
{
  return !isBlocking() || isReady();
}

void BaseTexture::scale(float /*ratio*/)
{
}

bool BaseTexture::canRescale()
{
  return false;
}

InternalTexturePtr BaseTexture::_getFromCache(const std::string& url, bool iNoMipmap,
                                              unsigned int sampling,
                                              const std::optional<bool>& invertY)
{
  auto engine = _getEngine();
  if (!engine) {
    return nullptr;
  }

  auto& texturesCache = engine->getLoadedTexturesCache();
  for (auto& texturesCacheEntry : texturesCache) {
    if (!invertY.has_value() || *invertY == texturesCacheEntry->invertY) {
      if ((texturesCacheEntry->url == url) && texturesCacheEntry->generateMipMaps != iNoMipmap) {
        if (!sampling || sampling == texturesCacheEntry->samplingMode) {
          texturesCacheEntry->incrementReferences();
          return texturesCacheEntry;
        }
      }
    }
  }

  return nullptr;
}

void BaseTexture::_rebuild(bool /*forceFullRebuild*/)
{
}

void BaseTexture::set_boundingBoxSize(const std::optional<Vector3>& /*value*/)
{
}

std::optional<Vector3>& BaseTexture::get_boundingBoxSize()
{
  return emptyVector3;
}

std::vector<AnimationPtr> BaseTexture::getAnimations()
{
  return animations;
}

std::unique_ptr<BaseTexture> BaseTexture::clone() const
{
  return nullptr;
}

unsigned int BaseTexture::get_textureType() const
{
  if (!_texture) {
    return Constants::TEXTURETYPE_UNSIGNED_INT;
  }

  return _texture->type ? _texture->type : Constants::TEXTURETYPE_UNSIGNED_INT;
}

unsigned int BaseTexture::get_textureFormat() const
{
  if (!_texture) {
    return Constants::TEXTUREFORMAT_RGBA;
  }

  return _texture->format ? _texture->format : Constants::TEXTUREFORMAT_RGBA;
}

void BaseTexture::_markAllSubMeshesAsTexturesDirty()
{
  auto scene = getScene();

  if (!scene) {
    return;
  }

  scene->markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

ArrayBufferView BaseTexture::readPixels(unsigned int faceIndex, int iLevel,
                                        std::optional<ArrayBufferView> buffer, bool flushRenderer)
{
  if (!_texture) {
    return ArrayBufferView();
  }

  auto size   = getSize();
  auto width  = size.width;
  auto height = size.height;

  const auto engine = _getEngine();
  if (!engine) {
    return ArrayBufferView();
  }

  if (iLevel != 0) {
    width  = width / static_cast<int>(std::pow(2, iLevel));
    height = height / static_cast<int>(std::pow(2, iLevel));

    width  = static_cast<int>(std::round(width));
    height = static_cast<int>(std::round(height));
  }

  if (_texture->isCube) {
    return engine->_readTexturePixels(_texture, width, height, static_cast<int>(faceIndex), iLevel,
                                      buffer, flushRenderer);
  }

  return engine->_readTexturePixels(_texture, width, height, -1, iLevel, buffer, flushRenderer);
}

ArrayBufferView BaseTexture::_readPixelsSync(unsigned int faceIndex, int iLevel,
                                             std::optional<ArrayBufferView> buffer,
                                             bool flushRenderer)
{
  if (!_texture) {
    return ArrayBufferView();
  }

  auto size   = getSize();
  auto width  = size.width;
  auto height = size.height;

  const auto engine = _getEngine();
  if (!engine) {
    return ArrayBufferView();
  }

  if (iLevel != 0) {
    width  = width / static_cast<int>(std::pow(2, iLevel));
    height = height / static_cast<int>(std::pow(2, iLevel));

    width  = static_cast<int>(std::round(width));
    height = static_cast<int>(std::round(height));
  }

  if (_texture->isCube) {
    return engine->_readTexturePixelsSync(_texture, width, height, static_cast<int>(faceIndex),
                                          iLevel, buffer, flushRenderer);
  }

  return engine->_readTexturePixelsSync(_texture, width, height, -1, iLevel, buffer, flushRenderer);
}

SphericalPolynomialPtr& BaseTexture::get_sphericalPolynomial()
{
  if (_texture) {
    if (_texture->_sphericalPolynomial || _texture->_sphericalPolynomialComputed) {
      return _texture->_sphericalPolynomial;
    }

    if (_texture->isReady) {
      if (!_texture->_sphericalPolynomialPromise) {
        _texture->_sphericalPolynomialPromise = [this]() -> SphericalPolynomialPtr {
          return CubeMapToSphericalPolynomialTools::ConvertCubeMapTextureToSphericalPolynomial(
            *this);
        };
        if (_texture->_sphericalPolynomialPromise == nullptr) {
          _texture->_sphericalPolynomialComputed = true;
        }
        else {
          _texture->_sphericalPolynomial         = _texture->_sphericalPolynomialPromise();
          _texture->_sphericalPolynomialComputed = true;

          return _texture->_sphericalPolynomial;
        }
      }

      return _nullSphericalPolynomial;
    }
  }

  return _nullSphericalPolynomial;
}

void BaseTexture::set_sphericalPolynomial(const SphericalPolynomialPtr& value)
{
  if (_texture) {
    _texture->_sphericalPolynomial = value;
  }
}

BaseTexturePtr& BaseTexture::get__lodTextureHigh()
{
  if (_texture) {
    return _texture->_lodTextureHigh;
  }
  return _nullBaseTexture;
}

BaseTexturePtr& BaseTexture::get__lodTextureMid()
{
  if (_texture) {
    return _texture->_lodTextureMid;
  }
  return _nullBaseTexture;
}

BaseTexturePtr& BaseTexture::get__lodTextureLow()
{
  if (_texture) {
    return _texture->_lodTextureLow;
  }
  return _nullBaseTexture;
}

void BaseTexture::dispose()
{
  if (_scene) {
    // Animations
    _scene->stopAnimation(this);

    // Remove from scene
    stl_util::remove_vector_elements_equal_sharedptr(_scene->textures, this);

    _scene->onTextureRemovedObservable.notifyObservers(this);
    _scene = nullptr;
  }

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();

  ThinTexture::dispose();
}

json BaseTexture::serialize() const
{
  return nullptr;
}

void BaseTexture::WhenAllReady(const std::vector<BaseTexture*>& textures,
                               const std::function<void()>& callback)
{
  auto numRemaining = textures.size();
  if (numRemaining == 0) {
    callback();
    return;
  }

  for (auto& texture : textures) {
    if (texture->isReady()) {
      if (--numRemaining == 0) {
        callback();
      }
    }
    else {
      auto onLoadObservable = static_cast<Texture*>(texture)->onLoadObservable();

      /* if (onLoadObservable) */ {
        onLoadObservable.addOnce([&](Texture*, EventState&) {
          if (--numRemaining == 0) {
            callback();
          }
        });
      }
    }
  }
}

bool BaseTexture::_isScene(const std::variant<Scene*, ThinEngine*>& sceneOrEngine)
{
  return std::holds_alternative<Scene*>(sceneOrEngine);
}

} // end of namespace BABYLON
