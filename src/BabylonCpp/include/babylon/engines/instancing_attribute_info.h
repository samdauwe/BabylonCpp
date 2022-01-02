#ifndef BABYLON_ENGINES_INSTANCING_ATTRIBUTE_INFO_H
#define BABYLON_ENGINES_INSTANCING_ATTRIBUTE_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

/**
 * @brief Interface for attribute information associated with buffer instanciation.
 */
struct BABYLON_SHARED_EXPORT InstancingAttributeInfo {
  /**
   * Name of the GLSL attribute
   * if attribute index is not specified, this is used to retrieve the index from the effect
   */
  std::string attributeName;

  /**
   * Index/offset of the attribute in the vertex shader
   * if not specified, this will be computes from the name.
   */
  std::optional<unsigned int> index = std::nullopt;

  /**
   * size of the attribute, 1, 2, 3 or 4
   */
  int attributeSize = 0;

  /**
   * Offset of the data in the Vertex Buffer acting as the instancing buffer
   */
  int offset = 0;

  /**
   * Modifies the rate at which generic vertex attributes advance when rendering multiple instances
   * default to 1
   */
  std::optional<unsigned int> divisor = std::nullopt;

  /**
   * type of the attribute, gl.BYTE, gl.UNSIGNED_BYTE, gl.SHORT, gl.UNSIGNED_SHORT, gl.FIXED,
   * gl.FLOAT. default is FLOAT
   */
  std::optional<unsigned int> attributeType = std::nullopt;

  /**
   * normalization of fixed-point data. behavior unclear, use FALSE, default is FALSE
   */
  std::optional<bool> normalized = std::nullopt;

}; // end of struct InstancingAttributeInfo

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_INSTANCING_ATTRIBUTE_INFO_H
