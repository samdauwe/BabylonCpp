#ifndef BABYLON_MATERIALS_NODE_INODE_MATERIAL_OPTIONS_H
#define BABYLON_MATERIALS_NODE_INODE_MATERIAL_OPTIONS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class used to configure NodeMaterial.
 */
struct BABYLON_SHARED_EXPORT INodeMaterialOptions {
  /**
   * Defines if blocks should emit comments
   */
  bool emitComments;
}; // end of struct INodeMaterialOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_INODE_MATERIAL_OPTIONS_H
