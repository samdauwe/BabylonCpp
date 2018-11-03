#ifndef BABYLON_SAMPLES_SPECIAL_FX_CONVOLUTION_POST_PROCESS_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_CONVOLUTION_POST_PROCESS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Convolution Post-process Scene. This example demonstrates how to apply
 * emboss filter to the scene using the ConvolutionPostProcess.
 * @see https://www.babylonjs-playground.com/#B0RH9H#0
 */
class ConvolutionPostProcessScene : public IRenderableScene {

public:
  ConvolutionPostProcessScene(ICanvas* iCanvas);
  ~ConvolutionPostProcessScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  MeshPtr _sphere0;
  MeshPtr _sphere1;
  MeshPtr _sphere2;
  MeshPtr _cube;
  float _alpha;

}; // end of class ConvolutionPostProcessScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_CONVOLUTION_POST_PROCESS_SCENE_H
