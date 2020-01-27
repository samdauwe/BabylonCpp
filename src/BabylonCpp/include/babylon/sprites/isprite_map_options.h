#ifndef BABYLON_SPRITES_ISPRITE_MAP_OPTIONS_H
#define BABYLON_SPRITES_ISPRITE_MAP_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/maths/vector2.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

/**
 * @brief Defines the basic options interface of a SpriteMap.
 */
struct BABYLON_SHARED_EXPORT ISpriteMapOptions {

  /**
   * Vector2 of the number of cells in the grid.
   */
  std::optional<Vector2> stageSize = std::nullopt;

  /**
   * Vector2 of the size of the output plane in World Units.
   */
  std::optional<Vector2> outputSize = std::nullopt;

  /**
   * Vector3 of the position of the output plane in World Units.
   */
  std::optional<Vector3> outputPosition = std::nullopt;

  /**
   * Vector3 of the rotation of the output plane.
   */
  std::optional<Vector3> outputRotation = std::nullopt;

  /**
   * number of layers that the system will reserve in resources.
   */
  std::optional<uint32_t> layerCount = std::nullopt;

  /**
   * number of max animation frames a single cell will reserve in resources.
   */
  std::optional<uint32_t> maxAnimationFrames = std::nullopt;

  /**
   * number cell index of the base tile when the system compiles.
   */
  std::optional<uint32_t> baseTile = std::nullopt;

  /**
   * boolean flip the sprite after its been repositioned by the framing data.
   */
  std::optional<bool> flipU = std::nullopt;

  /**
   * Vector3 scalar of the global RGB values of the SpriteMap.
   */
  std::optional<Vector3> colorMultiply = std::nullopt;

}; // end of struct ISpriteMapOptions

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_ISPRITE_MAP_OPTIONS_H
