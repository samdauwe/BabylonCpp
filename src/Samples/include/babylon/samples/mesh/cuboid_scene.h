#ifndef BABYLON_SAMPLES_MESH_CUBOID_SCENE_H
#define BABYLON_SAMPLES_MESH_CUBOID_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

class CuboidScene : public IRenderableScene {

public:
  CuboidScene(ICanvas* iCanvas);
  ~CuboidScene();

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

  /**
   * @brief Creates a 3D rectangle or cuboid directly without scaling a cube.
   * @param name name of the mesh
   * @param length size in the x-direction
   * @param width size in the z-direction
   * @param height size in the y-direction
   * @param scene scene
   * @return cuboid
   */
  static Mesh* CreateCuboid(const std::string& name, float length, float width,
                            float height, Scene* scene);

private:
  Mesh* _cuboid;

}; // end of class CuboidScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESH_CUBOID_SCENE_H
