#ifndef BABYLON_MISC_INTERFACES_ISCREENSHOT_SIZE_H
#define BABYLON_MISC_INTERFACES_ISCREENSHOT_SIZE_H

#include <optional>

namespace BABYLON {

/**
 * @brief Interface for screenshot methods with describe argument called `size` as object with
 * options
 * @link https://doc.babylonjs.com/api/classes/babylon.screenshottools
 */
struct IScreenshotSize {
  /**
   * number in pixels for canvas height
   */
  std::optional<size_t> height = std::nullopt;

  /**
   * multiplier allowing render at a higher or lower resolution
   * If value is defined then height and width will be ignored and taken from camera
   */
  std::optional<float> precision = std::nullopt;

  /**
   * number in pixels for canvas width
   */
  std::optional<size_t> width = std::nullopt;
}; // end of struct IScreenshotSize

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_HIGH_DYNAMIC_RANGE_CUBE_MAP_INFO_H
