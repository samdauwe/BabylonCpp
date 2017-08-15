#ifndef BABYLON_IMPL_CANVAS_H
#define BABYLON_IMPL_CANVAS_H

#include <babylon/interfaces/icanvas.h>

namespace BABYLON {
namespace impl {

class Canvas : public ICanvas {

public:
  Canvas();
  virtual ~Canvas();

  ClientRect& getBoundingClientRect() override;
  bool onlyRenderBoundingClientRect() const override;
  bool initializeContext3d() override;
  ICanvasRenderingContext2D* getContext2d() override;
  GL::IGLRenderingContext* getContext3d(const EngineOptions& options) override;
};

} // end of namespace impl
} // end of namespace BABYLON

#endif // BABYLON_IMPL_CANVAS_H
