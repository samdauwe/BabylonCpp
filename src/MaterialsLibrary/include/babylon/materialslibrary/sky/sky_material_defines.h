#ifndef BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT SkyMaterialDefines : public MaterialDefines {

  static constexpr unsigned int CLIPPLANE        = 0;
  static constexpr unsigned int POINTSIZE        = 1;
  static constexpr unsigned int FOG              = 2;
  static constexpr unsigned int VERTEXCOLOR      = 3;
  static constexpr unsigned int VERTEXALPHA      = 4;
  static constexpr unsigned int LOGARITHMICDEPTH = 5;
  static constexpr unsigned int NORMAL           = 6;
  static constexpr unsigned int UV1              = 7;
  static constexpr unsigned int UV2              = 8;

  SkyMaterialDefines();
  ~SkyMaterialDefines();

}; // end of struct SkyMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SKY_SKY_MATERIAL_DEFINES_H
