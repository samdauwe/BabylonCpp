#ifndef BABYLON_MISC_IINSPECTABLE_H
#define BABYLON_MISC_IINSPECTABLE_H

#include <functional>
#include <optional>
#include <string>
#include <variant>
#include <vector>

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
  /**
   * Button
   */
  Button = 6,
  /**
   * Options
   */
  Options = 7,
  /**
   * Tab
   */
  Tab = 8,
}; // end of class InspectableType

/**
 * Interface used to define custom inspectable options in "Options" mode.
 * This interface is used by the inspector to display the list of options
 */
struct BABYLON_SHARED_EXPORT IInspectableOptions {
  /**
   * Defines the visible part of the option
   */
  std::string label;
  /**
   * Defines the value part of the option (returned through the callback)
   */
  std::variant<float, std::string> value;

  /**
   * Defines if the option should be selected or not
   */
  std::optional<bool> selected = std::nullopt;
}; // end of class IInspectableOptions

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
  /**
   * Gets the callback function when using "Button" mode
   */
  std::function<void()> callback = nullptr;
  /**
   * Gets the list of options when using "Option" mode
   */
  std::vector<IInspectableOptions> options;
}; // end of class IInspectable

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_IINSPECTABLE_H
