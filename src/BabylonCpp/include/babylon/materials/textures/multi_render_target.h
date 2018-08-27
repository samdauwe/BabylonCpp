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

  /** Hidden */
  void _rebuild() override;

  void resize(Size size);
  void dispose() override;
  void releaseInternalTextures();

protected:
  bool get_isSupported() const;
  vector_t<TexturePtr>& get_textures();
  TexturePtr& get_depthTexture();
  void set_wrapU(unsigned int wrap);
  void set_wrapV(unsigned int wrap);
  unsigned int get_samples() const override;
  void set_samples(unsigned int value) override;

  void unbindFrameBuffer(Engine* engine, unsigned int faceIndex);

private:
  void _createInternalTextures();
  void _createTextures();

public:
  ReadOnlyProperty<MultiRenderTarget, bool> isSupported;
  ReadOnlyProperty<MultiRenderTarget, vector_t<TexturePtr>> textures;
  ReadOnlyProperty<MultiRenderTarget, TexturePtr> depthTexture;
  WriteOnlyProperty<MultiRenderTarget, unsigned int> wrapU;
  WriteOnlyProperty<MultiRenderTarget, unsigned int> wrapV;

private:
  vector_t<InternalTexture*> _internalTextures;
  vector_t<TexturePtr> _textures;
  IMultiRenderTargetOptions _multiRenderTargetOptions;
  TexturePtr _nullTexture;

}; // end of class MultiRenderTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MULTI_RENDER_TARGET_H
