#include <babylon/materials/textures/color_grading_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

ColorGradingTexture::ColorGradingTexture(const std::string& iUrl,
                                         const std::variant<Scene*, ThinEngine*>& sceneOrEngine,
                                         const std::function<void()>& onLoad)
    : BaseTexture{ColorGradingTexture::_isScene(sceneOrEngine) ? std::get<Scene*>(sceneOrEngine) :
                                                                 nullptr}
    , _onLoad{nullptr}
{
  if (iUrl.empty()) {
    return;
  }

  _textureMatrix = std::make_unique<Matrix>(Matrix::Identity());
  name           = iUrl;
  url            = iUrl;
  _onLoad        = onLoad;

  _texture = _getFromCache(url, true);

  if (!_texture) {
    if (ColorGradingTexture::_isScene(sceneOrEngine)) {
      _engine = std::get<Scene*>(sceneOrEngine)->getEngine();

      if (!std::get<Scene*>(sceneOrEngine)->useDelayedTextureLoading) {
        loadTexture();
      }
      else {
        delayLoadState = Constants::DELAYLOADSTATE_NOTLOADED;
      }
    }
    else {
      _engine = std::get<ThinEngine*>(sceneOrEngine);
      loadTexture();
    }
  }
  else {
    _engine = _texture->getEngine();
    _triggerOnLoad();
  }
}

ColorGradingTexture::~ColorGradingTexture() = default;

void ColorGradingTexture::_triggerOnLoad()
{
  if (_onLoad) {
    _onLoad();
  }
}

Matrix* ColorGradingTexture::getTextureMatrix(int /*uBase*/)
{
  return _textureMatrix.get();
}

InternalTexturePtr ColorGradingTexture::load3dlTexture()
{
  InternalTexturePtr texture = nullptr;
  if (_engine->webGLVersion() == 1.f) {
    texture = _engine->createRawTexture(Uint8Array(), 1, 1, Constants::TEXTUREFORMAT_RGBA, false,
                                        false, TextureConstants::BILINEAR_SAMPLINGMODE, "",
                                        Constants::TEXTURETYPE_UNSIGNED_INT);
  }
  else {
    texture = _engine->createRawTexture3D(Uint8Array(), 1, 1, 1, Constants::TEXTUREFORMAT_RGBA,
                                          false, false, TextureConstants::BILINEAR_SAMPLINGMODE, "",
                                          Constants::TEXTURETYPE_UNSIGNED_INT);
  }

  _texture = texture;

  _texture          = texture;
  _texture->isReady = false;

  isCube                    = false;
  is3D                      = _engine->webGLVersion() > 1.f;
  wrapU                     = Constants::TEXTURE_CLAMP_ADDRESSMODE;
  wrapV                     = Constants::TEXTURE_CLAMP_ADDRESSMODE;
  wrapR                     = Constants::TEXTURE_CLAMP_ADDRESSMODE;
  anisotropicFilteringLevel = 1u;

  const auto callback = [=](const std::variant<std::string, ArrayBufferView>& iText,
                            const std::string & /*onSuccess*/) -> void {
    if (!std::holds_alternative<std::string>(iText)) {
      return;
    }

    auto text = std::get<std::string>(iText);

    Uint8Array data;
    Float32Array tempData;

    auto lines = StringTools::split(text, '\n');
    auto size = 0ull, pixelIndexW = 0ull, pixelIndexH = 0ull, pixelIndexSlice = 0ull;
    auto maxColor = 0;

    for (auto& line : lines) {

      if (line.empty()) {
        continue;
      }

      if (line.at(0) == '#') {
        continue;
      }

      auto words = StringTools::split(line, ' ');
      if (size == 0) {
        // Number of space + one
        size = words.size();
        data.clear();
        data.resize(size * size * size * 4); // volume texture of side size and rgb 8
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
          = (pixelIndexW + pixelIndexSlice * size + pixelIndexH * size * size) * 4;

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
      _engine->updateRawTexture3D(_texture, data, Constants::TEXTUREFORMAT_RGBA, false);
    }
    else {
      _texture->updateSize(_size * _size, _size);
      _engine->updateRawTexture(_texture, data, Constants::TEXTUREFORMAT_RGBA, false);
    }

    texture->isReady = true;
    _triggerOnLoad();
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
  if (!url.empty() && StringTools::endsWith(StringTools::toLowerCase(url), ".3dl")) {
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

std::unique_ptr<ColorGradingTexture> ColorGradingTexture::Parse(const json& /*parsedTexture*/,
                                                                Scene* /*scene*/,
                                                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

json ColorGradingTexture::serialize() const
{
  return nullptr;
}

bool ColorGradingTexture::_isScene(const std::variant<Scene*, ThinEngine*>& sceneOrEngine)
{
  return std::holds_alternative<Scene*>(sceneOrEngine);
}

} // end of namespace BABYLON
