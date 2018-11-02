#ifndef BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

/**
 * @brief Manages the defines for the PBR Material.
 */
struct BABYLON_SHARED_EXPORT PBRMaterialDefines
    : public MaterialDefines,
      public IImageProcessingConfigurationDefines {

  /**
   * @brief Initializes the PBR Material defines.
   */
  PBRMaterialDefines();
  ~PBRMaterialDefines() override;

  /**
   * @brief Resets the PBR Material defines.
   */
  void reset() override;

  /**
   * @brief Converts the material define values to a string.
   * @returns - String of material define information.
   */
  virtual std::string toString() const override;

}; // end of struct PBRMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H
