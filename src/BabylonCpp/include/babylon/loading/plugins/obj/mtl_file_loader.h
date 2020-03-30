#ifndef BABYLON_LOADING_PLUGINS_OBJ_MTL_FILE_LOADER_H
#define BABYLON_LOADING_PLUGINS_OBJ_MTL_FILE_LOADER_H

#include <memory>
#include <variant>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Scene;
class StandardMaterial;
class Texture;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;
using TexturePtr          = std::shared_ptr<Texture>;

/**
 * @brief Class reading and parsing the MTL file bundled with the obj file.
 */
class BABYLON_SHARED_EXPORT MTLFileLoader {

public:
  /**
   * Invert Y-Axis of referenced textures on load
   */
  static bool INVERT_TEXTURE_Y;

public:
  MTLFileLoader();
  ~MTLFileLoader() = default;

  /**
   * @brief This function will read the mtl file and create each material described inside.
   * This function could be improve by adding :
   * -some component missing (Ni, Tf...)
   * -including the specific options available
   *
   * @param scene defines the scene the material will be created in
   * @param data defines the mtl data to parse
   * @param rootUrl defines the rooturl to use in order to load relative dependencies
   * @param forAssetContainer defines if the material should be registered in the scene
   */
  void parseMTL(Scene* scene, const std::variant<std::string, ArrayBuffer>& data,
                const std::string& rootUrl, bool forAssetContainer);

  /**
   * @brief Gets the texture for the material.
   *
   * If the material is imported from input file,
   * We sanitize the url to ensure it takes the textre from aside the material.
   *
   * @param rootUrl The root url to load from
   * @param value The value stored in the mtl
   * @return The Texture
   */
  static TexturePtr _getTexture(const std::string& rootUrl, const std::string& value, Scene* scene);

public:
  /**
   * All material loaded from the mtl will be set here
   */
  std::vector<StandardMaterialPtr> materials;

}; // end of class MTLFileLoader

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_OBJ_MTL_FILE_LOADER_H
