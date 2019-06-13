#ifndef BABYLON_SAMPLES_MESHES_CUBES_SCENE_H
#define BABYLON_SAMPLES_MESHES_CUBES_SCENE_H

#include <tuple>
#include <vector>

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Cubes Scene. Example demonstrating how to create animated cubes.
 * @see https://playground.babylonjs.com/#F7BSJ6#4
 */
class CubesScene : public IRenderableScene {

  using Index = std::tuple<unsigned, unsigned, unsigned>;

public:
  CubesScene(ICanvas* iCanvas);
  ~CubesScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  float _t;
  std::vector<MeshPtr> _boxes;
  std::vector<Index> _indices;

}; // end of class CubesScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_CUBES_SCENE_H
