#ifndef BABYLON_ENGINES_SHADER_STORE_H
#define BABYLON_ENGINES_SHADER_STORE_H

#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>

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
  /**
   * Gets or sets the relative url used to load shaders (WGSL) if using the engine in non-minified
   * mode
   */
  static std::string ShadersRepositoryWGSL;
  /**
   * Store of each shader  (WGSL)
   */
  static std::unordered_map<std::string, std::string> ShadersStoreWGSL;
  /**
   * Store of each included file for a shader (WGSL)
   */
  static std::unordered_map<std::string, std::string> IncludesShadersStoreWGSL;

  /**
   * @brief Gets the shaders repository path for a given shader language.
   * @param shaderLanguage the shader language
   * @returns the path to the shaders repository
   */
  static std::string& GetShadersRepository(const ShaderLanguage shaderLanguage
                                           = ShaderLanguage::GLSL);

  /**
   * @brief Gets the shaders store of a given shader language.
   * @param shaderLanguage the shader language
   * @returns the shaders store
   */
  static std::unordered_map<std::string, std::string>& GetShadersStore(const ShaderLanguage
                                                                       = ShaderLanguage::GLSL);

  /**
   * @brief Gets the include shaders store of a given shader language.
   * @param shaderLanguage the shader language
   * @returns the include shaders store
   */
  static std::unordered_map<std::string, std::string>&
  GetIncludesShadersStore(const ShaderLanguage = ShaderLanguage::GLSL);

}; // end of struct ShaderStore

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_SHADER_STORE_H
