#include <babylon/materials/textures/cube_texture.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/iinternal_texture_loader.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/matrix.h>
#include <babylon/misc/serialization_helper.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

CubeTexturePtr
CubeTexture::CreateFromImages(const std::vector<std::string>& iFiles,
                              Scene* scene, bool iNoMipmap)
{
  std::string rootUrlKey = "";

  for (const auto& url : iFiles) {
    rootUrlKey += url;
  }

  const std::vector<std::string> emptyStringList;
  return CubeTexture::New(rootUrlKey, scene, emptyStringList, iNoMipmap,
                          iFiles);
}

CubeTexturePtr
CubeTexture::CreateFromPrefilteredData(const std::string& url, Scene* scene,
                                       const std::string& forcedExtension,
                                       bool createPolynomials)
{
  const std::vector<std::string> emptyStringList;
  return CubeTexture::New(url, scene, emptyStringList, false, emptyStringList,
                          nullptr, nullptr, Constants::TEXTUREFORMAT_RGBA, true,
                          forcedExtension, createPolynomials);
}

CubeTexture::CubeTexture(
  const std::string& rootUrl, Scene* scene,
  const std::vector<std::string>& extensions, bool iNoMipmap,
  const std::vector<std::string>& iFiles,
  const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError,
  unsigned int format, bool prefiltered, const std::string& forcedExtension,
  bool createPolynomials, float lodScale, float lodOffset)
    : BaseTexture{scene}
    , url{rootUrl}
    , boundingBoxPosition{Vector3::Zero()}
    , rotationY{this, &CubeTexture::get_rotationY, &CubeTexture::set_rotationY}
    , noMipmap{this, &CubeTexture::get_noMipmap}
    , _prefiltered{false}
    , isPrefiltered{this, &CubeTexture::get_isPrefiltered}
    , _delayedOnLoad{nullptr}
    , _boundingBoxSize{std::nullopt}
    , _rotationY{0.f}
    , _noMipmap{iNoMipmap}
    , _textureMatrix{std::make_unique<Matrix>(Matrix::Identity())}
    , _format{format}
    , _forcedExtension{forcedExtension}
    , _createPolynomials{createPolynomials}
    , _lodScale{lodScale}
    , _lodOffset{lodOffset}
{
  isCube = true;

  name            = rootUrl;
  hasAlpha        = false;
  coordinatesMode = TextureConstants::CUBIC_MODE;

  if (rootUrl.empty() && iFiles.empty()) {
    return;
  }

  const auto lastDot = String::lastIndexOf(rootUrl, ".");
  const auto extension
    = (!forcedExtension.empty()) ?
        forcedExtension :
        (lastDot > -1 ?
           String::toLowerCase(rootUrl.substr(static_cast<size_t>(lastDot))) :
           "");
  const auto isDDS = (extension == ".dds");
  const auto isEnv = (extension == ".env");

  if (isEnv) {
    gammaSpace   = false;
    _prefiltered = false;
  }
  else {
    _prefiltered = prefiltered;

    if (prefiltered) {
      gammaSpace = false;
    }
  }

  _texture = _getFromCache(rootUrl, noMipmap);

  _files = iFiles;

  if (iFiles.empty()) {
    _extensions = extensions;

    if (!isEnv && !isDDS && _extensions.empty()) {
      _extensions
        = {"_px.jpg", "_py.jpg", "_pz.jpg", "_nx.jpg", "_ny.jpg", "_nz.jpg"};
    }

    _files.clear();

    if (!_extensions.empty()) {
      for (const auto& iExtension : _extensions) {
        _files.emplace_back(rootUrl + iExtension);
      }
    }
  }

  if (!_texture) {
    if (!scene->useDelayedTextureLoading) {
      if (prefiltered) {
        _texture = scene->getEngine()->createPrefilteredCubeTexture(
          rootUrl, scene, lodScale, lodOffset, onLoad, onError, format,
          forcedExtension, _createPolynomials);
      }
      else {
        _texture = scene->getEngine()->createCubeTexture(
          rootUrl, scene, _files, noMipmap, onLoad, onError, _format,
          forcedExtension, false, lodScale, lodOffset);
      }
    }
    else {
      delayLoadState = Constants::DELAYLOADSTATE_NOTLOADED;
    }
  }
  else if (onLoad) {
    if (_texture->isReady) {
      Tools::SetImmediate([]() {
        // EventState es{-1};
        // onLoad(nullptr, es);
      });
    }
    else {
      // _texture->onLoadedObservable.add(onLoad);
    }
  }
}

CubeTexture::~CubeTexture()
{
}

void CubeTexture::set_boundingBoxSize(const std::optional<Vector3>& value)
{
  if (_boundingBoxSize && (*_boundingBoxSize).equals(*value)) {
    return;
  }
  _boundingBoxSize = value;
  auto scene       = getScene();
  if (scene) {
    scene->markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
  }
}

