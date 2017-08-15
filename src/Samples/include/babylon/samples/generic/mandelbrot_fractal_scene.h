#ifndef BABYLON_SAMPLES_GENERIC_MANDELBROT_FRACTAL_SCENE_H
#define BABYLON_SAMPLES_GENERIC_MANDELBROT_FRACTAL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class MandelbrotFractalScene : public IRenderableScene {

public:
  MandelbrotFractalScene(ICanvas* iCanvas);
  ~MandelbrotFractalScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCamera* _camera;
  PointLight* _pl;

}; // end of class MandelbrotFractalScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_GENERIC_MANDELBROT_FRACTAL_SCENE_H
