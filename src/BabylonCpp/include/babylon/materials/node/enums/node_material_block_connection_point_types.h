#ifndef BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_BLOCK_CONNECTION_POINT_TYPES_H
#define BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_BLOCK_CONNECTION_POINT_TYPES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines the kind of connection point for node based material.
 */
enum class BABYLON_SHARED_EXPORT NodeMaterialBlockConnectionPointTypes : unsigned int {
  /** Float */
  Float = 1,
  /** Int */
  Int = 2,
  /** Vector2 */
  Vector2 = 4,
  /** Vector3 */
  Vector3 = 8,
  /** Vector4 */
  Vector4 = 16,
  /** Color3 */
  Color3 = 32,
  /** Color4 */
  Color4 = 64,
  /** Matrix */
  Matrix = 128,
  /** Custom object */
  Object = 256,
  /** Detect type based on connection */
  AutoDetect = 1024,
  /** Output type that will be defined by input type */
  BasedOnInput = 2048
}; // end of enum class NodeMaterialBlockConnectionPointTypes

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_BLOCK_CONNECTION_POINT_TYPES_H
