#ifndef BABYLON_MESHES_BUILDERS_CAPSULE_BUILDER_H
#define BABYLON_MESHES_BUILDERS_CAPSULE_BUILDER_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class ICreateCapsuleOptions;
class Scene;
class SphereOptions;
FWD_CLASS_SPTR(Mesh)

/**
 * @brief Class containing static functions to help procedurally build meshes.
 */
struct BABYLON_SHARED_EXPORT CapsuleBuilder {

  /**
   * @brief Creates a capsule or a pill mesh.
   * @param name defines the name of the mesh
   * @param options The constructors options.
   * @param scene The scene the mesh is scoped to.
   * @returns Capsule Mesh
   */
  static MeshPtr CreateCapsule(const std::string& name, ICreateCapsuleOptions& options,
                               Scene* scene = nullptr);

}; // end of struct CapsuleBuilder

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_CAPSULE_BUILDER_H
