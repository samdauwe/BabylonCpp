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
    , _textureMatrix{std::make_unique<Matrix>(Matrix::Identity())}
{
  if (iUrl.empty()) {
    return;
  }

  name = iUrl;
  url  = iUrl;
  setHasAlpha(false);
  isCube                    = false;
  wrapU                     = TextureConstants::CLAMP_ADDRESSMODE;
  wrapV                     = TextureConstants::CLAMP_ADDRESSMODE;
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
  _texture = getScene()->getEngine()->createRawTexture(
    Uint8Array(), 1, 1, EngineConstants::TEXTUREFORMAT_RGBA, false, false,
    TextureConstants::BILINEAR_SAMPLINGMODE);

  auto callback = [&](const std::string& text) {
    Uint8Array data;
    Float32Array tempData;

    std::vector<std::string> lines = String::split(text, '\n');
    size_t size = 0, pixelIndexW = 0, pixelIndexH = 0, pixelIndexSlice = 0;
    int maxColor = 0;

    for (auto& line : lines) {

      if (line.empty()) {
        continue;
      }

      if (line.at(0) == '#') {
        continue;
      }

      std::vector<std::string> words = String::split(line, ' ');
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
        int r = std::max(std::stoi(words[0]), 0);
        int g = std::max(std::stoi(words[1]), 0);
        int b = std::max(std::stoi(words[2]), 0);

        maxColor = std::max(r, maxColor);
        maxColor = std::max(g, maxColor);
        maxColor = std::max(b, maxColor);

        size_t pixelStorageIndex
          = (pixelIndexW + pixelIndexSlice * size + pixelIndexH * size * size)
            * 4;

        tempData[pixelStorageIndex + 0] = static_cast<float>(r);
        tempData[pixelStorageIndex + 1] = static_cast<float>(g);
        tempData[pixelStorageIndex + 2] = static_cast<float>(b);

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

    const auto _size = static_cast<int>(size);
    _texture->updateSize(_size * _size, _size);
    getScene()->getEngine()->updateRawTexture(
      _texture, data, EngineConstants::TEXTUREFORMAT_RGBA, false);
  };

  Tools::LoadFile(url, callback);
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
    texture = std::make_unique<ColorGradingTexture>(parsedTextureName, scene);
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
