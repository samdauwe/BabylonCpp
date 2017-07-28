#ifndef BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PBRMaterialDefines
  : public MaterialDefines,
    public IImageProcessingConfigurationDefines {

  PBRMaterialDefines();
  ~PBRMaterialDefines();

  void reset();

}; // end of struct PBRMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H