#ifndef BABYLON_H
#define BABYLON_H

#include <babylon/interfaces/icanvas.h>

namespace BABYLON {
namespace impl {

class Canvas : public ICanvas {

public:
  Canvas();
  virtual ~Canvas();

  ClientRect& getBoundingClientRect() override;
  bool initializeContext3d() override;
  ICanvasRenderingContext2D* getContext2d() override;
  GL::IGLRenderingContext* getContext3d(const EngineOptions& options) override;
};

} // end of namespace impl
} // end of namespace BABYLON

#endif // BABYLON_H
