#ifndef BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_UTILS_H
#define BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_UTILS_H

#include <babylon/babylon_global.h>
#include <babylon/core/any.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader_interfaces.h>

namespace BABYLON {

/**
 * @brief Utils functions for GLTF.
 */
struct BABYLON_SHARED_EXPORT GLTFUtils {

  /**
   * @brief Sets the given "parameter" matrix
   * @param scene: the {BABYLON.Scene} object
   * @param source: the source node where to pick the matrix
   * @param parameter: the GLTF technique parameter
   * @param uniformName: the name of the shader's uniform
   * @param shaderMaterial: the shader material
   */
  static void SetMatrix(Scene* scene, Node* source,
                        const IGLTFTechniqueParameter& parameter,
                        const std::string& uniformName,
                        ShaderMaterial* shaderMaterial);
  /**
   * Sets the given "parameter" matrix
   * @param shaderMaterial: the shader material
   * @param uniform: the name of the shader's uniform
   * @param value: the value of the uniform
   * @param type: the uniform's type (EParameterType FLOAT, VEC2, VEC3 or VEC4)
   */
  static bool SetUniform(ShaderMaterial* shaderMaterial,
                         const std::string& uniform, any value,
                         EParameterType type);

  /**
   * If the uri is a base64 string
   * @param uri: the uri to test
   */
  static bool IsBase64(const std::string& uri);

  /**
   * Returns the wrap mode of the texture
   * @param mode: the mode value
   */
  static unsigned int GetWrapMode(ETextureWrapMode mode);

  /**
   * Returns the byte stride giving an accessor
   * @param accessor: the GLTF accessor objet
   */
  static unsigned int GetByteStrideFromType(const IGLTFAccessor& accessor);

  /**
   * Returns the texture filter mode giving a mode value
   * @param mode: the filter mode value
   */
  static unsigned int GetTextureFilterMode(ETextureFilterType mode);

}; // end of struct GLTFUtils

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_UTILS_H
