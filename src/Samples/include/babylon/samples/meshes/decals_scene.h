#ifndef BABYLON_SAMPLES_MESHES_DECALS_SCENE_H
#define BABYLON_SAMPLES_MESHES_DECALS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
struct PointerEvent;
class StandardMaterial;
using ArcRotateCameraPtr  = std::shared_ptr<ArcRotateCamera>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

namespace Samples {

/**
 * @brief Decals Scene. Example demonstrating how to create decals to apply
 * additional textures to a portion of a mesh.
 * @see https://www.babylonjs-playground.com/#EEUVTY#0
 * @see https://doc.babylonjs.com/how_to/decals
 */
class DecalsScene : public IRenderableScene {

public:
  DecalsScene(ICanvas* iCanvas);
  ~DecalsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  ArcRotateCameraPtr _camera;
  StandardMaterialPtr _decalMaterial;
  std::function<void(PointerEvent&& evt)> _onPointerDown;

}; // end of class DecalsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_DECALS_SCENE_H
