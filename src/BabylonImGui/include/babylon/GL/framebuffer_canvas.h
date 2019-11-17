#ifndef BABYLON_IMPL_CANVAS_H
#define BABYLON_IMPL_CANVAS_H

#include <babylon/GL/gl_rendering_context.h>
#include <babylon/babylon_api.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {
namespace GL {

class FramebufferCanvas : public ICanvas {

public:
  FramebufferCanvas();
  ~FramebufferCanvas() override;
  bool onlyRenderBoundingClientRect() const override;
  void initializeFrameBuffer();

  ClientRect& getBoundingClientRect() override;
  bool initializeContext3d() override;
  ICanvasRenderingContext2D* getContext2d() override;
  GL::IGLRenderingContext* getContext3d(const EngineOptions& options) override;

  void bind();
  void unbind();

  unsigned int textureId();
  Uint8Array readPixelsRgb();
  void saveScreenshotJpg(const char* filename, int quality, int imageWidth = -1);
  void saveScreenshotPng(const char* filename);

private:
  std::shared_ptr<BABYLON::GL::IGLFramebuffer> mFrameBuffer;
  std::unique_ptr<BABYLON::GL::IGLTexture> mTextureColorBuffer;
};

} // end of namespace GL
} // end of namespace BABYLON

#endif // BABYLON_IMPL_CANVAS_H
