#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_SYSTEM_VALUES_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_SYSTEM_VALUES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Enum used to define system values e.g. values automatically provided
 * by the system.
 */
enum class BABYLON_SHARED_EXPORT NodeMaterialSystemValues : unsigned int {
  /** World */
  World = 1,
  /** View */
  View = 2,
  /** Projection */
  Projection = 3,
  /** ViewProjection */
  ViewProjection = 4,
  /** WorldView */
  WorldView = 5,
  /** WorldViewProjection */
  WorldViewProjection = 6,
  /** CameraPosition */
  CameraPosition = 7,
  /** Fog Color */
  FogColor = 8
}; // end of enum class NodeMaterialSystemValues

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_SYSTEM_VALUES_H
