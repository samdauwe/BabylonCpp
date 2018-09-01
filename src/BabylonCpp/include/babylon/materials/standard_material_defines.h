#ifndef BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT StandardMaterialDefines
    : public MaterialDefines,
      public IImageProcessingConfigurationDefines {

  StandardMaterialDefines();
  ~StandardMaterialDefines();

  void setReflectionMode(const string_t& modeToEnable);

}; // end of struct StandardMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H
