#ifndef BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT WaterMaterialDefines : public MaterialDefines {

  WaterMaterialDefines();
  ~WaterMaterialDefines() override; // = default

}; // end of struct WaterMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_DEFINES_H
