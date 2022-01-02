#include <babylon/engines/extensions/read_texture_extension.h>

#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

ReadTextureExtension::ReadTextureExtension(ThinEngine* engine) : _this{engine}
{
}

ArrayBufferView ReadTextureExtension::_readTexturePixels(const InternalTexturePtr& texture,
                                                         int width, int height, int faceIndex,
                                                         int level,
                                                         std::optional<ArrayBufferView> buffer)
{
  auto& gl = *_this->_gl;
  if (!_this->_dummyFramebuffer) {
    auto dummy = gl.createFramebuffer();

    if (!dummy) {
      throw std::runtime_error("Unable to create dummy framebuffer");
    }

    _this->_dummyFramebuffer = dummy;
  }
  gl.bindFramebuffer(GL::FRAMEBUFFER, _this->_dummyFramebuffer.get());

  if (faceIndex > -1) {
    auto _faceIndex = static_cast<unsigned>(faceIndex);
    gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                            GL::TEXTURE_CUBE_MAP_POSITIVE_X + _faceIndex,
                            texture->_webGLTexture.get(), level);
  }
  else {
    gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0, GL::TEXTURE_2D,
                            texture->_webGLTexture.get(), level);
  }

  auto readType = _this->_getWebGLTextureType(texture->type);

  switch (readType) {
    case GL::UNSIGNED_BYTE: {
      if (!buffer.has_value()) {
        buffer = ArrayBufferView(Uint8Array(static_cast<std::size_t>(4 * width * height)));
      }
      readType = GL::UNSIGNED_BYTE;
      gl.readPixels(0, 0, width, height, GL::RGBA, readType, buffer->uint8Array());
      gl.bindFramebuffer(GL::FRAMEBUFFER, _this->_currentFramebuffer.get());
    } break;
    default: {
      if (!buffer.has_value()) {
        buffer = ArrayBufferView(Float32Array(static_cast<std::size_t>(4 * width * height)));
      }
      readType                = GL::FLOAT;
      auto bufferFloat32Array = buffer->float32Array();
      gl.readPixels(0, 0, width, height, GL::RGBA, readType, bufferFloat32Array);
      gl.bindFramebuffer(GL::FRAMEBUFFER, _this->_currentFramebuffer.get());
      buffer = ArrayBufferView(bufferFloat32Array);
    } break;
  }

  return *buffer;
}

} // end of namespace BABYLON
