#include <babylon/engines/extensions/multi_render_extension.h>

#include <babylon/core/logging.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/imulti_render_target_options.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/maths/isize.h>

namespace BABYLON {

MultiRenderExtension::MultiRenderExtension(ThinEngine* engine) : _this{engine}
{
}

MultiRenderExtension::~MultiRenderExtension() = default;

void MultiRenderExtension::unBindMultiColorAttachmentFramebuffer(
  const std::vector<InternalTexturePtr>& textures, bool disableGenerateMipMaps,
  const std::function<void()>& onBeforeUnbind)
{
  _this->_currentRenderTarget = nullptr;

  // If MSAA, we need to bitblt back to main texture
  auto& gl = *_this->_gl;

  auto& attachments = textures[0]->_attachments;
  auto count        = attachments.size();

  if (textures[0]->_MSAAFramebuffer) {
    gl.bindFramebuffer(GL::READ_FRAMEBUFFER, textures[0]->_MSAAFramebuffer.get());
    gl.bindFramebuffer(GL::DRAW_FRAMEBUFFER, textures[0]->_framebuffer.get());

    for (size_t i = 0; i < count; ++i) {
      const auto iStr = std::to_string(i);
      auto& texture   = textures[i];

      for (size_t j = 0; j < count; ++j) {
        attachments[j] = GL::NONE;
      }

      attachments[i] = gl[_this->webGLVersion() > 1.f ? "COLOR_ATTACHMENT" + iStr :
                                                        "COLOR_ATTACHMENT" + iStr + "_WEBGL"];
      gl.readBuffer(attachments[i]);
      gl.drawBuffers(attachments);
      gl.blitFramebuffer(0, 0, texture->width, texture->height, 0, 0, texture->width,
                         texture->height, GL::COLOR_BUFFER_BIT, GL::NEAREST);
    }
    for (size_t i = 0; i < count; i++) {
      const auto iStr = std::to_string(i);
      attachments[i]  = gl[_this->webGLVersion() > 1.f ? "COLOR_ATTACHMENT" + iStr :
                                                         "COLOR_ATTACHMENT" + iStr + "_WEBGL"];
    }
    gl.drawBuffers(attachments);
  }

  for (size_t i = 0; i < count; ++i) {
    const auto& texture = textures[i];
    if (texture->generateMipMaps && !disableGenerateMipMaps && !texture->isCube) {
      _this->_bindTextureDirectly(GL::TEXTURE_2D, texture);
      gl.generateMipmap(GL::TEXTURE_2D);
      _this->_bindTextureDirectly(GL::TEXTURE_2D, nullptr);
    }
  }

  if (onBeforeUnbind) {
    if (textures[0]->_MSAAFramebuffer) {
      // Bind the correct framebuffer
      _this->_bindUnboundFramebuffer(textures[0]->_framebuffer);
    }
    onBeforeUnbind();
  }

  _this->_bindUnboundFramebuffer(nullptr);
}

std::vector<InternalTexturePtr> MultiRenderExtension::createMultipleRenderTarget(
  ISize size, const IMultiRenderTargetOptions& options, bool initializeBuffers)
{
  auto generateMipMaps       = options.generateMipMaps.value_or(false);
  auto generateDepthBuffer   = options.generateDepthBuffer.value_or(true);
  auto generateStencilBuffer = options.generateStencilBuffer.value_or(false);
  auto generateDepthTexture  = options.generateDepthTexture.value_or(false);
  auto textureCount          = options.textureCount.value_or(1);

  auto defaultType         = Constants::TEXTURETYPE_UNSIGNED_INT;
  auto defaultSamplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE;

  const auto& types         = options.types;
  const auto& samplingModes = options.samplingModes;

  auto width  = size.width;
  auto height = size.height;

  auto& gl = *_this->_gl;
  // Create the framebuffer
  auto framebuffer = gl.createFramebuffer();
  _this->_bindUnboundFramebuffer(framebuffer);

  std::vector<InternalTexturePtr> textures;
  std::vector<GL::GLenum> attachments;

  auto depthStencilBuffer = _this->_setupFramebufferDepthAttachments(
    generateStencilBuffer, generateDepthBuffer, width, height);

  for (unsigned int i = 0; i < textureCount; ++i) {
    const auto iStr   = std::to_string(i);
    auto samplingMode = (i < samplingModes.size()) ? samplingModes[i] : defaultSamplingMode;
    auto type         = (i < types.size()) ? types[i] : defaultType;

    if (type == Constants::TEXTURETYPE_FLOAT && !_this->_caps.textureFloatLinearFiltering) {
      // if floating point linear (GL::FLOAT) then force to NEAREST_SAMPLINGMODE
      samplingMode = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
    }
    else if (type == Constants::TEXTURETYPE_HALF_FLOAT
             && !_this->_caps.textureHalfFloatLinearFiltering) {
      // if floating point linear (HALF_FLOAT) then force to NEAREST_SAMPLINGMODE
      samplingMode = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
    }

    auto filters = _this->_getSamplingParameters(samplingMode, generateMipMaps);
    if (type == Constants::TEXTURETYPE_FLOAT && !_this->_caps.textureFloat) {
      type = Constants::TEXTURETYPE_UNSIGNED_INT;
      BABYLON_LOG_WARN(
        "MultiRenderExtension",
        "Float textures are not supported. Render target forced to TEXTURETYPE_UNSIGNED_BYTE type")
    }

    auto texture    = InternalTexture::New(_this, InternalTextureSource::MultiRenderTarget);
    auto attachment = gl[_this->webGLVersion() > 1.f ? "COLOR_ATTACHMENT" + iStr :
                                                       "COLOR_ATTACHMENT" + iStr + "_WEBGL"];

    textures.emplace_back(texture);
    attachments.emplace_back(attachment);

    gl.activeTexture(gl["TEXTURE" + iStr]);
    gl.bindTexture(GL::TEXTURE_2D, texture->_hardwareTexture ?
                                     texture->_hardwareTexture->underlyingResource().get() :
                                     nullptr);

    gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
    gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);
    gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
    gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

    gl.texImage2D(GL::TEXTURE_2D, 0,
                  static_cast<int>(_this->_getRGBABufferInternalSizedFormat(type)), width, height,
                  0, GL::RGBA, _this->_getWebGLTextureType(type), nullptr);

    gl.framebufferTexture2D(
      GL::DRAW_FRAMEBUFFER, attachment, GL::TEXTURE_2D,
      texture->_hardwareTexture ? texture->_hardwareTexture->underlyingResource().get() : nullptr,
      0);

    if (generateMipMaps) {
      gl.generateMipmap(GL::TEXTURE_2D);
    }

    // Unbind
    _this->_bindTextureDirectly(GL::TEXTURE_2D, nullptr);

    texture->_framebuffer           = std::move(framebuffer);
    texture->_depthStencilBuffer    = std::move(depthStencilBuffer);
    texture->baseWidth              = width;
    texture->baseHeight             = height;
    texture->width                  = width;
    texture->height                 = height;
    texture->isReady                = true;
    texture->samples                = 1;
    texture->generateMipMaps        = generateMipMaps;
    texture->samplingMode           = samplingMode;
    texture->type                   = type;
    texture->_generateDepthBuffer   = generateDepthBuffer;
    texture->_generateStencilBuffer = generateStencilBuffer;
    texture->_attachments           = attachments;
    texture->_textureArray          = textures;

    _this->_internalTexturesCache.emplace_back(texture);
  }

