#ifndef BABYLON_SPRITES_SPRITE_PACKED_MANAGER_H
#define BABYLON_SPRITES_SPRITE_PACKED_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

class SpritePackedManager;
using SpritePackedManagerPtr = std::shared_ptr<SpritePackedManager>;

/**
 * @brief Class used to manage multiple sprites of different sizes on the same spritesheet.
 * @see https://doc.babylonjs.com/babylon101/sprites
 */
class BABYLON_SHARED_EXPORT SpritePackedManager : public SpriteManager {

public:
  template <typename... Ts>
  static SpritePackedManagerPtr New(Ts&&... args)
  {
    auto spriteManager
      = std::shared_ptr<SpritePackedManager>(new SpritePackedManager(std::forward<Ts>(args)...));
    spriteManager->addToScene(spriteManager);

    return spriteManager;
  }
  ~SpritePackedManager() override; // = default

protected:
  /**
   * @brief Creates a new sprite manager from a packed sprite sheet.
   * @param name defines the manager's name
   * @param imgUrl defines the sprite sheet url
   * @param capacity defines the maximum allowed number of sprites
   * @param scene defines the hosting scene
   * @param spriteJSON null otherwise a JSON object defining sprite sheet data
   * @param epsilon defines the epsilon value to align texture (0.01 by default)
   * @param samplingMode defines the smapling mode to use with spritesheet
   * @param fromPacked set to true; do not alter
   */
  SpritePackedManager(const std::string& name, const std::string& imgUrl, unsigned int capacity,
                      Scene* scene, const std::string& spriteJSON = "", float epsilon = 0.01f,
                      unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE);

}; // end of class SpritePackedManager

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_PACKED_MANAGER_H
