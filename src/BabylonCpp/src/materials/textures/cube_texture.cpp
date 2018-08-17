#include <babylon/materials/textures/cube_texture.h>

#include <babylon/core/json.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/matrix.h>
#include <babylon/tools/serialization_helper.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

unique_ptr_t<CubeTexture>
CubeTexture::CreateFromImages(const vector_t<string_t>& iFiles, Scene* scene,
                              bool noMipmap)
{
  string_t rootUrlKey = "";

  for (const auto& url : iFiles) {
    rootUrlKey += url;
  }

  const vector_t<string_t> emptyStringList;
  return ::std::make_unique<CubeTexture>(rootUrlKey, scene, emptyStringList,
                                         noMipmap, iFiles);
}

unique_ptr_t<CubeTexture>
CubeTexture::CreateFromPrefilteredData(const string_t& url, Scene* scene,
                                       const string_t& forcedExtension,
                                       bool createPolynomials)
{
  const vector_t<string_t> emptyStringList;
  return ::std::make_unique<CubeTexture>(
    url, scene, emptyStringList, false, emptyStringList, nullptr, nullptr,
    EngineConstants::TEXTUREFORMAT_RGBA, true, forcedExtension,
    createPolynomials);
}

CubeTexture::CubeTexture(
  const string_t& rootUrl, Scene* scene, const vector_t<string_t>& extensions,
  bool noMipmap, const vector_t<string_t>& iFiles,
  const ::std::function<void(InternalTexture*, EventState&)>& onLoad,
  const ::std::function<void()>& onError, unsigned int format, bool prefiltered,
  const string_t& forcedExtension, bool createPolynomials, float lodScale,
  float lodOffset)
    : BaseTexture{scene}
    , url{rootUrl}
    , boundingBoxPosition{Vector3::Zero()}
    , rotationY{this, &CubeTexture::get_rotationY, &CubeTexture::set_rotationY}
    , _prefiltered{false}
    , _boundingBoxSize{nullopt_t}
    , _rotationY{0.f}
    , _noMipmap{noMipmap}
    , _textureMatrix{::std::make_unique<Matrix>(Matrix::Identity())}
    , _format{format}
    , _createPolynomials{createPolynomials}
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
      for (const auto& extension : _extensions) {
        _files.emplace_back(rootUrl + extension);
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
          rootUrl, scene, extensions, noMipmap, onLoad, onError, _format,
          forcedExtension, false, lodScale, lodOffset);
      }
    }
    else {
      delayLoadState = EngineConstants::DELAYLOADSTATE_NOTLOADED;
    }
  }
  else if (onLoad) {
    if (_texture->isReady) {
      Tools::SetImmediate([&onLoad]() {
        EventState es{-1};
        onLoad(nullptr, es);
      });
    }
    else {
      _texture->onLoadedObservable.add(onLoad);
    }
  }
}

CubeTexture::~CubeTexture()
{
}

void CubeTexture::set_boundingBoxSize(const nullable_t<Vector3>& value)
{
  if (_boundingBoxSize && (*_boundingBoxSize).equals(*value)) {
    return;
  }
  _boundingBoxSize = value;
  auto scene       = getScene();
  if (scene) {
    scene->markAllMaterialsAsDirty(Material::TextureDirtyFlag());
  }
}

nullable_t<Vector3>& CubeTexture::get_boundingBoxSize()
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

void CubeTexture::delayLoad()
{
  if (delayLoadState != EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  auto scene = getScene();

  if (!scene) {
    return;
  }

  delayLoadState = EngineConstants::DELAYLOADSTATE_LOADED;
  _texture       = _getFromCache(url, _noMipmap);

  if (!_texture) {
    if (_prefiltered) {
      _texture = scene->getEngine()->createPrefilteredCubeTexture(
        url, scene, lodGenerationScale, lodGenerationOffset, nullptr, nullptr,
        _format, "", _createPolynomials);
    }
    else {

      _texture = scene->getEngine()->createCubeTexture(
        url, scene, _files, _noMipmap, nullptr, nullptr, _format);
    }
  }
}

Matrix* CubeTexture::getReflectionTextureMatrix()
{
  return _textureMatrix.get();
}

void CubeTexture::setReflectionTextureMatrix(const Matrix& value)
{
  _textureMatrix = ::std::make_unique<Matrix>(value);
}

unique_ptr_t<CubeTexture> CubeTexture::Parse(const Json::value& parsedTexture,
                                             Scene* scene,
                                             const string_t& rootUrl)
{

  auto cubeTexture = ::std::make_unique<CubeTexture>(
    rootUrl + Json::GetString(parsedTexture, "name"), scene,
    Json::ToStringVector(parsedTexture, "extensions"));
  SerializationHelper::Parse(cubeTexture.get(), parsedTexture, scene);

  // Local Cubemaps
  if (parsedTexture.contains("boundingBoxPosition")) {
    cubeTexture->boundingBoxPosition = Vector3::FromArray(
      Json::ToArray<float>(parsedTexture, "boundingBoxPosition"));
  }
  if (parsedTexture.contains("boundingBoxSize")) {
    cubeTexture->boundingBoxSize = Vector3::FromArray(
      Json::ToArray<float>(parsedTexture, "boundingBoxSize"));
  }

  // Animations
  if (parsedTexture.contains("animations")) {
    for (auto& parsedAnimation : Json::GetArray(parsedTexture, "animations")) {
      cubeTexture->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
  }

  return cubeTexture;
}

unique_ptr_t<CubeTexture> CubeTexture::clone() const
{
  auto scene = getScene();

  if (!scene) {
    return nullptr;
  }

  auto newTexture = ::std::make_unique<CubeTexture>(url, scene, _extensions,
                                                    _noMipmap, _files);

  return newTexture;
}

} // end of namespace BABYLON