  if (generateDepthTexture && _this->_caps.depthTextureExtension) {
    // Depth texture
    auto depthTexture = InternalTexture::New(_this, InternalTextureSource::MultiRenderTarget);
    GL::IGLTexture* depthTextureHardwareTexture
      = depthTexture->_hardwareTexture ?
          depthTexture->_hardwareTexture->underlyingResource().get() :
          nullptr;

    gl.activeTexture(GL::TEXTURE0);
    gl.bindTexture(GL::TEXTURE_2D, depthTextureHardwareTexture);
    gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, GL::NEAREST);
    gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, GL::NEAREST);
    gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
    gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);
    gl.texImage2D(GL::TEXTURE_2D,                                                            //
                  0,                                                                         //
                  _this->webGLVersion() < 2.f ? GL::DEPTH_COMPONENT : GL::DEPTH_COMPONENT16, //
                  width,                                                                     //
                  height,                                                                    //
                  0,                                                                         //
                  GL::DEPTH_COMPONENT,                                                       //
                  GL::UNSIGNED_SHORT,                                                        //
                  nullptr                                                                    //
    );

    gl.framebufferTexture2D(GL::FRAMEBUFFER,             //
                            GL::DEPTH_ATTACHMENT,        //
                            GL::TEXTURE_2D,              //
                            depthTextureHardwareTexture, //
                            0                            //
    );

    depthTexture->_framebuffer           = std::move(framebuffer);
    depthTexture->baseWidth              = width;
    depthTexture->baseHeight             = height;
    depthTexture->width                  = width;
    depthTexture->height                 = height;
    depthTexture->isReady                = true;
    depthTexture->samples                = 1;
    depthTexture->generateMipMaps        = generateMipMaps;
    depthTexture->samplingMode           = GL::NEAREST;
    depthTexture->_generateDepthBuffer   = generateDepthBuffer;
    depthTexture->_generateStencilBuffer = generateStencilBuffer;

    textures.emplace_back(depthTexture);
    _this->_internalTexturesCache.emplace_back(depthTexture);
  }

  if (initializeBuffers) {
    gl.drawBuffers(attachments);
  }

  _this->_bindUnboundFramebuffer(nullptr);

  _this->resetTextureCache();

  return textures;
}

