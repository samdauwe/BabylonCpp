#ifndef BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_SUZANNE_SCENE_H
#define BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_SUZANNE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <vector>

namespace BABYLON {

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

namespace Samples {

/**
 * @brief Import Suzanne Scene.
 * @see
 * https://www.davrous.com/2013/06/17/tutorial-part-3-learning-how-to-write-a-3d-soft-engine-in-c-ts-or-js-loading-meshes-exported-from-blender/
 * @see http://david.blob.core.windows.net/softengine3d/part6webgl/index.html
 */
class ImportSuzanneScene : public IRenderableScene {

public:
  ImportSuzanneScene(ICanvas* iCanvas);
  ~ImportSuzanneScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  std::vector<AbstractMeshPtr> _meshes;

}; // end of class ImportSuzanneScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_LOADERS_BABYLON_IMPORT_SUZANNE_SCENE_H
