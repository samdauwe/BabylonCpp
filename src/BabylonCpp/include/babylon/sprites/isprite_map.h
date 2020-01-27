#ifndef BABYLON_SPRITES_ISPRITE_MAP_H
#define BABYLON_SPRITES_ISPRITE_MAP_H

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/sprites/isprite_map_options.h>
#include <babylon/sprites/isprites.h>

namespace BABYLON {

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

/**
 * @brief Defines the IDisposable interface in order to be cleanable from resources.
 */
struct BABYLON_SHARED_EXPORT ISpriteMap : public IDisposable {

  /**
   * String name of the SpriteMap.
   */
  std::string name;

  /**
   * The JSON Array file from a https://www.codeandweb.com/texturepacker export.  Or similar
   * structure.
   */
  ISpriteJSONAtlas atlasJSON;

  /**
   * Texture of the SpriteMap.
   */
  TexturePtr spriteSheet = nullptr;

  /**
   * The parameters to initialize the SpriteMap with.
   */
  ISpriteMapOptions options;

}; // end of struct ISpriteJSONSprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_ISPRITE_MAP_H
