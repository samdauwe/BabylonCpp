#include <babylon/materials/textures/cube_texture.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
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
  const vector_t<string_t> emptyStringList;
  return ::std::make_unique<CubeTexture>("", scene, emptyStringList, noMipmap,
                                         iFiles);
}

unique_ptr_t<CubeTexture>
CreateFromPrefilteredData(const string_t& url, Scene* scene,
                          const string_t& forcedExtension)
{
  const vector_t<string_t> emptyStringList;
  return ::std::make_unique<CubeTexture>(
    url, scene, emptyStringList, false, emptyStringList, nullptr, nullptr,
    EngineConstants::TEXTUREFORMAT_RGBA, true, forcedExtension);
}

CubeTexture::CubeTexture(
  const string_t& rootUrl, Scene* scene, const vector_t<string_t>& extensions,
  bool noMipmap, const vector_t<string_t>& iFiles,
  const ::std::function<void(InternalTexture*, const EventState&)>& onLoad,
  const ::std::function<void()>& onError, unsigned int format, bool prefiltered,
  const string_t& forcedExtension)
    : BaseTexture{scene}
    , url{rootUrl}
    , coordinatesMode{TextureConstants::CUBIC_MODE}
    , _noMipmap{noMipmap}
    , _textureMatrix{::std::make_unique<Matrix>(Matrix::Identity())}
    , _format{format}
    , _prefiltered{prefiltered}
{
  isCube = true;
  if (prefiltered) {
    gammaSpace = false;
  }

  name = rootUrl;
  setHasAlpha(false);

  if (rootUrl.empty() && iFiles.empty()) {
    return;
  }

  _texture = _getFromCache(rootUrl, noMipmap);

  _files = iFiles;

  if (iFiles.empty()) {
    _extensions = extensions;

    if (_extensions.empty()) {
      _extensions
        = {"_px.jpg", "_py.jpg", "_pz.jpg", "_nx.jpg", "_ny.jpg", "_nz.jpg"};
    }

    for (const auto& extension : _extensions) {
      _files.emplace_back(rootUrl + extension);
    }
  }

  if (!_texture) {
    if (!scene->useDelayedTextureLoading) {
      if (prefiltered) {
        _texture = scene->getEngine()->createPrefilteredCubeTexture(
          rootUrl, scene, lodGenerationScale, lodGenerationOffset, onLoad,
          onError, format, forcedExtension);
      }
      else {
        _texture = scene->getEngine()->createCubeTexture(
          rootUrl, scene, extensions, noMipmap, onLoad, onError, _format,
          forcedExtension);
      }
    }
    else {
      delayLoadState = EngineConstants::DELAYLOADSTATE_NOTLOADED;
    }
  }
  else if (onLoad) {
    if (_texture->isReady) {
      Tools::SetImmediate([&onLoad]() { onLoad(nullptr, EventState(-1)); });
    }
    else {
      _texture->onLoadedObservable.add(onLoad);
    }
  }
}

CubeTexture::~CubeTexture()
{
}

// Methods
void CubeTexture::delayLoad()
{
  if (delayLoadState != EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  delayLoadState = EngineConstants::DELAYLOADSTATE_LOADED;
  _texture       = _getFromCache(url, _noMipmap);

  if (!_texture) {
    if (_prefiltered) {
      _texture = getScene()->getEngine()->createPrefilteredCubeTexture(
        url, getScene(), lodGenerationScale, lodGenerationOffset, nullptr,
        nullptr, _format);
    }
    else {

      _texture = getScene()->getEngine()->createCubeTexture(
        url, getScene(), _files, _noMipmap, nullptr, nullptr, _format);
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
  auto newTexture = ::std::make_unique<CubeTexture>(
    url, getScene(), _extensions, _noMipmap, _files);

  return newTexture;
}

} // end of namespace BABYLON
