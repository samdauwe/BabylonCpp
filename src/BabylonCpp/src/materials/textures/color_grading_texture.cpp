#include <babylon/materials/textures/color_grading_texture.h>

#include <babylon/core/json.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ColorGradingTexture::ColorGradingTexture(const std::string& iUrl, Scene* scene)
    : BaseTexture{scene}
    , _textureMatrix{std_util::make_unique<Matrix>(Matrix::Identity())}
{
  if (iUrl.empty()) {
    return;
  }

  name                      = iUrl;
  url                       = iUrl;
  hasAlpha                  = false;
  isCube                    = false;
  wrapU                     = Texture::CLAMP_ADDRESSMODE;
  wrapV                     = Texture::CLAMP_ADDRESSMODE;
  anisotropicFilteringLevel = 1;

  _texture = _getFromCache(url, true);

  if (!_texture) {
    if (!scene->useDelayedTextureLoading) {
      loadTexture();
    }
    else {
      delayLoadState = Engine::DELAYLOADSTATE_NOTLOADED;
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

GL::IGLTexture* ColorGradingTexture::load3dlTexture()
{
  _texture = getScene()->getEngine()->createRawTexture(
    Uint8Array(), 1, 1, Engine::TEXTUREFORMAT_RGBA, false, false,
    Texture::BILINEAR_SAMPLINGMODE);

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
        tempData[pixelStorageIndex + 3] = 0.f;

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
      data[i] = static_cast<std::uint8_t>(tempData[i]
                                          / static_cast<float>(maxColor * 255));
    }

    getScene()->getEngine()->updateTextureSize(
      _texture, static_cast<int>(size * size), static_cast<int>(size));
    getScene()->getEngine()->updateRawTexture(
      _texture, data, Engine::TEXTUREFORMAT_RGBA, false);
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
  auto newTexture = std_util::make_unique<ColorGradingTexture>(url, getScene());

  // Base texture
  newTexture->level = level;

  return newTexture;
}

void ColorGradingTexture::delayLoad()
{
  if (delayLoadState != Engine::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  delayLoadState = Engine::DELAYLOADSTATE_LOADED;
  _texture       = nullptr; // _getFromCache(url, true);

  if (!_texture) {
    loadTexture();
  }
}

void ColorGradingTexture::Bind(BaseTexture* colorGrading, Effect* effect)
{
  effect->setTexture("cameraColorGrading2DSampler", colorGrading);

  float x = colorGrading->level;            // Texture Level
  float y = colorGrading->getSize().height; // Texture Size example with 8
  float z = y - 1.f;                        // SizeMinusOne 8 - 1
  float w = 1.f / y;                        // Space of 1 slice 1 / 8

  effect->setFloat4("vCameraColorGradingInfos", x, y, z, w);

  float slicePixelSizeU = w / y; // Space of 1 pixel in U direction, e.g. 1/64
  float slicePixelSizeV = w;     // Space of 1 pixel in V direction, e.g. 1/8

  // Extent of lookup range in U for a single slice so that range corresponds to
  // (size-1) texels, for example 7/64
  float x2 = z * slicePixelSizeU;
  // Extent of lookup range in V for a single slice so that range corresponds to
  // (size-1) texels, for example 7/8
  float y2 = z / y;
  // Offset of lookup range in U to align sample position with texel centre, for
  // example 0.5/64
  float z2 = 0.5f * slicePixelSizeU;
  // Offset of lookup range in V to align sample position with texel centre, for
  // example 0.5/8
  float w2 = 0.5f * slicePixelSizeV;

  effect->setFloat4("vCameraColorGradingScaleOffset", x2, y2, z2, w2);
}

void ColorGradingTexture::PrepareUniformsAndSamplers(
  std::vector<std::string>& uniformsList,
  std::vector<std::string>& samplersList)
{
  uniformsList.emplace_back("vCameraColorGradingInfos");
  uniformsList.emplace_back("vCameraColorGradingScaleOffset");

  samplersList.emplace_back("cameraColorGrading2DSampler");
}

std::unique_ptr<ColorGradingTexture>
ColorGradingTexture::Parse(const Json::value& parsedTexture, Scene* scene,
                           const std::string& /*rootUrl*/)
{
  std::unique_ptr<ColorGradingTexture> texture = nullptr;
  if (parsedTexture.contains("name")
      && !Json::GetBool(parsedTexture, "isRenderTarget")) {
    auto parsedTextureName = Json::GetString(parsedTexture, "name");
    texture
      = std_util::make_unique<ColorGradingTexture>(parsedTextureName, scene);
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
