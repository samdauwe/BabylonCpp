#include <babylon/materials/textures/color_grading_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/core/string.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

ColorGradingTexture::ColorGradingTexture(const std::string& iUrl, Scene* scene)
    : BaseTexture{scene}
{
  if (iUrl.empty()) {
    return;
  }

  _engine        = scene->getEngine();
  _textureMatrix = std::make_unique<Matrix>(Matrix::Identity());
  name           = iUrl;
  url            = iUrl;
  hasAlpha       = false;
  isCube         = false;
  is3D           = _engine->webGLVersion() > 1.f;
  wrapU          = Constants::TEXTURE_CLAMP_ADDRESSMODE;
  wrapV          = Constants::TEXTURE_CLAMP_ADDRESSMODE;
  wrapR          = Constants::TEXTURE_CLAMP_ADDRESSMODE;

  anisotropicFilteringLevel = 1;

  _texture = _getFromCache(url, true);

  if (!_texture) {
    if (!scene->useDelayedTextureLoading) {
      loadTexture();
    }
    else {
      delayLoadState = Constants::DELAYLOADSTATE_NOTLOADED;
    }
  }
}

ColorGradingTexture::~ColorGradingTexture() = default;

Matrix* ColorGradingTexture::getTextureMatrix()
{
  return _textureMatrix.get();
}

InternalTexturePtr ColorGradingTexture::load3dlTexture()
{
  InternalTexturePtr texture = nullptr;
  if (_engine->webGLVersion() == 1.f) {
    texture = _engine->createRawTexture(
      Uint8Array(), 1, 1, Constants::TEXTUREFORMAT_RGBA, false, false,
      TextureConstants::BILINEAR_SAMPLINGMODE);
  }
  else {
    texture = _engine->createRawTexture3D(
      Uint8Array(), 1, 1, 1, Constants::TEXTUREFORMAT_RGBA, false, false,
      TextureConstants::BILINEAR_SAMPLINGMODE);
  }

  _texture = texture;

  const auto callback = [&](const std::variant<std::string, ArrayBuffer>& iText,
                            const std::string & /*onSuccess*/) -> void {
    if (!std::holds_alternative<std::string>(iText)) {
      return;
    }

    auto text = std::get<std::string>(iText);

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
        const auto r = std::max(std::stoi(words[0]), 0);
        const auto g = std::max(std::stoi(words[1]), 0);
        const auto b = std::max(std::stoi(words[2]), 0);

        maxColor = std::max(r, maxColor);
        maxColor = std::max(g, maxColor);
        maxColor = std::max(b, maxColor);

        size_t pixelStorageIndex
          = (pixelIndexW + pixelIndexSlice * size + pixelIndexH * size * size)
            * 4;

        if (pixelStorageIndex + 2 < tempData.size()) {
          tempData[pixelStorageIndex + 0] = static_cast<float>(r);
          tempData[pixelStorageIndex + 1] = static_cast<float>(g);
          tempData[pixelStorageIndex + 2] = static_cast<float>(b);
        }

        ++pixelIndexH;
        if (pixelIndexH % size == 0) {
          ++pixelIndexSlice;
          pixelIndexH = 0;
          if (pixelIndexSlice % size == 0) {
            ++pixelIndexW;
            pixelIndexSlice = 0;
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
          auto value = tempData[i];
          value /= static_cast<float>(maxColor * 255);
          data[i] = static_cast<std::uint8_t>(value);
        }
      }
    }

    const auto _size = static_cast<int>(size);
    if (_texture->is3D) {
      _texture->updateSize(_size, _size, _size);
      _engine->updateRawTexture3D(_texture, data, Constants::TEXTUREFORMAT_RGBA,
                                  false);
    }
    else {
      _texture->updateSize(_size * _size, _size);
      _engine->updateRawTexture(_texture, data, Constants::TEXTUREFORMAT_RGBA,
                                false);
    }
  };

  auto scene = getScene();
  if (scene) {
    scene->_loadFile(url, callback);
  }
  else {
    _engine->_loadFile(url, callback);
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
  auto newTexture = std::make_unique<ColorGradingTexture>(url, getScene());

  // Base texture
  newTexture->level = level;

  return newTexture;
}

void ColorGradingTexture::delayLoad(const std::string& /*forcedExtension*/)
{
  if (delayLoadState != Constants::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  delayLoadState = Constants::DELAYLOADSTATE_LOADED;
  _texture       = _getFromCache(url, true);

  if (!_texture) {
    loadTexture();
  }
}

std::unique_ptr<ColorGradingTexture>
ColorGradingTexture::Parse(const json& /*parsedTexture*/, Scene* /*scene*/,
                           const std::string& /*rootUrl*/)
{
  return nullptr;
}

json ColorGradingTexture::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
