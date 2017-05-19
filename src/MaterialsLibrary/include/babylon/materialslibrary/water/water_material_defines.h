#ifndef BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT WaterMaterialDefines : public MaterialDefines {

  static constexpr unsigned int BUMP                  = 0;
  static constexpr unsigned int REFLECTION            = 1;
  static constexpr unsigned int CLIPPLANE             = 2;
  static constexpr unsigned int ALPHATEST             = 3;
  static constexpr unsigned int POINTSIZE             = 4;
  static constexpr unsigned int FOG                   = 5;
  static constexpr unsigned int NORMAL                = 6;
  static constexpr unsigned int UV1                   = 7;
  static constexpr unsigned int UV2                   = 8;
  static constexpr unsigned int VERTEXCOLOR           = 9;
  static constexpr unsigned int VERTEXALPHA           = 10;
  static constexpr unsigned int INSTANCES             = 11;
  static constexpr unsigned int SPECULARTERM          = 12;
  static constexpr unsigned int LOGARITHMICDEPTH      = 13;
  static constexpr unsigned int FRESNELSEPARATE       = 14;
  static constexpr unsigned int BUMPSUPERIMPOSE       = 15;
  static constexpr unsigned int BUMPAFFECTSREFLECTION = 16;
  static constexpr unsigned int SHADOWS               = 17;
  static constexpr unsigned int SHADOWFULLFLOAT       = 18;

  WaterMaterialDefines();
  ~WaterMaterialDefines();

}; // end of struct WaterMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_WATER_WATER_MATERIAL_DEFINES_H
