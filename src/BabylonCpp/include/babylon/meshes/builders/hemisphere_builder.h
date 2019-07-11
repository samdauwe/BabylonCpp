#ifndef BABYLON_MESHES_BUILDERS_HEMISPHERE_BUILDER_H
#define BABYLON_MESHES_BUILDERS_HEMISPHERE_BUILDER_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class HemisphereOptions;
class Mesh;
class Scene;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Class containing static functions to help procedurally build meshes.
 */
struct BABYLON_SHARED_EXPORT HemisphereBuilder {

  /**
   * @brief Creates a hemisphere mesh.
   * @param name defines the name of the mesh
   * @param options defines the options used to create the mesh
   * @param scene defines the hosting scene
   * @returns the hemisphere mesh
   */
  static MeshPtr CreateHemisphere(const std::string& name,
                                  HemisphereOptions& options,
                                  Scene* scene = nullptr);

}; // end of struct HemisphereBuilder

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_HEMISPHERE_BUILDER_H
