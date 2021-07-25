#ifndef BABYLON_ENGINES_SHADER_STORE_H
#define BABYLON_ENGINES_SHADER_STORE_H

#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines the shader related stores and directory.
 */
struct BABYLON_SHARED_EXPORT ShaderStore {
  /**
   * Gets or sets the relative url used to load shaders if using the engine in non-minified mode
   */
  static std::string ShadersRepository;
  /**
   * Store of each shader (The can be looked up using effect.key)
   */
  static std::unordered_map<std::string, std::string> ShadersStore;
  /**
   * Store of each included file for a shader (The can be looked up using effect.key)
   */
  static std::unordered_map<std::string, std::string> IncludesShadersStore;
}; // end of struct ShaderStore

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_SHADER_STORE_H
