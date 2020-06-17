#ifndef BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_MODES_H
#define BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_MODES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Enum used to define the material modes.
 */
enum class BABYLON_SHARED_EXPORT NodeMaterialModes : unsigned int {
  /** Regular material */
  Material = 0,
  /** For post process */
  PostProcess = 1,
  /** For particle system */
  Particle = 2,
}; // end of enum class NodeMaterialModes

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_ENUMS_NODE_MATERIAL_MODES_H
