#ifndef BABYLON_MATERIALS_NODE_INODE_MATERIAL_EDITOR_OPTIONS_H
#define BABYLON_MATERIALS_NODE_INODE_MATERIAL_EDITOR_OPTIONS_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Interface used to configure the node material editor.
 */
struct BABYLON_SHARED_EXPORT INodeMaterialEditorOptions {
  /**
   * Define the URl to load node editor script
   */
  std::string editorURL;
}; // end of struct INodeMaterialEditorOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_INODE_MATERIAL_EDITOR_OPTIONS_H
