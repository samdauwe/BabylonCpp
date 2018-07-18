#include <babylon/materials/textures/hdr_cube_texture.h>

#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

vector_t<string_t> HDRCubeTexture::_facesMapping{
  "right", //
  "left",  //
  "up",    //
  "down",  //
  "front", //
  "back"   //
};

HDRCubeTexture::HDRCubeTexture(
  const string_t& iUrl, Scene* scene, size_t size, bool noMipmap,
  bool generateHarmonics, bool iGammaSpace, bool /*reserved*/,
  const ::std::function<void()>& onLoad,
  const ::std::function<void(const string_t& message,
                             const string_t& exception)>& onError)
    : BaseTexture(scene)
    , url{iUrl}
    , coordinatesMode{TextureConstants::CUBIC_MODE}
    , isBlocking{this, &HDRCubeTexture::get_isBlocking,
                 &HDRCubeTexture::set_isBlocking}
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
    , _boundingBoxSize{nullptr}
{
  if (iUrl.empty()) {
    return;
  }

  name           = url;
  url            = url;
  hasAlpha       = false;
  isCube         = true;
  _textureMatrix = Matrix::Identity();
  _onLoad        = onLoad;
  _onError       = onError;
  gammaSpace     = iGammaSpace;

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

void HDRCubeTexture::set_boundingBoxSize(const Nullable<Vector3>& value)
{
  if (!value) {
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

Nullable<Vector3>& HDRCubeTexture::get_boundingBoxSize()
{
  return _boundingBoxSize;
}

Float32Array HDRCubeTexture::loadTexture()
{
  return Float32Array();
}

HDRCubeTexture* HDRCubeTexture::clone()
{
  return nullptr;
}

void HDRCubeTexture::delayLoad()
{
}

Matrix* HDRCubeTexture::getReflectionTextureMatrix()
{
  return &_textureMatrix;
}

void HDRCubeTexture::setReflectionTextureMatrix(const Matrix& value)
{
  _textureMatrix = value;
}

HDRCubeTexture* HDRCubeTexture::Parse(const Json::value& /*parsedTexture*/,
                                      Scene* /*scene*/,
                                      const string_t& /*rootUrl*/)
{
  return nullptr;
}

Json::object HDRCubeTexture::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
