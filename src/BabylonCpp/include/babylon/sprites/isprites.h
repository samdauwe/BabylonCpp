#ifndef BABYLON_SPRITES_ISPRITES_H
#define BABYLON_SPRITES_ISPRITES_H

#include <optional>

#include <nlohmann/json.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

using json = nlohmann::json;

namespace BABYLON {

/**
 * @brief Defines the basic options interface of a Sprite Frame Source Size.
 */
struct BABYLON_SHARED_EXPORT ISpriteJSONSpriteSourceSize {
  /**
   * number of the original width of the Frame
   */
  uint32_t w;

  /**
   * number of the original height of the Frame
   */
  uint32_t h;
}; // end of struct ISpriteJSONSpriteSourceSize

/**
 * @brief Defines the basic options interface of a Sprite Frame Data.
 */
struct BABYLON_SHARED_EXPORT ISpriteJSONSpriteFrameData {
  /**
   * number of the x offset of the Frame
   */
  float x;

  /**
   * number of the y offset of the Frame
   */
  float y;

  /**
   * number of the width of the Frame
   */
  float w;

  /**
   * number of the height of the Frame
   */
  float h;
}; // end of struct ISpriteJSONSpriteFrameData

/**
 * @brief Defines the basic options interface of a JSON Sprite.
 */
struct BABYLON_SHARED_EXPORT ISpriteJSONSprite {
  /**
   * string name of the Frame
   */
  std::string filename;

  /**
   * ISpriteJSONSpriteFrame basic object of the frame data
   */
  ISpriteJSONSpriteFrameData frame;

  /**
   * boolean to flag is the frame was rotated.
   */
  bool rotated;

  /**
   * boolean to flag is the frame was trimmed.
   */
  bool trimmed;

  /**
   * ISpriteJSONSpriteFrame basic object of the source data
   */
  ISpriteJSONSpriteFrameData spriteSourceSize;

  /**
   * ISpriteJSONSpriteFrame basic object of the source data
   */
  ISpriteJSONSpriteSourceSize sourceSize;
}; // end of struct ISpriteJSONSprite

/**
 * @brief Defines the basic options interface of a JSON atlas.
 */
struct BABYLON_SHARED_EXPORT ISpriteJSONAtlas {
  /**
   * Array of objects that contain the frame data.
   */
  std::vector<ISpriteJSONSprite> frames;

  /**
   * object basic object containing the sprite meta data.
   */
  std::optional<json> meta = std::nullopt;
}; // end of struct ISpriteJSONAtlas

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_ISPRITES_H
