#ifndef BABYLON_MISC_IINSPECTABLE_H
#define BABYLON_MISC_IINSPECTABLE_H

#include <optional>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Enum that determines the text-wrapping mode to use.
 */
enum class InspectableType {
  /**
   * Checkbox for booleans
   */
  Checkbox = 0,
  /**
   * Sliders for numbers
   */
  Slider = 1,
  /**
   * Vector3
   */
  Vector3 = 2,
  /**
   * Quaternions
   */
  Quaternion = 3,
  /**
   * Color3
   */
  Color3 = 4,
  /**
   * String
   */
  String = 5,
};

/**
 * @brief Interface used to define custom inspectable properties.
 * This interface is used by the inspector to display custom property grids
 * @see https://doc.babylonjs.com/how_to/debug_layer#extensibility
 */
struct BABYLON_SHARED_EXPORT IInspectable {
  /**
   * Gets the label to display
   */
  std::string label;
  /**
   * Gets the name of the property to edit
   */
  std::string propertyName;
  /**
   * Gets the type of the editor to use
   */
  InspectableType type;
  /**
   * Gets the minimum value of the property when using in "slider" mode
   */
  std::optional<float> min = std::nullopt;
  /**
   * Gets the maximum value of the property when using in "slider" mode
   */
  std::optional<float> max = std::nullopt;
  /**
   * Gets the setp to use when using in "slider" mode
   */
  std::optional<float> step = std::nullopt;
}; // end of class IInspectable

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_IINSPECTABLE_H
