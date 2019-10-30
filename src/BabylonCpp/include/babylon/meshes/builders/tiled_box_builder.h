#ifndef BABYLON_MESHES_BUILDERS_TILED_BOX_BUILDER_H
#define BABYLON_MESHES_BUILDERS_TILED_BOX_BUILDER_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Mesh;
class Scene;
class TiledBoxOptions;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Class containing static functions to help procedurally build meshes.
 */
struct BABYLON_SHARED_EXPORT TiledBoxBuilder {

  /**
   * @brief Creates a box mesh.
   * faceTiles sets the pattern, tile size and number of tiles for a face     *
   * * You can set different colors and different images to each box side by
   * using the parameters `faceColors` (an array of 6 Color3 elements) and
   * `faceUV` (an array of 6 Vector4 elements)
   * * Please read this tutorial :
   * https://doc.babylonjs.com/how_to/createbox_per_face_textures_and_colors
   * * You can also set the mesh side orientation with the values :
   * BABYLON.Mesh.FRONTSIDE (default), BABYLON.Mesh.BACKSIDE or
   * BABYLON.Mesh.DOUBLESIDE
   * * If you create a double-sided mesh, you can choose what parts of the
   * texture image to crop and stick respectively on the front and the back
   * sides with the parameters `frontUVs` and `backUVs` (Vector4). Detail here :
   * https://doc.babylonjs.com/babylon101/discover_basic_elements#side-orientation
   * * The mesh can be set to updatable with the boolean parameter `updatable`
   * (default false) if its internal geometry is supposed to change once created
   * @param name defines the name of the mesh
   * @param options defines the options used to create the mesh
   * @param scene defines the hosting scene
   * @returns the box mesh
   */
  static MeshPtr CreateTiledBox(const std::string& name,
                                TiledBoxOptions& options,
                                Scene* scene = nullptr);

}; // end of struct TiledBoxBuilder

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUILDERS_TILED_BOX_BUILDER_H
