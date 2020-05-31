#ifndef BABYLON_MATERIALS_NODE_NODE_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_NODE_NODE_MATERIAL_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

/**
 * @brief Manages the defines for the Node Material.
 */
struct BABYLON_SHARED_EXPORT NodeMaterialDefines : public MaterialDefines,
                                                   public IImageProcessingConfigurationDefines {

  /**
   * @brief Initializes the Node Material defines.
   */
  NodeMaterialDefines();
  ~NodeMaterialDefines() override;

  void setValue(const std::string& name, bool value, bool markAsUnprocessedIfDirty = false);

}; // end of struct NodeMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_NODE_MATERIAL_DEFINES_H
