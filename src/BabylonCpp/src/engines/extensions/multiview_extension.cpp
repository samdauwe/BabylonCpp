#include <babylon/engines/extensions/multiview_extension.h>

#include <babylon/engines/engine.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

MultiviewExtension::MultiviewExtension(Engine* engine) : _this{engine}
{
}

MultiviewExtension::~MultiviewExtension() = default;

InternalTexturePtr MultiviewExtension::createMultiviewRenderTargetTexture(int width, int height)
{
  auto& gl = *_this->_gl;

  if (!_this->getCaps().multiview) {
    throw std::runtime_error("Multiview is not supported");
  }

  auto internalTexture          = InternalTexture::New(_this, InternalTextureSource::Unknown, true);
  internalTexture->width        = width;
  internalTexture->height       = height;
  internalTexture->_framebuffer = gl.createFramebuffer();

  internalTexture->_colorTextureArray = gl.createTexture();
  gl.bindTexture(GL::TEXTURE_2D_ARRAY, internalTexture->_colorTextureArray.get());
  gl.texStorage3D(GL::TEXTURE_2D_ARRAY, 1, GL::RGBA8, width, height, 2);

  internalTexture->_depthStencilTextureArray = gl.createTexture();
  gl.bindTexture(GL::TEXTURE_2D_ARRAY, internalTexture->_depthStencilTextureArray.get());
  gl.texStorage3D(GL::TEXTURE_2D_ARRAY, 1, GL::DEPTH32F_STENCIL8, width, height, 2);
  internalTexture->isReady = true;
  return internalTexture;
}

void MultiviewExtension::bindMultiviewFramebuffer(const InternalTexturePtr& multiviewTexture)
{
  auto& gl = *_this->_gl;

  _this->bindFramebuffer(multiviewTexture, 0, std::nullopt, std::nullopt, true);
  gl.bindFramebuffer(GL::DRAW_FRAMEBUFFER, multiviewTexture->_framebuffer.get());
  if (multiviewTexture->_colorTextureArray && multiviewTexture->_depthStencilTextureArray) {
    gl.framebufferTextureMultiviewOVR(GL::DRAW_FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                                      multiviewTexture->_colorTextureArray.get(), 0, 0, 2);
    gl.framebufferTextureMultiviewOVR(GL::DRAW_FRAMEBUFFER, GL::DEPTH_STENCIL_ATTACHMENT,
                                      multiviewTexture->_depthStencilTextureArray.get(), 0, 0, 2);
  }
  else {
    throw std::runtime_error("Invalid multiview frame buffer");
  }
}

} // end of namespace BABYLON
