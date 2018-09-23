#include <babylon/materials/textures/color_grading_texture.h>

#include <babylon/core/json.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ColorGradingTexture::ColorGradingTexture(const std::string& iUrl, Scene* scene)
    : BaseTexture{scene}
{
  if (iUrl.empty()) {
    return;
  }

  _engine        = scene->getEngine();
  _textureMatrix = ::std::make_unique<Matrix>(Matrix::Identity());
  name           = iUrl;
  url            = iUrl;
  hasAlpha       = false;
  isCube         = false;
  is3D           = _engine->webGLVersion() > 1.f;
  wrapU          = TextureConstants::CLAMP_ADDRESSMODE;
  wrapV          = TextureConstants::CLAMP_ADDRESSMODE;
  wrapR          = TextureConstants::CLAMP_ADDRESSMODE;

  anisotropicFilteringLevel = 1;

  _texture = _getFromCache(url, true);

  if (!_texture) {
    if (!scene->useDelayedTextureLoading) {
      loadTexture();
    }
    else {
      delayLoadState = EngineConstants::DELAYLOADSTATE_NOTLOADED;
    }
  }
}

ColorGradingTexture::~ColorGradingTexture()
{
}

Matrix* ColorGradingTexture::getTextureMatrix()
{
  return _textureMatrix.get();
}

InternalTexture* ColorGradingTexture::load3dlTexture()
{
  InternalTexture* texture = nullptr;
  if (_engine->webGLVersion() == 1.f) {
    texture = _engine->createRawTexture(
      Uint8Array(), 1, 1, EngineConstants::TEXTUREFORMAT_RGBA, false, false,
      TextureConstants::BILINEAR_SAMPLINGMODE);
  }
  else {
    texture = _engine->createRawTexture3D(
      Uint8Array(), 1, 1, 1, EngineConstants::TEXTUREFORMAT_RGBA, false, false,
      TextureConstants::BILINEAR_SAMPLINGMODE);
  }

  _texture = texture;

#if 0
  const auto callback = [&](Variant<std::string, ArrayBuffer>& iText) {
    if (!iText.is<std::string>()) {
      return;
    }

    auto text = iText.get<std::string>();

    Uint8Array data;
    Float32Array tempData;

    auto lines  = String::split(text, '\n');
    size_t size = 0, pixelIndexW = 0, pixelIndexH = 0, pixelIndexSlice = 0;
    int maxColor = 0;

    for (auto& line : lines) {

      if (line.empty()) {
        continue;
      }

      if (line.at(0) == '#') {
        continue;
      }

      auto words = String::split(line, ' ');
      if (size == 0) {
        // Number of space + one
        size = words.size();
        data.clear();
        data.resize(size * size * size
                    * 4); // volume texture of side size and rgb 8
        tempData.clear();
        tempData.resize(size * size * size * 4);
        continue;
      }

      if (size != 0) {
        int r = ::std::max(::std::stoi(words[0]), 0);
        int g = ::std::max(::std::stoi(words[1]), 0);
        int b = ::std::max(::std::stoi(words[2]), 0);

        maxColor = ::std::max(r, maxColor);
        maxColor = ::std::max(g, maxColor);
        maxColor = ::std::max(b, maxColor);

        size_t pixelStorageIndex
          = (pixelIndexW + pixelIndexSlice * size + pixelIndexH * size * size)
            * 4;

        if (pixelStorageIndex + 2 < tempData.size()) {
          tempData[pixelStorageIndex + 0] = static_cast<float>(r);
          tempData[pixelStorageIndex + 1] = static_cast<float>(g);
          tempData[pixelStorageIndex + 2] = static_cast<float>(b);
        }

        ++pixelIndexSlice;
        if (pixelIndexSlice % size == 0) {
          ++pixelIndexH;
          pixelIndexSlice = 0;
          if (pixelIndexH % size == 0) {
            ++pixelIndexW;
            pixelIndexH = 0;
          }
        }
      }
    }

    if (!tempData.empty() && !data.empty()) {
      for (size_t i = 0; i < tempData.size(); ++i) {
        if (i > 0 && (i + 1) % 4 == 0) {
          data[i] = 255;
        }
        else {
          float value = tempData[i];
          value /= static_cast<float>(maxColor * 255);
          data[i] = static_cast<std::uint8_t>(value);
        }
      }
    }

    const auto _size = static_cast<int>(size);
    if (_texture->is3D) {
      _texture->updateSize(_size, _size, _size);
      _engine->updateRawTexture3D(_texture, data,
                                  EngineConstants::TEXTUREFORMAT_RGBA, false);
    }
    else {
      _texture->updateSize(_size * _size, _size);
      _engine->updateRawTexture(_texture, data,
                                EngineConstants::TEXTUREFORMAT_RGBA, false);
    }
  };
#endif

  auto scene = getScene();
  if (scene) {
    // scene->_loadFile(url, callback);
  }
  else {
    // _engine->_loadFile(url, callback);
  }

  return _texture;
}

void ColorGradingTexture::loadTexture()
{
  if (!url.empty() && String::endsWith(String::toLowerCase(url), ".3dl")) {
    load3dlTexture();
  }
}

std::unique_ptr<ColorGradingTexture> ColorGradingTexture::clone() const
{
  auto newTexture = ::std::make_unique<ColorGradingTexture>(url, getScene());

  // Base texture
  newTexture->level = level;

  return newTexture;
}

void ColorGradingTexture::delayLoad()
{
  if (delayLoadState != EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  delayLoadState = EngineConstants::DELAYLOADSTATE_LOADED;
  _texture       = nullptr; // _getFromCache(url, true);

  if (!_texture) {
    loadTexture();
  }
}

std::unique_ptr<ColorGradingTexture>
ColorGradingTexture::Parse(const Json::value& parsedTexture, Scene* scene,
                           const std::string& /*rootUrl*/)
{
  std::unique_ptr<ColorGradingTexture> texture = nullptr;
  if (parsedTexture.contains("name")
      && !Json::GetBool(parsedTexture, "isRenderTarget")) {
    auto parsedTextureName = Json::GetString(parsedTexture, "name");
    texture = ::std::make_unique<ColorGradingTexture>(parsedTextureName, scene);
    texture->name  = parsedTextureName;
    texture->level = Json::GetNumber<float>(parsedTexture, "level", 0.f);
  }
  return texture;
}

Json::object ColorGradingTexture::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
