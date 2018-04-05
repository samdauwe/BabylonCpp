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
  const string_t& iUrl, Scene* scene, const Nullable<size_t>& size,
  bool noMipmap, bool generateHarmonics, bool useInGammaSpace,
  bool usePMREMGenerator, const ::std::function<void()>& onLoad,
  const ::std::function<void(const string_t& message,
                             const string_t& exception)>& onError)
    : BaseTexture(scene)
    , url{iUrl}
    , coordinatesMode{TextureConstants::CUBIC_MODE}
    , isPMREM{false}
    , isBlocking{this, &HDRCubeTexture::get_isBlocking,
                 &HDRCubeTexture::set_isBlocking}
    , boundingBoxPosition{Vector3::Zero()}
    , boundingBoxSize{this, &HDRCubeTexture::get_boundingBoxSize,
                      &HDRCubeTexture::set_boundingBoxSize}
    , _isBlocking{true}
    , _useInGammaSpace{false}
    , _generateHarmonics{generateHarmonics}
    , _noMipmap{noMipmap}
    , _size{size}
    , _usePMREMGenerator{usePMREMGenerator}
    , _isBABYLONPreprocessed{false}
    , _onLoad{onLoad}
    , _onError{onError}
    , _boundingBoxSize{nullptr}
{
  if (iUrl.empty()) {
    return;
  }

  name = url;
  url  = url;
  setHasAlpha(false);
  isCube         = true;
  _textureMatrix = Matrix::Identity();
  _onLoad        = onLoad;
  _onError       = onError;
  gammaSpace     = false;

  auto caps = scene->getEngine()->getCaps();

  if (size) {
    _isBABYLONPreprocessed = false;
    _noMipmap              = noMipmap;
    _size                  = size;
    _useInGammaSpace       = useInGammaSpace;
    _usePMREMGenerator     = usePMREMGenerator && caps.textureLOD
                         && caps.textureFloat && !_useInGammaSpace;
  }
  else {
    _isBABYLONPreprocessed = true;
    _noMipmap              = false;
    _useInGammaSpace       = false;
    _usePMREMGenerator
      = caps.textureLOD && caps.textureFloat && !_useInGammaSpace;
  }
  isPMREM = _usePMREMGenerator;

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

Float32Array HDRCubeTexture::loadBabylonTexture()
{
  return Float32Array();
}

Float32Array HDRCubeTexture::loadHDRTexture()
{
  return Float32Array();
}

void HDRCubeTexture::loadTexture()
{
  if (_isBABYLONPreprocessed) {
    loadBabylonTexture();
  }
  else {
    loadHDRTexture();
  }
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

void HDRCubeTexture::generateBabylonHDROnDisk(
  const string_t& /*url*/, size_t /*size*/,
  const ::std::function<void()>& /*onError*/)
{
}

void HDRCubeTexture::generateBabylonHDR(
  const string_t& url, size_t size,
  const ::std::function<void(const ArrayBuffer& arrayBuffer)>& /*callback*/,
  const ::std::function<void()>& /*onError*/)
{
  // Needs the url tho create the texture.
  if (url.empty()) {
    return;
  }

  // Check Power of two size.
  if (!Tools::IsExponentOfTwo(size)) { // Need to check engine.needPOTTextures
    return;
  }

  // Coming Back in 3.x.
  BABYLON_LOG_ERROR("HDRCubeTexture",
                    "Generation of Babylon HDR is coming back in 3.2.");
}

} // end of namespace BABYLON
