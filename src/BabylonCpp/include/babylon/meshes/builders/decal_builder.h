#ifndef BABYLON_MESHES_BUILDERS_DECAL_BUILDER_H
#define BABYLON_MESHES_BUILDERS_DECAL_BUILDER_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class AbstractMesh;
class DecalOptions;
class Mesh;
class Scene;
using MeshPtr         = std::shared_ptr<Mesh>;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

/**
 * @brief Class containing static functions to help procedurally build meshes.
 */
struct BABYLON_SHARED_EXPORT DecalBuilder {

  /**
   * @brief Creates a decal mesh.
   * A decal is a mesh usually applied as a model onto the surface of another
   * mesh. So don't forget the parameter `sourceMesh` depicting the decal
   * * The parameter `position` (Vector3, default `(0, 0, 0)`) sets the position
   * of the decal in World coordinates
   * * The parameter `normal` (Vector3, default `Vector3.Up`) sets the normal of
   * the mesh where the decal is applied onto in World coordinates
   * * The parameter `size` (Vector3, default `(1, 1, 1)`) sets the decal
   * scaling
   * * The parameter `angle` (float in radian, default 0) sets the angle to
   * rotate the decal
   * @param name defines the name of the mesh
   * @param sourceMesh defines the mesh where the decal must be applied
   * @param options defines the options used to create the mesh
   * @param scene defines the hosting scene
   * @returns the decal mesh
   * @see https://doc.babylonjs.com/how_to/decals
   */
  static MeshPtr CreateDecal(const std::string& name,
                             const AbstractMesh& sourceMesh,
                             DecalOptions& options, Scene* scene = nullptr);

}; // end of struct DecalBuilder

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_DECAL_BUILDER_H
