#include <babylon/materials/textures/texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/mesh/buffer.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

constexpr unsigned int Texture::NEAREST_SAMPLINGMODE;
constexpr unsigned int Texture::BILINEAR_SAMPLINGMODE;
constexpr unsigned int Texture::TRILINEAR_SAMPLINGMODE;

Texture::Texture(const std::string& _url, Scene* scene, bool noMipmap,
                 bool invertY, unsigned int samplingMode,
                 const std::function<void()>& onLoad,
                 const std::function<void()>& onError, Buffer* buffer,
                 bool deleteBuffer, unsigned int format)
    : BaseTexture{scene}
    , url{_url}
    , uOffset{0.f}
    , vOffset{0.f}
    , uScale{1.0}
    , vScale{1.0}
    , uAng{0.f}
    , vAng{0.f}
    , wAng{0.f}
    , _invertY{invertY}
    , _samplingMode{samplingMode}
    , _format{format}
    , _noMipmap{noMipmap}
    , _rowGenerationMatrix{nullptr}
    , _cachedTextureMatrix{nullptr}
    , _projectionModeMatrix{nullptr}
    , _buffer{buffer}
    , _deleteBuffer{deleteBuffer}
    , _onLoad{onLoad}
{
  name = _url;

  if (url.empty()) {
    return;
  }

  _texture = _getFromCache(url, noMipmap, samplingMode);

  _load = [this]() {
    if (_onLoadObservable.hasObservers()) {
      _onLoadObservable.notifyObservers();
    }
    if (_onLoad) {
      _onLoad();
    }
  };

  if (!_texture) {
    if (!scene->useDelayedTextureLoading) {
      _texture = scene->getEngine()->createTexture(
        url, noMipmap, invertY, scene, _samplingMode, _load, onError, _buffer,
        nullptr, _format);
      if (deleteBuffer) {
        delete _buffer;
      }
    }
    else {
      delayLoadState = Engine::DELAYLOADSTATE_NOTLOADED;

      _delayedOnLoad  = _load;
      _delayedOnError = onError;
    }
  }
  else {
    if (_texture->isReady) {
      Tools::SetImmediate([this]() { _load(); });
    }
    else {
      _texture->onLoadedCallbacks.emplace_back(_load);
    }
  }
}

Texture::~Texture()
{
}

IReflect::Type Texture::type() const
{
  return IReflect::Type::TEXTURE;
}

bool Texture::noMipmap() const
{
  return _noMipmap;
}

void Texture::delayLoad()
{
  if (delayLoadState != Engine::DELAYLOADSTATE_NOTLOADED) {
    return;
  }

  delayLoadState = Engine::DELAYLOADSTATE_LOADED;
  _texture       = _getFromCache(url, _noMipmap, _samplingMode);

  if (!_texture) {
    _texture = getScene()->getEngine()->createTexture(
      url, _noMipmap, _invertY, getScene(), _samplingMode, _delayedOnLoad,
      _delayedOnError, _buffer, nullptr, _format);
    if (_deleteBuffer) {
      delete _buffer;
    }
  }
}

void Texture::updateSamplingMode(unsigned int samplingMode)
{
  if (!_texture) {
    return;
  }

  _samplingMode = samplingMode;
  getScene()->getEngine()->updateTextureSamplingMode(samplingMode, _texture);
}

void Texture::_prepareRowForTextureGeneration(float x, float y, float z,
                                              Vector3& t)
{
  x *= uScale;
  y *= vScale;

  x -= 0.5f * uScale;
  y -= 0.5f * vScale;
  z -= 0.5f;

  Vector3::TransformCoordinatesFromFloatsToRef(x, y, z, *_rowGenerationMatrix,
                                               t);

  t.x += 0.5f * uScale + uOffset;
  t.y += 0.5f * vScale + vOffset;
  t.z += 0.5f;
}

Matrix* Texture::getTextureMatrix()
{
  if (stl_util::almost_equal(uOffset, _cachedUOffset)
      && stl_util::almost_equal(vOffset, _cachedVOffset)
      && stl_util::almost_equal(uScale, _cachedUScale)
      && stl_util::almost_equal(vScale, _cachedVScale)
      && stl_util::almost_equal(uAng, _cachedUAng)
      && stl_util::almost_equal(vAng, _cachedVAng)
      && stl_util::almost_equal(wAng, _cachedWAng)) {
    return _cachedTextureMatrix.get();
  }

  _cachedUOffset = uOffset;
  _cachedVOffset = vOffset;
  _cachedUScale  = uScale;
  _cachedVScale  = vScale;
  _cachedUAng    = uAng;
  _cachedVAng    = vAng;
  _cachedWAng    = wAng;

  if (!_cachedTextureMatrix) {
    _cachedTextureMatrix = std::make_unique<Matrix>(Matrix::Zero());
    _rowGenerationMatrix = std::make_unique<Matrix>();
    _t0                  = Vector3::Zero();
    _t1                  = Vector3::Zero();
    _t2                  = Vector3::Zero();
  }

  Matrix::RotationYawPitchRollToRef(vAng, uAng, wAng, *_rowGenerationMatrix);

  _prepareRowForTextureGeneration(0.f, 0.f, 0.f, _t0);
  _prepareRowForTextureGeneration(1.f, 0.f, 0.f, _t1);
  _prepareRowForTextureGeneration(0.f, 1.f, 0.f, _t2);

  _t1.subtractInPlace(_t0);
  _t2.subtractInPlace(_t0);

  Matrix::IdentityToRef(*_cachedTextureMatrix);
  _cachedTextureMatrix->m[0] = _t1.x;
  _cachedTextureMatrix->m[1] = _t1.y;
  _cachedTextureMatrix->m[2] = _t1.z;

  _cachedTextureMatrix->m[4] = _t2.x;
  _cachedTextureMatrix->m[5] = _t2.y;
  _cachedTextureMatrix->m[6] = _t2.z;

  _cachedTextureMatrix->m[8]  = _t0.x;
  _cachedTextureMatrix->m[9]  = _t0.y;
  _cachedTextureMatrix->m[10] = _t0.z;

  return _cachedTextureMatrix.get();
}

