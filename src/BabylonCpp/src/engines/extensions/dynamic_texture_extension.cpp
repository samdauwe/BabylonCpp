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
    width  = _this->needPOTTextures() ?
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
                                                   ICanvas* /*source*/, std::optional<bool> invertY,
                                                   bool premulAlpha,
                                                   std::optional<unsigned int> /*format*/,
                                                   bool forceBindTexture)
{
  if (!texture) {
    return;
  }

  const auto target = GL::TEXTURE_2D;

  const auto wasPreviouslyBound
    = _this->_bindTextureDirectly(target, texture, true, forceBindTexture);

  _this->_unpackFlipY(invertY.value_or(texture->invertY));

  if (premulAlpha) {
    _this->_gl->pixelStorei(GL::UNPACK_PREMULTIPLY_ALPHA_WEBGL, 1);
  }

#if 0
  const auto textureType    = _this->_getWebGLTextureType(texture->type);
  const auto glformat       = _this->_getInternalFormat(format.value_or(texture->format));
  const auto internalFormat = _this->_getRGBABufferInternalSizedFormat(texture->type, glformat);
  _this->_gl->texImage2D(target, 0, internalFormat, glformat, textureType, source);
#endif

  if (texture->generateMipMaps) {
    _this->_gl->generateMipmap(target);
  }

  if (!wasPreviouslyBound) {
    _this->_bindTextureDirectly(target, nullptr);
  }

  if (premulAlpha) {
    _this->_gl->pixelStorei(GL::UNPACK_PREMULTIPLY_ALPHA_WEBGL, 0);
  }

  texture->isReady = true;
}

} // end of namespace BABYLON
