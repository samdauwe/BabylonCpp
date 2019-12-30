#include <babylon/engines/extensions/dynamic_texture_extension.h>

#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

DynamicTextureExtension::DynamicTextureExtension(ThinEngine* engine) : _this{engine}
{
}

DynamicTextureExtension::~DynamicTextureExtension() = default;

InternalTexturePtr DynamicTextureExtension::createDynamicTexture(int width, int height,
                                                                 bool generateMipMaps,
                                                                 unsigned int samplingMode)
{
  auto texture        = InternalTexture::New(_this, InternalTextureSource::Dynamic);
  texture->baseWidth  = width;
  texture->baseHeight = height;

  if (generateMipMaps) {
    width = _this->needPOTTextures() ?
              ThinEngine::GetExponentOfTwo(width, _this->_caps.maxTextureSize) :
              width;
    height = _this->needPOTTextures() ?
               ThinEngine::GetExponentOfTwo(height, _this->_caps.maxTextureSize) :
               height;
  }

  // _this->resetTextureCache();
  texture->width           = width;
  texture->height          = height;
  texture->isReady         = false;
  texture->generateMipMaps = generateMipMaps;
  texture->samplingMode    = samplingMode;

  _this->updateTextureSamplingMode(samplingMode, texture);

  _this->_internalTexturesCache.emplace_back(texture);

  return texture;
}

void DynamicTextureExtension::updateDynamicTexture(const InternalTexturePtr& texture,
                                                   ICanvas* /*canvas*/, bool invertY,
                                                   bool premulAlpha,
                                                   std::optional<unsigned int> /*format */,
                                                   bool forceBindTexture)
{
  if (!texture) {
    return;
  }

  _this->_bindTextureDirectly(GL::TEXTURE_2D, texture, true, forceBindTexture);
  _this->_unpackFlipY(invertY);
  if (premulAlpha) {
    _this->_gl->pixelStorei(GL::UNPACK_PREMULTIPLY_ALPHA_WEBGL, 1);
  }
#if 0
  auto internalFormat = format ? _getInternalFormat(format) : GL::RGBA;
  _gl->texImage2D(GL::TEXTURE_2D, 0, static_cast<int>(internalFormat),
                  internalFormat, GL::UNSIGNED_BYTE, canvas);
#endif
  if (texture->generateMipMaps) {
    _this->_gl->generateMipmap(GL::TEXTURE_2D);
  }
  _this->_bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  if (premulAlpha) {
    _this->_gl->pixelStorei(GL::UNPACK_PREMULTIPLY_ALPHA_WEBGL, 0);
  }
  texture->isReady = true;
}

} // end of namespace BABYLON
