#ifndef BABYLON_MATERIALS_TEXTURES_MULTI_RENDER_TARGET_H
#define BABYLON_MATERIALS_TEXTURES_MULTI_RENDER_TARGET_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/imulti_render_target_options.h>
#include <babylon/materials/textures/render_target_texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MultiRenderTarget : public RenderTargetTexture {

public:
  MultiRenderTarget(const string_t& name, Size size, std::size_t count,
                    Scene* scene,
                    const nullable_t<IMultiRenderTargetOptions>& options
                    = nullopt_t);
  ~MultiRenderTarget() override;

  bool isSupported() const;
  vector_t<TexturePtr>& textures();
  TexturePtr depthTexture();

  void setWrapU(unsigned int wrap);
  void setWrapV(unsigned int wrap);
  void _rebuild() override;
  unsigned int samples() const;
  void setSamples(unsigned int value);
  void resize(Size size);
  void dispose() override;
  void releaseInternalTextures();

protected:
  void unbindFrameBuffer(Engine* engine, unsigned int faceIndex);

private:
  void _createInternalTextures();
  void _createTextures();

private:
  vector_t<InternalTexture*> _internalTextures;
  vector_t<TexturePtr> _textures;
  IMultiRenderTargetOptions _multiRenderTargetOptions;

}; // end of class MultiRenderTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MULTI_RENDER_TARGET_H
