#ifndef BABYLON_SAMPLES_MESHES_MESH_UTILS_H
#define BABYLON_SAMPLES_MESHES_MESH_UTILS_H

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

namespace Samples {

struct MeshUtils {

  /**
   * @brief Creates a 12 Pointed Star.
   */
  static MeshPtr CreateTwelvePointedStar(size_t len, Scene* scene);

}; // end of struct MeshUtils

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_MESHES_MESH_UTILS_H
