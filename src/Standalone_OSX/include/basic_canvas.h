#ifndef BABYLON_BASIC_CANVAS_H
#define BABYLON_BASIC_CANVAS_H

#include <babylon/interfaces/icanvas.h>

class BasicCanvas : public BABYLON::ICanvas {

public:
  BasicCanvas();
  virtual ~BasicCanvas();

  BABYLON::ClientRect& getBoundingClientRect() override;
  bool initializeContext3d() override;
  BABYLON::ICanvasRenderingContext2D* getContext2d() override;
  BABYLON::GL::IGLRenderingContext* getContext3d(const BABYLON::EngineOptions& options) override;
};

#endif // BABYLON_BASIC_CANVAS_H
