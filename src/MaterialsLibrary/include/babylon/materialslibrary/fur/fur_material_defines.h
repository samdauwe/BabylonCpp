#ifndef BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT FurMaterialDefines : public MaterialDefines {

  static constexpr unsigned int DIFFUSE          = 0;
  static constexpr unsigned int HEIGHTMAP        = 1;
  static constexpr unsigned int CLIPPLANE        = 2;
  static constexpr unsigned int ALPHATEST        = 3;
  static constexpr unsigned int POINTSIZE        = 4;
  static constexpr unsigned int FOG              = 5;
  static constexpr unsigned int NORMAL           = 6;
  static constexpr unsigned int UV1              = 7;
  static constexpr unsigned int UV2              = 8;
  static constexpr unsigned int VERTEXCOLOR      = 9;
  static constexpr unsigned int VERTEXALPHA      = 10;
  static constexpr unsigned int INSTANCES        = 11;
  static constexpr unsigned int HIGHLEVEL        = 12;
  static constexpr unsigned int LOGARITHMICDEPTH = 13;
  static constexpr unsigned int SPECULARTERM     = 14;
  static constexpr unsigned int SHADOWFULLFLOAT  = 15;

  FurMaterialDefines();
  ~FurMaterialDefines();

}; // end of struct FurMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_FUR_FUR_MATERIAL_DEFINES_H
