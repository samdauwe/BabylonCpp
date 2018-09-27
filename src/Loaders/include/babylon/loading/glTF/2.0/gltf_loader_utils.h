#ifndef BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_UTILS_H
#define BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_UTILS_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace GLTF2 {

/**
 * @brief Utils functions for GLTF.
 */
struct BABYLON_SHARED_EXPORT GLTFUtils {

  /**
   * @brief Tests if the uri is a base64 string.
   * @param uri: the uri to test.
   */
  static bool IsBase64(const std::string& uri);

  /**
   * @brief Decodes the base64 uri.
   * @param uri: the uri to decode.
   */
  static Uint8Array DecodeBase64(const std::string& uri);

  /**
   * @brief Validates the uri.
   * @param uri: the uri to validate.
   */
  static bool ValidateUri(const std::string& uri);

}; // end of struct GLTFUtils

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_2_0_GLTF_LOADER_UTILS_H
