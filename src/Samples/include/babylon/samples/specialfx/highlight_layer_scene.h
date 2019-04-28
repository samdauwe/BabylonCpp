#ifndef BABYLON_SAMPLES_SPECIAL_FX_HIGHLIGHT_LAYER_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_HIGHLIGHT_LAYER_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class HighlightLayer;
using HighlightLayerPtr = std::shared_ptr<HighlightLayer>;

namespace Samples {

/**
 * @brief Highlight layer scene. Example demonstrating how to highlight a mesh.
 * @see https://www.babylonjs-playground.com/#7EESGZ#0
 * @see https://doc.babylonjs.com/how_to/highlight_layer
 */
class HighlightLayerScene : public IRenderableScene {

public:
  HighlightLayerScene(ICanvas* iCanvas);
  ~HighlightLayerScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _alpha;
  HighlightLayerPtr _hl;

}; // end of class HighlightLayerScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_HIGHLIGHT_LAYER_SCENE_H
