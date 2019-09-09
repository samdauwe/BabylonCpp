#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_BLOCK_CONNECTION_POINT_MODE_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_BLOCK_CONNECTION_POINT_MODE_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Enum defining the mode of a NodeMaterialBlockConnectionPoint.
 */
enum class BABYLON_SHARED_EXPORT
  NodeMaterialBlockConnectionPointMode : unsigned int {
    /** Value is an uniform */
    Uniform,
    /** Value is a mesh attribute */
    Attribute,
    /** Value is a varying between vertex and fragment shaders */
    Varying,
    /** Mode is undefined */
    Undefined,
  }; // end of enum class NodeMaterialBlockConnectionPointMode

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_NODE_NODE_MATERIAL_BLOCK_CONNECTION_POINT_MODE_H
