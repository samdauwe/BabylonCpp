#ifndef BABYLON_MATERIALS_IIOPTION_SHADOW_DEPTH_MATERIAL_H
#define BABYLON_MATERIALS_IIOPTION_SHADOW_DEPTH_MATERIAL_H

#include <optional>
#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Options to be used when creating a shadow depth material.
 */
struct BABYLON_SHARED_EXPORT IIOptionShadowDepthMaterial {
  /**
   * Variables in the vertex shader code that need to have their names remapped.
   * The format is: ["var_name", "var_remapped_name", "var_name", "var_remapped_name", ...]
   * "var_name" should be either: worldPos or vNormalW
   * So, if the variable holding the world position in your vertex shader is not named worldPos, you
   * must tell the system the name to use instead by using: ["worldPos", "myWorldPosVar"] assuming
   * the variable is named myWorldPosVar in your code. If the normal must also be remapped:
   * ["worldPos", "myWorldPosVar", "vNormalW", "myWorldNormal"]
   */
  std::vector<std::string> remappedVariables;

  /**
   * Set standalone to true if the base material wrapped by ShadowDepthMaterial is not used for a
   * regular object but for depth shadow generation only
   */
  std::optional<bool> standalone = std::nullopt;
}; // end of struct IIOptionShadowDepthMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IIOPTION_SHADOW_DEPTH_MATERIAL_H
