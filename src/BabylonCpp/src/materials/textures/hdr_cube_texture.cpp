#include <babylon/materials/textures/hdr_cube_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/tools/hdr/cube_map_to_spherical_polynomial_tools.h>
#include <babylon/tools/hdr/hdr_tools.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

std::vector<std::string> HDRCubeTexture::_facesMapping{
  "right", //
  "left",  //
  "up",    //
  "down",  //
  "front", //
  "back"   //
};

HDRCubeTexture::HDRCubeTexture(
  const std::string& iUrl, Scene* scene, size_t size, bool noMipmap,
  bool generateHarmonics, bool iGammaSpace, bool /*reserved*/,
  const std::function<void()>& onLoad,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError)
    : BaseTexture(scene)
    , url{iUrl}
    , rotationY{this, &HDRCubeTexture::get_rotationY,
                &HDRCubeTexture::set_rotationY}
    , boundingBoxPosition{Vector3::Zero()}
    , _isBlocking{true}
    , _rotationY{0.f}
    , _generateHarmonics{generateHarmonics}
    , _noMipmap{noMipmap}
    , _size{size}
    , _onLoad{onLoad}
    , _onError{onError}
    , _boundingBoxSize{std::nullopt}
{
  if (iUrl.empty()) {
    return;
  }

  name            = iUrl;
  url             = iUrl;
  hasAlpha        = false;
  isCube          = true;
  _textureMatrix  = Matrix::Identity();
  _onLoad         = onLoad;
  _onError        = onError;
  gammaSpace      = iGammaSpace;
  coordinatesMode = TextureConstants::CUBIC_MODE;

  _noMipmap = noMipmap;
  _size     = size;

  _texture = _getFromCache(url, _noMipmap);

  if (!_texture) {
    if (!scene->useDelayedTextureLoading) {
      loadTexture();
    }
    else {
      delayLoadState = EngineConstants::DELAYLOADSTATE_NOTLOADED;
    }
  }
}

HDRCubeTexture::~HDRCubeTexture()
{
}

void HDRCubeTexture::set_isBlocking(bool value)
{
  _isBlocking = value;
}

bool HDRCubeTexture::get_isBlocking() const
{
  return _isBlocking;
}

void HDRCubeTexture::set_rotationY(float value)
{
  _rotationY = value;
  setReflectionTextureMatrix(Matrix::RotationY(_rotationY));
}

float HDRCubeTexture::get_rotationY() const
{
  return _rotationY;
}

void HDRCubeTexture::set_boundingBoxSize(const std::optional<Vector3>& value)
{
  if (!value.has_value()) {
    return;
  }

  if (_boundingBoxSize && (*_boundingBoxSize).equals(*value)) {
    return;
  }
  _boundingBoxSize = value;
  auto scene       = getScene();
  if (scene) {
    scene->markAllMaterialsAsDirty(Material::TextureDirtyFlag());
  }
}

std::optional<Vector3>& HDRCubeTexture::get_boundingBoxSize()
{
  return _boundingBoxSize;
}

void HDRCubeTexture::loadTexture()
{
  const auto callback
    = [this](const ArrayBuffer& buffer) -> std::vector<ArrayBufferView> {
    lodGenerationOffset = 0.f;
    lodGenerationScale  = 0.8f;

    auto scene = getScene();

    if (!scene) {
      return {};
    }
    // Extract the raw linear data.
    auto data = HDRTools::GetCubeMapTextureData(buffer, _size);

    // Generate harmonics if needed.
    if (_generateHarmonics) {
      auto _sphericalPolynomial = CubeMapToSphericalPolynomialTools::
        ConvertCubeMapToSphericalPolynomial(data);
      sphericalPolynomial = _sphericalPolynomial;
    }

    std::vector<ArrayBufferView> results;
    Uint8Array byteArray;

    // Push each faces.
    for (unsigned int j = 0; j < 6; ++j) {

      // Create uintarray fallback.
      if (!scene->getEngine()->getCaps().textureFloat) {
        // 3 channels of 1 bytes per pixel in bytes.
        byteArray.resize(_size * _size * 3);
      }

      auto& dataFace = data[HDRCubeTexture::_facesMapping[j]].float32Array;

      // If special cases.
      if (gammaSpace || !byteArray.empty()) {
        for (size_t i = 0; i < _size * _size; ++i) {

          // Put in gamma space if requested.
          if (gammaSpace) {
            dataFace[(i * 3) + 0]
              = std::pow(dataFace[(i * 3) + 0], Math::ToGammaSpace);
            dataFace[(i * 3) + 1]
              = std::pow(dataFace[(i * 3) + 1], Math::ToGammaSpace);
            dataFace[(i * 3) + 2]
              = std::pow(dataFace[(i * 3) + 2], Math::ToGammaSpace);
          }

          // Convert to int texture for fallback.
          if (!byteArray.empty()) {
            auto r = std::max(dataFace[(i * 3) + 0] * 255, 0.f);
            auto g = std::max(dataFace[(i * 3) + 1] * 255, 0.f);
            auto b = std::max(dataFace[(i * 3) + 2] * 255, 0.f);

            // May use luminance instead if the result is not accurate.
            auto max = std::max(std::max(r, g), b);
            if (max > 255) {
              auto scale = 255.f / max;
              r *= scale;
              g *= scale;
              b *= scale;
            }

            byteArray[(i * 3) + 0] = static_cast<uint8_t>(r);
            byteArray[(i * 3) + 1] = static_cast<uint8_t>(g);
            byteArray[(i * 3) + 2] = static_cast<uint8_t>(b);
          }
        }
      }

      if (!byteArray.empty()) {
        results.emplace_back(byteArray);
      }
      else {
        results.emplace_back(dataFace);
      }
    }
    return results;
  };

  auto scene = getScene();
  if (scene) {
    _texture = scene->getEngine()->createRawCubeTextureFromUrl(
      url, scene, static_cast<int>(_size), EngineConstants::TEXTUREFORMAT_RGB,
      scene->getEngine()->getCaps().textureFloat ?
        EngineConstants::TEXTURETYPE_FLOAT :
        EngineConstants::TEXTURETYPE_UNSIGNED_INT,
      _noMipmap, callback, nullptr, _onLoad, _onError);
  }
}

HDRCubeTexturePtr HDRCubeTexture::clone() const
{
  auto scene = getScene();
  if (!scene) {
    return nullptr;
  }

  auto newTexture = HDRCubeTexture::New(url, scene, _size, _noMipmap,
                                        _generateHarmonics, gammaSpace);

  // Base texture
  newTexture->level            = level;
  newTexture->wrapU            = wrapU;
  newTexture->wrapV            = wrapV;
  newTexture->coordinatesIndex = coordinatesIndex;
  newTexture->coordinatesMode  = coordinatesMode();

  return newTexture;
}

void HDRCubeTexture::delayLoad()
{
  if (delayLoadState != EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  delayLoadState = EngineConstants::DELAYLOADSTATE_LOADED;
  _texture       = _getFromCache(url, _noMipmap);

  if (!_texture) {
    loadTexture();
  }
}

Matrix* HDRCubeTexture::getReflectionTextureMatrix()
{
  return &_textureMatrix;
}

void HDRCubeTexture::setReflectionTextureMatrix(const Matrix& value)
{
  _textureMatrix = value;
}

HDRCubeTexture* HDRCubeTexture::Parse(const json& /*parsedTexture*/,
                                      Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
  return nullptr;
}

json HDRCubeTexture::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
