#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_DECORATOR_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_DECORATOR_H

#include <optional>
#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Enum defining the type of properties that can be edited in the property pages in the NME.
 */
enum class PropertyTypeForEdition {
  /** property is a boolean */
  Boolean,
  /** property is a float */
  Float,
  /** property is a Vector2 */
  Vector2,
  /** property is a list of values */
  List,
}; // end of enum class PropertyTypeForEdition

/**
 * @brief Interface that defines an option in a variable of type list
 */
struct BABYLON_SHARED_EXPORT IEditablePropertyListOption {
  /** label of the option */
  std::string label;
  /** value of the option */
  std::string value;
}; // end of struct IEditablePropertyListOption

/**
 * @brief Interface that defines the options available for an editable property.
 */
struct BABYLON_SHARED_EXPORT IEditablePropertyOption {
  /** min value */
  std::optional<float> min = std::nullopt;
  /** max value */
  std::optional<float> max = std::nullopt;
  /** notifiers: indicates which actions to take when the property is changed */
  struct Notifiers {
    /** the material should be rebuilt */
    std::optional<bool> rebuild = std::nullopt;
    /** the preview should be updated */
    std::optional<bool> update = std::nullopt;
  };
  std::optional<Notifiers> notifiers = std::nullopt;
  /** list of the options for a variable of type list */
  std::vector<IEditablePropertyListOption> options;
}; // end of struct IEditablePropertyListOption

/**
 * @brief Interface that describes an editable property.
 */
struct BABYLON_SHARED_EXPORT IPropertyDescriptionForEdition {
  /** name of the property */
  std::string propertyName;
  /** display name of the property */
  std::string displayName;
  /** type of the property */
  PropertyTypeForEdition type;
  /** group of the property - all properties with the same group value will be displayed in a
   * specific section */
  std::string groupName;
  /** options for the property */
  IEditablePropertyOption options;
}; // end of struct IPropertyDescriptionForEdition

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_DECORATOR_H
