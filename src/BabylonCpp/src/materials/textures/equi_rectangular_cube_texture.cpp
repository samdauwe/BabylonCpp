#include <babylon/materials/textures/equi_rectangular_cube_texture.h>

#include <babylon/core/data_view.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/misc/highdynamicrange/panorama_to_cube_map_tools.h>

namespace BABYLON {

std::array<std::string, 6> EquiRectangularCubeTexture::_FacesMapping{
  "right", "left", "up", "down", "front", "back"};

EquiRectangularCubeTexture::EquiRectangularCubeTexture(
  const std::string& iUrl, Scene* scene, size_t size, bool iNoMipmap,
  bool iGammaSpace, const std::function<void()>& onLoad,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError)
    : BaseTexture{scene}
    , coordinatesMode{TextureConstants::CUBIC_MODE}
    , _onLoad{nullptr}
    , _onError{nullptr}
{
  if (iUrl.empty()) {
    throw std::runtime_error("Image url is not set");
  }

  name       = iUrl;
  url        = iUrl;
  _size      = size;
  _noMipmap  = iNoMipmap;
  gammaSpace = iGammaSpace;
  _onLoad    = onLoad;
  _onError   = onError;

  hasAlpha = false;
  isCube   = true;

  _texture = _getFromCache(url, _noMipmap);

  if (!_texture) {
    if (!scene->useDelayedTextureLoading) {
      loadImage([this] { loadTexture(); }, _onError);
    }
    else {
      delayLoadState = Constants::DELAYLOADSTATE_NOTLOADED;
    }
  }
  else if (onLoad) {
    if (_texture->isReady) {
      onLoad();
    }
    else {
      _texture->onLoadedObservable.add(
        [&](InternalTexture*, EventState&) { onLoad(); });
    }
  }
}

void EquiRectangularCubeTexture::loadImage(
  const std::function<void()>& /*loadTextureCallback*/,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& /*onError*/)
{
}

EquiRectangularCubeTexture::~EquiRectangularCubeTexture() = default;

void EquiRectangularCubeTexture::loadTexture()
{
  using ArrayBufferViewArray = std::vector<ArrayBufferView>;

  auto scene = getScene();
  const auto callback
    = [this](const ArrayBuffer & /*arrayBuffer*/) -> ArrayBufferViewArray {
    auto imageData = getFloat32ArrayFromArrayBuffer(_buffer);

    // Extract the raw linear data.
    auto data = PanoramaToCubeMapTools::ConvertPanoramaToCubemap(
      imageData, _width, _height, _size);

    ArrayBufferViewArray results;

    // Push each faces.
    for (unsigned int i = 0; i < 6; i++) {
      const auto& faceMapping = EquiRectangularCubeTexture::_FacesMapping[i];
      if (faceMapping == "right") {
        results.emplace_back(data.right);
      }
      else if (faceMapping == "left") {
        results.emplace_back(data.left);
      }
      else if (faceMapping == "up") {
        results.emplace_back(data.up);
      }
      else if (faceMapping == "down") {
        results.emplace_back(data.down);
      }
      else if (faceMapping == "front") {
        results.emplace_back(data.front);
      }
      else if (faceMapping == "back") {
        results.emplace_back(data.back);
      }
    }

    return results;
  };

  if (!scene) {
    return;
  }
  _texture = scene->getEngine()->createRawCubeTextureFromUrl(
    url,                          //
    scene,                        //
    static_cast<int>(_size),      //
    Constants::TEXTUREFORMAT_RGB, //
    scene->getEngine()->getCaps().textureFloat ?
      Constants::TEXTURETYPE_FLOAT :
      Constants::TEXTURETYPE_UNSIGNED_INTEGER,
    _noMipmap, //
    callback,  //
    nullptr,   //
    _onLoad,   //
    _onError   //
  );
}

Float32Array EquiRectangularCubeTexture::getFloat32ArrayFromArrayBuffer(
  const ArrayBuffer& buffer)
{
  DataView dataView(buffer);
  Float32Array floatImageData((buffer.size() * 3) / 4);

  auto k = 0u;
  for (size_t i = 0; i < buffer.size(); ++i) {
    // We drop the transparency channel, because we do not need/want it
    if ((i + 1) % 4 != 0) {
      floatImageData[k++] =  static_cast<float>(dataView.getUint8(i)) / 255.f;
    }
  }

  return floatImageData;
}

const std::string EquiRectangularCubeTexture::getClassName() const
{
  return "EquiRectangularCubeTexture";
}

EquiRectangularCubeTexturePtr EquiRectangularCubeTexture::clone() const
{
  const auto& scene = getScene();
  if (!scene) {
    return nullptr;
  }

  auto newTexture
    = EquiRectangularCubeTexture::New(url, scene, _size, _noMipmap, gammaSpace);

  // Base texture
  newTexture->level            = level;
  newTexture->wrapU            = wrapU;
  newTexture->wrapV            = wrapV;
  newTexture->coordinatesIndex = coordinatesIndex;
  newTexture->coordinatesMode  = coordinatesMode;

  return newTexture;
}

} // end of namespace BABYLON