unsigned int MultiRenderExtension::updateMultipleRenderTargetTextureSampleCount(
  const std::vector<InternalTexturePtr>& textures, unsigned int samples, bool initializeBuffers)
{
  if (_this->webGLVersion() < 2.f || textures.empty()) {
    return 1;
  }

  if (textures[0]->samples == samples) {
    return samples;
  }

  auto count = textures[0]->_attachments.size();

  if (count == 0) {
    return 1;
  }

  auto& gl = *_this->_gl;

  samples = std::min(samples, _this->getCaps().maxMSAASamples);

  // Dispose previous render buffers
  if (textures[0]->_depthStencilBuffer) {
    gl.deleteRenderbuffer(textures[0]->_depthStencilBuffer.get());
    textures[0]->_depthStencilBuffer = nullptr;
  }

  if (textures[0]->_MSAAFramebuffer) {
    gl.deleteFramebuffer(textures[0]->_MSAAFramebuffer.get());
    textures[0]->_MSAAFramebuffer = nullptr;
  }

  for (size_t i = 0; i < count; i++) {
    const auto& texture = textures[i];
    if (texture->_MSAARenderBuffer) {
      gl.deleteRenderbuffer(texture->_MSAARenderBuffer.get());
      texture->_MSAARenderBuffer = nullptr;
    }
  }

  if (samples > 1) {
    auto framebuffer = gl.createFramebuffer();

    if (!framebuffer) {
      throw std::runtime_error("Unable to create multi sampled framebuffer");
    }

    _this->_bindUnboundFramebuffer(framebuffer);

    auto depthStencilBuffer = _this->_setupFramebufferDepthAttachments(
      textures[0]->_generateStencilBuffer, textures[0]->_generateDepthBuffer, textures[0]->width,
      textures[0]->height, static_cast<int>(samples));

    Uint32Array attachments;

    for (size_t i = 0; i < count; ++i) {
      auto iStr       = std::to_string(i);
      auto& texture   = textures[i];
      auto attachment = gl[_this->webGLVersion() > 1.f ? "COLOR_ATTACHMENT" + iStr :
                                                         "COLOR_ATTACHMENT" + iStr + "_WEBGL"];

      auto colorRenderbuffer = gl.createRenderbuffer();

      if (!colorRenderbuffer) {
        throw std::runtime_error("Unable to create multi sampled framebuffer");
      }

      gl.bindRenderbuffer(GL::RENDERBUFFER, colorRenderbuffer.get());
      gl.renderbufferStorageMultisample(GL::RENDERBUFFER, static_cast<int>(samples),
                                        _this->_getRGBAMultiSampleBufferFormat(texture->type),
                                        texture->width, texture->height);

      gl.framebufferRenderbuffer(GL::FRAMEBUFFER, attachment, GL::RENDERBUFFER,
                                 colorRenderbuffer.get());

      texture->_MSAAFramebuffer    = framebuffer;
      texture->_MSAARenderBuffer   = std::move(colorRenderbuffer);
      texture->samples             = samples;
      texture->_depthStencilBuffer = std::move(depthStencilBuffer);
      gl.bindRenderbuffer(GL::RENDERBUFFER, nullptr);
      attachments.emplace_back(attachment);
    }
    if (initializeBuffers) {
      gl.drawBuffers(attachments);
    }
  }
  else {
    _this->_bindUnboundFramebuffer(textures[0]->_framebuffer);
  }

  _this->_bindUnboundFramebuffer(nullptr);

  return samples;
}

void MultiRenderExtension::bindAttachments(const std::vector<unsigned int>& attachments)
{
  auto& gl = *_this->_gl;

  gl.drawBuffers(attachments);
}

std::vector<unsigned int>
MultiRenderExtension::buildTextureLayout(const std::vector<bool>& textureStatus) const
{
  auto& gl = *_this->_gl;

  std::vector<unsigned int> result;

  for (auto i = 0ull; i < textureStatus.size(); ++i) {
    if (textureStatus[i]) {
      result.emplace_back(gl["COLOR_ATTACHMENT" + std::to_string(i)]);
    }
    else {
      result.emplace_back(GL::NONE);
    }
  }

  return result;
}

void MultiRenderExtension::restoreSingleAttachment()
{
  bindAttachments({GL::BACK});
}

void MultiRenderExtension::restoreSingleAttachmentForRenderTarget()
{
  bindAttachments({GL::COLOR_ATTACHMENT0});
}

void MultiRenderExtension::clearAttachments(Uint32Array& attachments,
                                            const std::optional<Color4>& colorMain,
                                            const std::optional<Color4>& colorOthers,
                                            bool clearDepth, bool clearStencil)
{
  if (attachments.empty()) {
    return;
  }

  auto& gl = *_this->_gl;

  gl.drawBuffers({attachments[0]});
  _this->clear(colorMain, colorMain.has_value(), clearDepth, clearStencil);

  const auto saveVal = attachments[0];
  attachments[0]     = GL::NONE;

  gl.drawBuffers(attachments);
  _this->clear(colorOthers, colorOthers.has_value(), false, false);

  attachments[0] = saveVal;
}

} // end of namespace BABYLON
