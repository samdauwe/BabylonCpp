#ifndef BABYLON_SAMPLES_GENERIC_INFINITE_LOADER_SCENE_H
#define BABYLON_SAMPLES_GENERIC_INFINITE_LOADER_SCENE_H

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

class InfiniteLoaderScene : public IRenderableScene {

public:
  InfiniteLoaderScene(ICanvas* iCanvas);
  ~InfiniteLoaderScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  std::vector<Ring> _rings;

}; // end of class InfiniteLoaderScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_GENERIC_INFINITE_LOADER_SCENE_H
