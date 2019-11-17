#ifndef BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT StandardMaterialDefines : public MaterialDefines,
                                                       public IImageProcessingConfigurationDefines {

  StandardMaterialDefines();
  ~StandardMaterialDefines() override; // = default

  void setReflectionMode(const std::string& modeToEnable);

  /**
   * @brief Converts the material define values to a string.
   * @returns - String of material define information.
   */
  [[nodiscard]] std::string toString() const override;

}; // end of struct StandardMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H
