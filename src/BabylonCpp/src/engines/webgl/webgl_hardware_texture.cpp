#include <babylon/engines/webgl/webgl_hardware_texture.h>

#include <stdexcept>

#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

WebGLHardwareTexture::WebGLHardwareTexture(WebGLTexturePtr existingTexture,
                                           WebGLRenderingContext* context)
    : _MSAARenderBuffer{nullptr}
{
  _context = context;
  if (!existingTexture) {
    existingTexture = context->createTexture();
    if (!existingTexture) {
      throw std::runtime_error("Unable to create webGL texture");
    }
  }
  set(existingTexture);
}

WebGLHardwareTexture::~WebGLHardwareTexture() = default;

WebGLTexturePtr& WebGLHardwareTexture::underlyingResource()
{
  return _webGLTexture;
}

void WebGLHardwareTexture::setUsage(const InternalTextureSource& /*textureSource*/,
                                    bool /*generateMipMaps*/, bool /*isCube*/, int /*width*/,
                                    int /*height*/)
{
}

void WebGLHardwareTexture::set(const WebGLTexturePtr& hardwareTexture)
{
  _webGLTexture = hardwareTexture;
}

void WebGLHardwareTexture::reset()
{
  _webGLTexture     = nullptr;
  _MSAARenderBuffer = nullptr;
}

void WebGLHardwareTexture::release()
{
  if (_MSAARenderBuffer) {
    _context->deleteRenderbuffer(_MSAARenderBuffer.get());
    _MSAARenderBuffer = nullptr;
  }

  if (_webGLTexture && _context) {
    _context->deleteTexture(_webGLTexture.get());
  }
  reset();
}

} // end of namespace BABYLON
