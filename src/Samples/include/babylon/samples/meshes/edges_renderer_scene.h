#ifndef BABYLON_SAMPLES_MESHES_EDGES_RENDERER_SCENE_H
#define BABYLON_SAMPLES_MESHES_EDGES_RENDERER_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Edges Renderer Scene. Example demonstrating how to enable edges
 * rendering on meshes.
 * @see https://www.babylonjs-playground.com/#TYAHX#10
 */
struct EdgesRenderScene : public IRenderableScene {

  EdgesRenderScene(ICanvas* iCanvas);
  ~EdgesRenderScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct EdgesRenderScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_EDGES_RENDERER_SCENE_H
