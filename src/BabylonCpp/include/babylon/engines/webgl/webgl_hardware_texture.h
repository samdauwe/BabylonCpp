#ifndef BABYLON_ENGINES_WEBGL_WEBGL_HARDWARE_TEXTURE_H
#define BABYLON_ENGINES_WEBGL_WEBGL_HARDWARE_TEXTURE_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/hardware_texture_wrapper.h>

namespace BABYLON {

namespace GL {
class IGLRenderbuffer;
class IGLRenderingContext;
class IGLTexture;
} // end of namespace GL

// GL
using WebGLTexturePtr       = std::shared_ptr<GL::IGLTexture>;
using WebGLRenderbufferPtr  = std::shared_ptr<GL::IGLRenderbuffer>;
using WebGLRenderingContext = GL::IGLRenderingContext;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT WebGLHardwareTexture : public HardwareTextureWrapper<WebGLTexturePtr> {

public:
  WebGLHardwareTexture(WebGLTexturePtr existingTexture = nullptr,
                       WebGLRenderingContext* context  = nullptr);
  ~WebGLHardwareTexture(); // = default

  /**
   * @brief Gets the underlying Resource
   */
  WebGLTexturePtr& underlyingResource() override;

  void setUsage(const InternalTextureSource& textureSource, bool generateMipMaps, bool isCube,
                int width, int height) override;

  void set(const WebGLTexturePtr& hardwareTexture) override;

  void reset() override;

  void release() override;

public:
  WebGLRenderbufferPtr _MSAARenderBuffer;

private:
  WebGLTexturePtr _webGLTexture;
  WebGLRenderingContext* _context;

}; // end of struct WebGLHardwareTexture

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_WEBGL_WEBGL_HARDWARE_TEXTURE_H
