#ifndef BABYLON_MATERIALS_IMAGE_PROCESSING_CONFIGURATION_DEFINES_H
#define BABYLON_MATERIALS_IMAGE_PROCESSING_CONFIGURATION_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ImageProcessingConfigurationDefines
    : public MaterialDefines,
      public IImageProcessingConfigurationDefines {

  ImageProcessingConfigurationDefines();
  ~ImageProcessingConfigurationDefines();

}; // end of struct ImageProcessingConfigurationDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IIMAGE_PROCESSING_CONFIGURATION_DEFINES_H
