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

InternalTexturePtr
RenderTargetExtension::createRenderTargetTexture(const std::variant<ISize, float>& size,
                                                 const IRenderTargetOptions& options)
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
  auto& gl = *_this->_gl;

  auto texture = InternalTexture::New(_this, InternalTextureSource::RenderTarget);
  _this->_bindTextureDirectly(GL::TEXTURE_2D, texture, true);

  int width = 0, height = 0;
  if (std::holds_alternative<ISize>(size)) {
    auto textureSize = std::get<ISize>(size);
    width            = textureSize.width;
    height           = textureSize.height;
  }
  else if (std::holds_alternative<float>(size)) {
    auto textureSize = std::get<float>(size);
    width            = static_cast<int>(textureSize);
    height           = static_cast<int>(textureSize);
  }

  auto filters = _this->_getSamplingParameters(fullOptions.samplingMode.value(),
                                               fullOptions.generateMipMaps.value());

  if (fullOptions.type.value() == Constants::TEXTURETYPE_FLOAT && !_this->_caps.textureFloat) {
    fullOptions.type = Constants::TEXTURETYPE_UNSIGNED_INT;
    BABYLON_LOG_WARN(
      "RenderTargetExtension",
      "Float textures are not supported. Render target forced to TEXTURETYPE_UNSIGNED_BYTE type")
  }

  gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
  gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);
  gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
  gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

  gl.texImage2D(GL::TEXTURE_2D, 0,
                static_cast<GL::GLint>(_this->_getRGBABufferInternalSizedFormat(
                  fullOptions.type.value(), fullOptions.format.value())),
                width, height, 0, _this->_getInternalFormat(fullOptions.format.value()),
                _this->_getWebGLTextureType(fullOptions.type.value()), nullptr);

  // Create the framebuffer
  auto currentFrameBuffer = _this->_currentFramebuffer;
  auto framebuffer        = gl.createFramebuffer();
  _this->_bindUnboundFramebuffer(framebuffer);
  gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0, GL::TEXTURE_2D,
                          texture->_webGLTexture.get(), 0);

  texture->_depthStencilBuffer = _this->_setupFramebufferDepthAttachments(
    fullOptions.generateStencilBuffer.value(), fullOptions.generateDepthBuffer.value(), width,
    height);

  if (fullOptions.generateMipMaps.value()) {
    gl.generateMipmap(GL::TEXTURE_2D);
  }

  // Unbind
  _this->_bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  gl.bindRenderbuffer(GL::RENDERBUFFER, nullptr);
  _this->_bindUnboundFramebuffer(currentFrameBuffer);

  texture->_framebuffer           = std::move(framebuffer);
  texture->baseWidth              = width;
  texture->baseHeight             = height;
  texture->width                  = width;
  texture->height                 = height;
  texture->isReady                = true;
  texture->samples                = 1;
  texture->generateMipMaps        = fullOptions.generateMipMaps.value();
  texture->samplingMode           = fullOptions.samplingMode.value();
  texture->type                   = fullOptions.type.value();
  texture->format                 = fullOptions.format.value();
  texture->_generateDepthBuffer   = fullOptions.generateDepthBuffer.value();
  texture->_generateStencilBuffer = fullOptions.generateStencilBuffer.value();

  // resetTextureCache();

  _this->_internalTexturesCache.emplace_back(texture);

  return texture;
}

InternalTexturePtr
RenderTargetExtension::createDepthStencilTexture(const std::variant<int, ISize>& size,
                                                 const DepthTextureCreationOptions& options)
{
  if (options.isCube.value_or(false)) {
    auto width
      = std::holds_alternative<int>(size) ? std::get<int>(size) : std::get<ISize>(size).width;
    return _this->_createDepthStencilCubeTexture(width, options);
  }
  else {
    return _this->_createDepthStencilTexture(size, options);
  }
}

InternalTexturePtr
RenderTargetExtension::_createDepthStencilTexture(const std::variant<int, ISize>& size,
                                                  const DepthTextureCreationOptions& options)
{
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

  auto& gl = *_this->_gl;
  _this->_bindTextureDirectly(GL::TEXTURE_2D, internalTexture, true);

  _this->_setupDepthStencilTexture(internalTexture, size, *internalOptions.generateStencil,
                                   *internalOptions.bilinearFiltering,
                                   *internalOptions.comparisonFunction);

  if (_this->webGLVersion() > 1.f) {
    if (internalOptions.generateStencil.value_or(false)) {
      gl.texImage2D(GL::TEXTURE_2D, 0, GL::DEPTH24_STENCIL8, internalTexture->width,
                    internalTexture->height, 0, GL::DEPTH_STENCIL, GL::UNSIGNED_INT_24_8, nullptr);
    }
    else {
      gl.texImage2D(GL::TEXTURE_2D, 0, GL::DEPTH_COMPONENT24, internalTexture->width,
                    internalTexture->height, 0, GL::DEPTH_COMPONENT, GL::UNSIGNED_INT, nullptr);
    }
  }
  else {
    if (internalOptions.generateStencil.value_or(false)) {
      gl.texImage2D(GL::TEXTURE_2D, 0, GL::DEPTH_STENCIL, internalTexture->width,
                    internalTexture->height, 0, GL::DEPTH_STENCIL, GL::UNSIGNED_INT_24_8, nullptr);
    }
    else {
      gl.texImage2D(GL::TEXTURE_2D, 0, GL::DEPTH_COMPONENT, internalTexture->width,
                    internalTexture->height, 0, GL::DEPTH_COMPONENT, GL::UNSIGNED_INT, nullptr);
    }
  }

  _this->_bindTextureDirectly(GL::TEXTURE_2D, nullptr);

  return internalTexture;
}

} // end of namespace BABYLON