Matrix* Texture::getReflectionTextureMatrix()
{
  if (stl_util::almost_equal(uOffset, _cachedUOffset)
      && stl_util::almost_equal(vOffset, _cachedVOffset)
      && stl_util::almost_equal(uScale, _cachedUScale)
      && stl_util::almost_equal(vScale, _cachedVScale)
      && (coordinatesMode() == _cachedCoordinatesMode)) {
    return _cachedTextureMatrix.get();
  }

  if (!_cachedTextureMatrix) {
    _cachedTextureMatrix  = std::make_unique<Matrix>(Matrix::Zero());
    _projectionModeMatrix = std::make_unique<Matrix>(Matrix::Zero());
  }

  _cachedCoordinatesMode = coordinatesMode();

  switch (coordinatesMode()) {
    case Texture::PLANAR_MODE:
      Matrix::IdentityToRef(*_cachedTextureMatrix);
      _cachedTextureMatrix->m[0]  = uScale;
      _cachedTextureMatrix->m[5]  = vScale;
      _cachedTextureMatrix->m[12] = uOffset;
      _cachedTextureMatrix->m[13] = vOffset;
      break;
    case Texture::PROJECTION_MODE:
      Matrix::IdentityToRef(*_projectionModeMatrix);

      _projectionModeMatrix->m[0]  = 0.5f;
      _projectionModeMatrix->m[5]  = -0.5f;
      _projectionModeMatrix->m[10] = 0.f;
      _projectionModeMatrix->m[12] = 0.5f;
      _projectionModeMatrix->m[13] = 0.5f;
      _projectionModeMatrix->m[14] = 1.f;
      _projectionModeMatrix->m[15] = 1.f;

      getScene()->getProjectionMatrix().multiplyToRef(*_projectionModeMatrix,
                                                      *_cachedTextureMatrix);
      break;
    default:
      Matrix::IdentityToRef(*_cachedTextureMatrix);
      break;
  }
  return _cachedTextureMatrix.get();
}

Texture* Texture::clone() const
{
  auto newTexture = Texture::New(_texture->url, getScene(), _noMipmap, _invertY,
                                 _samplingMode);

  // Base texture
  newTexture->setHasAlpha(hasAlpha());
  newTexture->level            = level;
  newTexture->wrapU            = wrapU;
  newTexture->wrapV            = wrapV;
  newTexture->coordinatesIndex = coordinatesIndex;
  newTexture->setCoordinatesMode(coordinatesMode());

  // Texture
  newTexture->uOffset = uOffset;
  newTexture->vOffset = vOffset;
  newTexture->uScale  = uScale;
  newTexture->vScale  = vScale;
  newTexture->uAng    = uAng;
  newTexture->vAng    = vAng;
  newTexture->wAng    = wAng;

  return newTexture;
}

Texture* Texture::CreateFromBase64String(const std::string& /*data*/,
                                         const std::string& name, Scene* scene,
                                         bool noMipmap, bool invertY,
                                         unsigned int samplingMode,
                                         const std::function<void()>& onLoad,
                                         const std::function<void()>& onError,
                                         unsigned int format)
{
  return Texture::New("data:" + name, scene, noMipmap, invertY, samplingMode,
                      onLoad, onError, nullptr, false, format);
}

std::unique_ptr<BaseTexture>
Texture::Parse(const Json::value& /*parsedTexture*/, Scene* /*scene*/,
               const std::string& /*rootUrl*/)
{
  return nullptr;
}

Texture* Texture::LoadFromDataString(const std::string& name, Buffer* buffer,
                                     Scene* scene, bool deleteBuffer,
                                     bool noMipmap, bool invertY,
                                     unsigned int samplingMode,
                                     const std::function<void()>& onLoad,
                                     const std::function<void()>& onError,
                                     unsigned int format)
{
  std::string _name = name;
  if (_name.substr(0, 5) != "data:") {
    _name = "data:" + name;
  }

  return Texture::New(name, scene, noMipmap, invertY, samplingMode, onLoad,
                      onError, buffer, deleteBuffer, format);
}

} // end of namespace BABYLON
