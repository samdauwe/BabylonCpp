#include <babylon/materials/textures/cube_texture.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/matrix.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

CubeTexture*
CubeTexture::CreateFromImages(const std::vector<std::string>& iFiles,
                              Scene* scene, bool noMipmap)
{
  return new CubeTexture("", scene, {}, noMipmap, iFiles);
}

CubeTexture::CubeTexture(const std::string& rootUrl, Scene* scene,
                         const std::vector<std::string>& extensions,
                         bool noMipmap, const std::vector<std::string>& iFiles,
                         const std::function<void()>& onLoad,
                         const std::function<void()>& onError)
    : BaseTexture{scene}
    , url{rootUrl}
    , coordinatesMode{Texture::CUBIC_MODE}
    , _noMipmap{noMipmap}
    , _textureMatrix{std_util::make_unique<Matrix>(Matrix::Identity())}
{
  name     = rootUrl;
  hasAlpha = false;

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
      _texture = scene->getEngine()->createCubeTexture(
        rootUrl, scene, extensions, noMipmap, onLoad, onError);
    }
    else {
      delayLoadState = Engine::DELAYLOADSTATE_NOTLOADED;
    }
  }
  else if (onLoad) {
    if (_texture->isReady) {
      Tools::SetImmediate([&onLoad]() { onLoad(); });
    }
    else {
      _texture->onLoadedCallbacks.emplace_back(onLoad);
    }
  }

  isCube = true;
}

CubeTexture::~CubeTexture()
{
}

// Methods
void CubeTexture::delayLoad()
{
  if (delayLoadState != Engine::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  delayLoadState = Engine::DELAYLOADSTATE_LOADED;
  _texture       = _getFromCache(url, _noMipmap);

  if (!_texture) {
    _texture = getScene()->getEngine()->createCubeTexture(
      url, getScene(), _extensions, _noMipmap);
  }
}

Matrix* CubeTexture::getReflectionTextureMatrix()
{
  return _textureMatrix.get();
}

std::unique_ptr<CubeTexture>
CubeTexture::Parse(const Json::value& /*parsedTexture*/, Scene* /*scene*/,
                   const std::string& /*rootUrl*/)
{
  return nullptr;
}

std::unique_ptr<CubeTexture> CubeTexture::clone() const
{
  auto newTexture = std_util::make_unique<CubeTexture>(
    url, getScene(), _extensions, _noMipmap, _files);

  return newTexture;
}

} // end of namespace BABYLON
