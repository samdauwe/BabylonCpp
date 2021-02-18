#ifndef BABYLON_LIGHTS_SHADOWS_ICUSTOM_SHADER_OPTIONS_H
#define BABYLON_LIGHTS_SHADOWS_ICUSTOM_SHADER_OPTIONS_H

#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines the options associated with the creation of a custom shader
 * for a shadow generator.
 */
struct BABYLON_SHARED_EXPORT ICustomShaderOptions {

  /**
   * Gets or sets the custom shader name to use
   */
  std::string shaderName;

  /**
   * The list of attribute names used in the shader
   */
  std::vector<std::string> attributes;

  /**
   * The list of uniform names used in the shader
   */
  std::vector<std::string> uniforms;

  /**
   * The list of sampler names used in the shader
   */
  std::vector<std::string> samplers;

  /**
   * The list of defines used in the shader
   */
  std::vector<std::string> defines;

}; // end of struct ICustomShaderOptions

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H
