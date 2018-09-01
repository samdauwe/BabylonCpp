#ifndef BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

/**
 * @brief Background material defines definition.
 * Hidden Mainly internal Use
 */
struct BABYLON_SHARED_EXPORT BackgroundMaterialDefines
    : public MaterialDefines,
      public IImageProcessingConfigurationDefines {

  /**
   * @brief Constructor of the defines.
   */
  BackgroundMaterialDefines();
  ~BackgroundMaterialDefines() override;

  void reset() override;

}; // end of struct BackgroundMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_DEFINES_H
