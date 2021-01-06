#ifndef BABYLON_MESHES_BUILDERS_DISC_BUILDER_H
#define BABYLON_MESHES_BUILDERS_DISC_BUILDER_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class DiscOptions;
class Scene;
FWD_CLASS_SPTR(Mesh)

/**
 * @brief Class containing static functions to help procedurally build meshes.
 */
struct BABYLON_SHARED_EXPORT DiscBuilder {

  // clang-format off
  /**
   * @brief Creates a plane polygonal mesh.  By default, this is a disc.
   * * The parameter `radius` sets the radius size (float) of the polygon (default 0.5)
   * * The parameter `tessellation` sets the number of polygon sides (positive integer, default 64). So a tessellation valued to 3 will build a triangle, to 4 a square, etc
   * * You can create an unclosed polygon with the parameter `arc` (positive float, default 1), valued between 0 and 1, what is the ratio of the circumference : 2 x PI x ratio
   * * You can also set the mesh side orientation with the values : BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or BABYLON.Mesh.DOUBLESIDE
   * * If you create a double-sided mesh, you can choose what parts of the texture image to crop and stick respectively on the front and the back sides with the parameters `frontUVs` and `backUVs` (Vector4). Detail here : https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * * The mesh can be set to updatable with the boolean parameter `updatable` (default false) if its internal geometry is supposed to change once created
   * @param name defines the name of the mesh
   * @param options defines the options used to create the mesh
   * @param scene defines the hosting scene
   * @returns the plane polygonal mesh
   * @see https://doc.babylonjs.com/how_to/set_shapes#disc-or-regular-polygon
   */
  // clang-format on
  static MeshPtr CreateDisc(const std::string& name, DiscOptions& options, Scene* scene = nullptr);

}; // end of struct DiscBuilder

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_DISC_BUILDER_H
