#include <babylon/engines/extensions/render_target_extension.h>

#include <babylon/core/logging.h>
#include <babylon/engines/depth_texture_creation_options.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/render_target_creation_options.h>

namespace BABYLON {

RenderTargetExtension::RenderTargetExtension(ThinEngine* engine) : _this{engine}
{
}

RenderTargetExtension::~RenderTargetExtension() = default;

InternalTexturePtr RenderTargetExtension::createRenderTargetTexture(
  const std::variant<int, RenderTargetSize, float>& size, const IRenderTargetOptions& options)
{
  RenderTargetCreationOptions fullOptions;
  fullOptions.generateMipMaps = options.generateMipMaps;
  fullOptions.generateDepthBuffer
    = !options.generateDepthBuffer.has_value() ? true : options.generateDepthBuffer;
  fullOptions.generateStencilBuffer
    = fullOptions.generateDepthBuffer.value() && options.generateStencilBuffer.value_or(false);
  fullOptions.type = options.type.value_or(Constants::TEXTURETYPE_UNSIGNED_INT);
  fullOptions.samplingMode
    = options.samplingMode.value_or(Constants::TEXTURE_TRILINEAR_SAMPLINGMODE);
  fullOptions.format = options.format.value_or(Constants::TEXTUREFORMAT_RGBA);

  if (fullOptions.type.value() == Constants::TEXTURETYPE_FLOAT
      && !_this->_caps.textureFloatLinearFiltering) {
    // if floating point linear (_gl->FLOAT) then force to NEAREST_SAMPLINGMODE
    fullOptions.samplingMode = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
  }
  else if (fullOptions.type.value() == Constants::TEXTURETYPE_HALF_FLOAT
           && !_this->_caps.textureHalfFloatLinearFiltering) {
    // if floating point linear (HALF_FLOAT) then force to NEAREST_SAMPLINGMODE
    fullOptions.samplingMode = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
  }

  if (fullOptions.type.value() == Constants::TEXTURETYPE_FLOAT && !_this->_caps.textureFloat) {
    fullOptions.type = Constants::TEXTURETYPE_UNSIGNED_INT;
    BABYLON_LOG_WARN(
      "RenderTargetExtension",
      "Float textures are not supported. Render target forced to TEXTURETYPE_UNSIGNED_BYTE type")
  }

  auto& gl         = *_this->_gl;
  auto texture     = InternalTexture::New(_this, InternalTextureSource::RenderTarget);
  const auto width = std::holds_alternative<RenderTargetSize>(size) ?
                                                           std::get<RenderTargetSize>(size).width :
                     std::holds_alternative<int>(size)   ? std::get<int>(size) :
                     std::holds_alternative<float>(size) ? static_cast<int>(std::get<float>(size)) :
                                                           0;
  const auto height
    = std::holds_alternative<RenderTargetSize>(size) ? std::get<RenderTargetSize>(size).height :
      std::holds_alternative<int>(size)              ? std::get<int>(size) :
      std::holds_alternative<float>(size)            ? static_cast<int>(std::get<float>(size)) :
                                                       0;
  const auto layers      = std::holds_alternative<RenderTargetSize>(size) ?
                             std::get<RenderTargetSize>(size).layers.value_or(0) :
                             0;
  const auto filters     = _this->_getSamplingParameters(*fullOptions.samplingMode,
                                                     fullOptions.generateMipMaps.value_or(false));
  const auto target      = layers != 0 ? GL::TEXTURE_2D_ARRAY : GL::TEXTURE_2D;
  const auto sizedFormat = static_cast<int>(
    _this->_getRGBABufferInternalSizedFormat(*fullOptions.type, fullOptions.format));
  const auto internalFormat = _this->_getInternalFormat(*fullOptions.format);
  const auto type           = _this->_getWebGLTextureType(*fullOptions.type);

  // Bind
  _this->_bindTextureDirectly(target, texture);

  if (layers != 0) {
    texture->is2DArray = true;
    gl.texImage3D(target, 0, sizedFormat, width, height, layers, 0, internalFormat, type, {});
  }
  else {
    gl.texImage2D(target, 0, sizedFormat, width, height, 0, internalFormat, type, {});
  }

  gl.texParameteri(target, GL::TEXTURE_MAG_FILTER, filters.mag);
  gl.texParameteri(target, GL::TEXTURE_MIN_FILTER, filters.min);
  gl.texParameteri(target, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
  gl.texParameteri(target, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

  // MipMaps
  if (fullOptions.generateMipMaps) {
    gl.generateMipmap(target);
  }

  _this->_bindTextureDirectly(target, nullptr);

  const auto currentFrameBuffer = _this->_currentFramebuffer;

  // Create the framebuffer
  auto framebuffer = gl.createFramebuffer();
  _this->_bindUnboundFramebuffer(framebuffer);
  texture->_depthStencilBuffer = _this->_setupFramebufferDepthAttachments(
    fullOptions.generateStencilBuffer.value(), fullOptions.generateDepthBuffer.value(), width,
    height);

  // No need to rebind on every frame
  if (!texture->is2DArray) {
    gl.framebufferTexture2D(
      GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0, GL::TEXTURE_2D,
      texture->_hardwareTexture ? texture->_hardwareTexture->underlyingResource().get() : nullptr,
      0);
  }

  _this->_bindUnboundFramebuffer(currentFrameBuffer);
  texture->_framebuffer           = std::move(framebuffer);
  texture->baseWidth              = width;
  texture->baseHeight             = height;
  texture->width                  = width;
  texture->height                 = height;
  texture->depth                  = layers;
  texture->isReady                = true;
  texture->samples                = 1;
  texture->generateMipMaps        = fullOptions.generateMipMaps.value();
  texture->samplingMode           = fullOptions.samplingMode.value();
  texture->type                   = fullOptions.type.value();
  texture->format                 = fullOptions.format.value();
  texture->_generateDepthBuffer   = fullOptions.generateDepthBuffer.value();
  texture->_generateStencilBuffer = fullOptions.generateStencilBuffer.value();

  _this->_internalTexturesCache.emplace_back(texture);

  return texture;
}

InternalTexturePtr
RenderTargetExtension::createDepthStencilTexture(const RenderTargetTextureSize& size,
                                                 const DepthTextureCreationOptions& options)
{
  if (options.isCube.value_or(false)) {
    const auto width = std::holds_alternative<int>(size) ?
                         std::get<int>(size) :
                       std::holds_alternative<RenderTargetSize>(size) ?
                         std::get<RenderTargetSize>(size).width :
                         static_cast<int>(std::holds_alternative<float>(size));
    return _this->_createDepthStencilCubeTexture(width, options);
  }
  else {
    return _this->_createDepthStencilTexture(size, options);
  }
}

InternalTexturePtr
RenderTargetExtension::_createDepthStencilTexture(const RenderTargetTextureSize& size,
                                                  const DepthTextureCreationOptions& options)
{
  auto& gl             = *_this->_gl;
  const auto layers    = std::holds_alternative<RenderTargetSize>(size) ?
                           std::get<RenderTargetSize>(size).layers.value_or(0) :
                           0;
  const auto target    = layers != 0 ? GL::TEXTURE_2D_ARRAY : GL::TEXTURE_2D;
  auto internalTexture = InternalTexture::New(_this, InternalTextureSource::Depth);

  if (!_this->_caps.depthTextureExtension) {
    BABYLON_LOG_ERROR("RenderTargetExtension",
                      "Depth texture is not supported by your browser or hardware.")
    return internalTexture;
  }

  auto internalOptions               = options;
  internalOptions.bilinearFiltering  = options.bilinearFiltering.value_or(false);
  internalOptions.comparisonFunction = options.comparisonFunction.value_or(0);
  internalOptions.generateStencil    = options.generateStencil.value_or(false);

  _this->_bindTextureDirectly(target, internalTexture, true);

  _this->_setupDepthStencilTexture(internalTexture, size, *internalOptions.generateStencil,
                                   *internalOptions.bilinearFiltering,
                                   *internalOptions.comparisonFunction);

  const auto type
    = internalOptions.generateStencil.value_or(false) ? GL::UNSIGNED_INT_24_8 : GL::UNSIGNED_INT;
  const auto internalFormat
    = internalOptions.generateStencil.value_or(false) ? GL::DEPTH_STENCIL : GL::DEPTH_COMPONENT;
  auto sizedFormat = internalFormat;
  if (_this->webGLVersion() > 1.f) {
    sizedFormat = internalOptions.generateStencil.value_or(false) ? GL::DEPTH24_STENCIL8 :
                                                                    GL::DEPTH_COMPONENT24;
  }

  if (internalTexture->is2DArray) {
    gl.texImage3D(target, 0, static_cast<int>(sizedFormat), internalTexture->width,
                  internalTexture->height, layers, 0, internalFormat, type, {});
  }
  else {
    gl.texImage2D(target, 0, static_cast<int>(sizedFormat), internalTexture->width,
                  internalTexture->height, 0, internalFormat, type, {});
  }

  _this->_bindTextureDirectly(target, nullptr);

  return internalTexture;
}

} // end of namespace BABYLON
