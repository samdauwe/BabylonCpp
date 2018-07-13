#ifndef BABYLON_SAMPLES_SPECIAL_FX_INFINITE_LOADER_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_INFINITE_LOADER_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

struct Range {
  float min;
  float max;
}; // end of struct Range

struct Ring {
  Ring();
  Ring(const Ring& other);
  ~Ring();

  void addTube(const std::vector<Vector3>& points);

  Mesh* center;
  std::vector<Mesh*> tubes;
  float angle;
}; // end of struct Ring

/**
 * @brief Hypnotizing infinite loader scene. Example demonstrating how to create
 * visually apealing loading animation.
 * @see https://www.babylonjs-playground.com/#VUJG1#1
 */
class InfiniteLoaderScene : public IRenderableScene {

public:
  InfiniteLoaderScene(ICanvas* iCanvas);
  ~InfiniteLoaderScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  std::vector<Ring> _rings;

}; // end of class InfiniteLoaderScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_SPECIAL_FX_INFINITE_LOADER_SCENE_H
