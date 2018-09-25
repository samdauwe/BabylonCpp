#ifndef BABYLON_SAMPLES_AIRPLANE_MODEL_SCENE_H
#define BABYLON_SAMPLES_AIRPLANE_MODEL_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Airplane model scene.
 * @see https://www.babylonjs-playground.com/#H3Z6J1#7
 */
class AirplaneModelScene : public IRenderableScene {

  using MakeWingFunction = std::function<MeshPtr(
    const MeshPtr& mesh, float radius, float fuselageXer, bool invertY)>;

public:
  AirplaneModelScene(ICanvas* iCanvas);
  ~AirplaneModelScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  void _reBake(const MeshPtr& mesh);
  MakeWingFunction _makeFormAppendage(float frontBaseV, float backBaseV,
                                      float ySize, float xOrigin, float yOrigin,
                                      float baseXsize, float xSweepback);

}; // end of class AirplaneModelScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_AIRPLANE_MODEL_SCENE_H
