#ifndef BABYLON_MATERIALS_TEXTURES_MULTI_RENDER_TARGET_H
#define BABYLON_MATERIALS_TEXTURES_MULTI_RENDER_TARGET_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/imulti_render_target_options.h>
#include <babylon/materials/textures/render_target_texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MultiRenderTarget : public RenderTargetTexture {

public:
  MultiRenderTarget(const std::string& name, Size size, std::size_t count,
                    Scene* scene);
  MultiRenderTarget(const std::string& name, Size size, std::size_t count,
                    Scene* scene, const IMultiRenderTargetOptions& options);
  ~MultiRenderTarget();

  bool isSupported() const;
  std::vector<Texture*>& textures();
  Texture* depthTexture();

  unsigned int samples() const;
  void setSamples(unsigned int value);
  void resize(Size size);
  void dispose(bool doNotRecurse = false) override;
  void releaseInternalTextures();

private:
  void _createInternalTextures();

private:
  std::vector<GL::IGLTexture*> _webGLTextures;
  std::vector<Texture*> _textures;
  std::size_t _count;
  IMultiRenderTargetOptions _multiRenderTargetOptions;

}; // end of class MultiRenderTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MULTI_RENDER_TARGET_H
