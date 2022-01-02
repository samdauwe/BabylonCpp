#ifndef BABYLON_MATERIALS_ISHADER_MATERIAL_OPTIONS_H
#define BABYLON_MATERIALS_ISHADER_MATERIAL_OPTIONS_H

#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines the options associated with the creation of a shader material.
 */
struct BABYLON_SHARED_EXPORT IShaderMaterialOptions {
  /**
   * Does the material work in alpha blend mode
   */
  bool needAlphaBlending = false;

  /**
   * Does the material work in alpha test mode
   */
  bool needAlphaTesting = false;

  /**
   * The list of attribute names used in the shader
   */
  std::vector<std::string> attributes = {"position", "normal", "uv"};

  /**
   * The list of unifrom names used in the shader
   */
  std::vector<std::string> uniforms = {"worldViewProjection"};

  /**
   * The list of UBO names used in the shader
   */
  std::vector<std::string> uniformBuffers;

  /**
   * The list of sampler names used in the shader
   */
  std::vector<std::string> samplers;

  /**
   * The list of defines used in the shader
   */
  std::vector<std::string> defines;
}; // end of struct IShaderMaterialOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_ISHADER_MATERIAL_OPTIONS_H