std::optional<Vector3>& CubeTexture::get_boundingBoxSize()
{
  return _boundingBoxSize;
}

void CubeTexture::set_rotationY(float value)
{
  _rotationY = value;
  setReflectionTextureMatrix(Matrix::RotationY(_rotationY));
}

float CubeTexture::get_rotationY() const
{
  return _rotationY;
}

bool CubeTexture::get_noMipmap() const
{
  return _noMipmap;
}

bool CubeTexture::get_isPrefiltered() const
{
  return _prefiltered;
}

const std::string CubeTexture::getClassName() const
{
  return "CubeTexture";
}

void CubeTexture::updateURL(
  const std::string& iUrl, const std::string& forcedExtension,
  const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad)
{
  if (!url.empty()) {
    releaseInternalTexture();
    getScene()->markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
  }

  name           = iUrl;
  url            = iUrl;
  delayLoadState = Constants::DELAYLOADSTATE_NOTLOADED;
  _prefiltered   = false;

  if (onLoad) {
    _delayedOnLoad = onLoad;
  }

  delayLoad(forcedExtension);
}

void CubeTexture::delayLoad(const std::string& forcedExtension)
{
  if (delayLoadState != Constants::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  auto scene = getScene();

  if (!scene) {
    return;
  }

  delayLoadState = Constants::DELAYLOADSTATE_LOADED;
  _texture       = _getFromCache(url, _noMipmap);

  if (!_texture) {
    if (_prefiltered) {
      _texture = scene->getEngine()->createPrefilteredCubeTexture(
        url, scene, lodGenerationScale, lodGenerationOffset, _delayedOnLoad,
        nullptr, _format, "", _createPolynomials);
    }
    else {

      _texture = scene->getEngine()->createCubeTexture(
        url, scene, _files, _noMipmap, _delayedOnLoad, nullptr, _format,
        forcedExtension);
    }
  }
}

Matrix* CubeTexture::getReflectionTextureMatrix()
{
  return _textureMatrix.get();
}

void CubeTexture::setReflectionTextureMatrix(Matrix value)
{
  if (value.updateFlag == _textureMatrix->updateFlag) {
    return;
  }

  if (value.isIdentity() != _textureMatrix->isIdentity()) {
    getScene()->markAllMaterialsAsDirty(
      Constants::MATERIAL_TextureDirtyFlag, [this](Material* mat) -> bool {
        auto activeTextures = mat->getActiveTextures();
        auto it = std::find_if(activeTextures.begin(), activeTextures.end(),
                               [this](const BaseTexturePtr& baseTexture) {
                                 return baseTexture.get() == this;
                               });
        return it != activeTextures.end();
      });
  }

  _textureMatrix = std::make_unique<Matrix>(value);
}

CubeTexturePtr CubeTexture::Parse(const json& parsedTexture, Scene* scene,
                                  const std::string& rootUrl)
{

  auto texture = SerializationHelper::Parse(
    [&]() -> CubeTexturePtr {
      auto prefiltered = false;
      if (json_util::get_bool(parsedTexture, "prefiltered")) {
        prefiltered = true;
      }
      return CubeTexture::New(
        rootUrl + json_util::get_string(parsedTexture, "name"), scene,
        json_util::get_array<std::string>(parsedTexture, "extensions"), false,
        std::vector<std::string>{}, nullptr, nullptr,
        Constants::TEXTUREFORMAT_RGBA, prefiltered);
    },
    parsedTexture, scene);

  // Local Cubemaps
  if (json_util::has_key(parsedTexture, "boundingBoxPosition")
      && !json_util::is_null(parsedTexture["boundingBoxPosition"])) {
    texture->boundingBoxPosition = Vector3::FromArray(
      json_util::get_array<float>(parsedTexture, "boundingBoxPosition"));
  }
  if (json_util::has_key(parsedTexture, "boundingBoxSize")
      && !json_util::is_null(parsedTexture["boundingBoxSize"])) {
    texture->boundingBoxSize = Vector3::FromArray(
      json_util::get_array<float>(parsedTexture, "boundingBoxSize"));
  }

  // Animations
  if (json_util::has_key(parsedTexture, "animations")) {
    for (auto& parsedAnimation :
         json_util::get_array<json>(parsedTexture, "animations"))
      texture->animations.emplace_back(Animation::Parse(parsedAnimation));
  }

  return texture;
}

CubeTexturePtr CubeTexture::clone() const
{
  auto scene = getScene();

  if (!scene) {
    return nullptr;
  }

  auto newCubeTexture = CubeTexture::New(
    url, scene, _extensions, _noMipmap, _files, nullptr, nullptr, _format,
    _prefiltered, _forcedExtension, _createPolynomials, _lodScale, _lodOffset);

  return newCubeTexture;
}

} // end of namespace BABYLON
