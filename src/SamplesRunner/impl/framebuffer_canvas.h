#ifndef BABYLON_IMPL_CANVAS_H
#define BABYLON_IMPL_CANVAS_H

#include <SamplesRunner/impl/gl_rendering_context.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {
namespace impl {

class FramebufferCanvas : public ICanvas 
{

public:
  FramebufferCanvas();
  virtual ~FramebufferCanvas();
  void initializeFrameBuffer();

  ClientRect& getBoundingClientRect() override;
  bool onlyRenderBoundingClientRect() const override;
  bool initializeContext3d() override;
  ICanvasRenderingContext2D* getContext2d() override;
  GL::IGLRenderingContext* getContext3d(const EngineOptions& options) override;

  void bind();
  void unbind();

  unsigned int TextureId();


private:
  std::unique_ptr<BABYLON::GL::IGLFramebuffer> mFrameBuffer;
  std::unique_ptr<BABYLON::GL::IGLTexture> mTextureColorBuffer;
};

} // end of namespace impl
} // end of namespace BABYLON

#endif // BABYLON_IMPL_CANVAS_H
