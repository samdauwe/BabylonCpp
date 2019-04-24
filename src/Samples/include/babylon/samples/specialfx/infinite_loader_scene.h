#ifndef BABYLON_SAMPLES_SPECIAL_FX_INFINITE_LOADER_SCENE_H
#define BABYLON_SAMPLES_SPECIAL_FX_INFINITE_LOADER_SCENE_H

#include <vector>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
class Vector3;
using MeshPtr = std::shared_ptr<Mesh>;

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

  MeshPtr center;
  std::vector<MeshPtr> tubes;
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
