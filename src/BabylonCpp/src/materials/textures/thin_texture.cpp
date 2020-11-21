#include <babylon/materials/textures/thin_texture.h>

#include <babylon/engines/constants.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/maths/size.h>

namespace BABYLON {

ThinTexture::ThinTexture(const InternalTexturePtr& internalTexture)
    : wrapU{this, &ThinTexture::get_wrapU, &ThinTexture::set_wrapU}
    , wrapV{this, &ThinTexture::get_wrapV, &ThinTexture::set_wrapV}
    , wrapR{Constants::TEXTURE_WRAP_ADDRESSMODE}
    , anisotropicFilteringLevel{4u}
    , delayLoadState{Constants::DELAYLOADSTATE_NONE}
    , coordinatesMode{this, &ThinTexture::get_coordinatesMode, &ThinTexture::set_coordinatesMode}
    , isCube{this, &ThinTexture::get_isCube, &ThinTexture::set_isCube}
    , is3D{this, &ThinTexture::get_is3D, &ThinTexture::set_is3D}
    , is2DArray{this, &ThinTexture::get_is2DArray, &ThinTexture::set_is2DArray}
    , _texture{nullptr}
    , _wrapU{Constants::TEXTURE_WRAP_ADDRESSMODE}
    , _wrapV{Constants::TEXTURE_WRAP_ADDRESSMODE}
    , _engine{nullptr}
    , _cachedSize{Size::Zero()}
    , _cachedBaseSize{Size::Zero()}
{
  _texture = internalTexture;
  if (_texture) {
    _engine = _texture->getEngine();
  }
}

ThinTexture::~ThinTexture() = default;

unsigned int ThinTexture::get_wrapU() const
{
  return _wrapU;
}

void ThinTexture::set_wrapU(unsigned int value)
{
  _wrapU = value;
}

unsigned int ThinTexture::get_wrapV() const
{
  return _wrapV;
}

void ThinTexture::set_wrapV(unsigned int value)
{
  _wrapV = value;
}

unsigned int ThinTexture::get_coordinatesMode() const
{
  return 0;
}

void ThinTexture::set_coordinatesMode(unsigned int /*value*/)
{
}

bool ThinTexture::get_isCube() const
{
  if (!_texture) {
    return false;
  }

  return _texture->isCube;
}

void ThinTexture::set_isCube(bool value)
{
  if (!_texture) {
    return;
  }

  _texture->isCube = value;
}

bool ThinTexture::get_is3D() const
{
  if (!_texture) {
    return false;
  }

  return _texture->is3D;
}

void ThinTexture::set_is3D(bool value)
{
  if (!_texture) {
    return;
  }

  _texture->is3D = value;
}

bool ThinTexture::get_is2DArray() const
{
  if (!_texture) {
    return false;
  }

  return _texture->is2DArray;
}

void ThinTexture::set_is2DArray(bool value)
{
  if (!_texture) {
    return;
  }

  _texture->is2DArray = value;
}

std::string ThinTexture::getClassName() const
{
  return "ThinTexture";
}

bool ThinTexture::isReady()
{
  if (delayLoadState == Constants::DELAYLOADSTATE_NOTLOADED) {
    delayLoad();
    return false;
  }

  if (_texture) {
    return _texture->isReady;
  }

  return false;
}

void ThinTexture::delayLoad(const std::string& /*forcedExtension*/)
{
}

InternalTexturePtr& ThinTexture::getInternalTexture()
{
  return _texture;
}

ISize& ThinTexture::getSize()
{
  if (_texture) {
    if (_texture->width) {
      _cachedSize.width  = _texture->width;
      _cachedSize.height = _texture->height;
      return _cachedSize;
    }

    if (_texture->_size) {
      _cachedSize.width  = _texture->_size;
      _cachedSize.height = _texture->_size;
      return _cachedSize;
    }
  }

  return _cachedSize;
}

ISize& ThinTexture::getBaseSize()
{
  if (!isReady() || !_texture) {
    _cachedBaseSize.width  = 0;
    _cachedBaseSize.height = 0;
    return _cachedBaseSize;
  }

  if (_texture->_size) {
    _cachedBaseSize.width  = _texture->_size;
    _cachedBaseSize.height = _texture->_size;
    return _cachedBaseSize;
  }

  _cachedBaseSize.width  = _texture->baseWidth;
  _cachedBaseSize.height = _texture->baseHeight;
  return _cachedBaseSize;
}

void ThinTexture::updateSamplingMode(unsigned int samplingMode)
{
  if (_texture && _engine) {
    _engine->updateTextureSamplingMode(samplingMode, _texture);
  }
}

void ThinTexture::releaseInternalTexture()
{
  if (_texture) {
    _texture->dispose();
    _texture = nullptr;
  }
}

void ThinTexture::dispose()
{
  if (_texture) {
    releaseInternalTexture();
    _engine = nullptr;
  }
}

} // end of namespace BABYLON
