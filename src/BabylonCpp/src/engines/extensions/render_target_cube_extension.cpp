#include <babylon/engines/extensions/render_target_cube_extension.h>

#include <babylon/core/logging.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/render_target_creation_options.h>
#include <babylon/maths/isize.h>

namespace BABYLON {

RenderTargetCubeExtension::RenderTargetCubeExtension(ThinEngine* engine) : _this{engine}
{
}

RenderTargetCubeExtension::~RenderTargetCubeExtension() = default;

InternalTexturePtr
RenderTargetCubeExtension::createRenderTargetCubeTexture(const ISize& size,
                                                         const IRenderTargetOptions& options)
{
  RenderTargetCreationOptions fullOptions;
  fullOptions.generateMipMaps       = options.generateMipMaps.value_or(true);
  fullOptions.generateDepthBuffer   = options.generateDepthBuffer.value_or(true);
  fullOptions.generateStencilBuffer = options.generateStencilBuffer.value_or(false);
  fullOptions.type                  = options.type.value_or(Constants::TEXTURETYPE_UNSIGNED_INT);
  fullOptions.samplingMode
    = options.samplingMode.value_or(Constants::TEXTURE_TRILINEAR_SAMPLINGMODE);
  fullOptions.format = options.format.value_or(Constants::TEXTUREFORMAT_RGBA);
  fullOptions.generateStencilBuffer
    = fullOptions.generateDepthBuffer.value() && fullOptions.generateStencilBuffer.value();

  if (fullOptions.type.value() == Constants::TEXTURETYPE_FLOAT
      && !_this->_caps.textureFloatLinearFiltering) {
    // if floating point linear (GL::FLOAT) then force to NEAREST_SAMPLINGMODE
    fullOptions.samplingMode = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
  }
  else if (fullOptions.type.value() == Constants::TEXTURETYPE_HALF_FLOAT
           && !_this->_caps.textureHalfFloatLinearFiltering) {
    // if floating point linear (HALF_FLOAT) then force to NEAREST_SAMPLINGMODE
    fullOptions.samplingMode = Constants::TEXTURE_NEAREST_SAMPLINGMODE;
  }
  auto& gl = *_this->_gl;

  auto texture = InternalTexture::New(_this, InternalTextureSource::RenderTarget);
  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture, true);

  auto filters = _this->_getSamplingParameters(fullOptions.samplingMode.value(),
                                               fullOptions.generateMipMaps.value());

  if (fullOptions.type.value() == Constants::TEXTURETYPE_FLOAT && !_this->_caps.textureFloat) {
    fullOptions.type = Constants::TEXTURETYPE_UNSIGNED_INT;
    BABYLON_LOG_WARN("RenderTargetCubeExtension",
                     "Float textures are not supported. Cube render target forced to "
                     "TEXTURETYPE_UNESIGNED_BYTE type")
  }

  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER, filters.mag);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER, filters.min);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
  gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

  for (unsigned int face = 0; face < 6; ++face) {
    gl.texImage2D((GL::TEXTURE_CUBE_MAP_POSITIVE_X + face), 0,
                  static_cast<GL::GLint>(_this->_getRGBABufferInternalSizedFormat(
                    fullOptions.type.value(), fullOptions.format.value())),
                  size.width, size.height, 0, _this->_getInternalFormat(fullOptions.format.value()),
                  _this->_getWebGLTextureType(fullOptions.type.value()), nullptr);
  }

  // Create the framebuffer
  auto framebuffer = gl.createFramebuffer();
  _this->_bindUnboundFramebuffer(framebuffer);

  texture->_depthStencilBuffer = _this->_setupFramebufferDepthAttachments(
    fullOptions.generateStencilBuffer.value(), fullOptions.generateDepthBuffer.value(), size.width,
    size.height);

  // Mipmaps
  if (texture->generateMipMaps) {
    gl.generateMipmap(GL::TEXTURE_CUBE_MAP);
  }

  // Unbind
  _this->_bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
  _this->_bindUnboundFramebuffer(nullptr);

  texture->_framebuffer           = std::move(framebuffer);
  texture->width                  = size.width;
  texture->height                 = size.height;
  texture->isReady                = true;
  texture->isCube                 = true;
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

} // end of namespace BABYLON
