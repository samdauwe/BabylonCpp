#ifndef BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT LavaMaterialDefines : public MaterialDefines {

  static constexpr unsigned int DIFFUSE          = 0;
  static constexpr unsigned int CLIPPLANE        = 1;
  static constexpr unsigned int ALPHATEST        = 2;
  static constexpr unsigned int POINTSIZE        = 3;
  static constexpr unsigned int FOG              = 4;
  static constexpr unsigned int LIGHT0           = 5;
  static constexpr unsigned int LIGHT1           = 6;
  static constexpr unsigned int LIGHT2           = 7;
  static constexpr unsigned int LIGHT3           = 8;
  static constexpr unsigned int SPOTLIGHT0       = 9;
  static constexpr unsigned int SPOTLIGHT1       = 10;
  static constexpr unsigned int SPOTLIGHT2       = 11;
  static constexpr unsigned int SPOTLIGHT3       = 12;
  static constexpr unsigned int HEMILIGHT0       = 13;
  static constexpr unsigned int HEMILIGHT1       = 14;
  static constexpr unsigned int HEMILIGHT2       = 15;
  static constexpr unsigned int HEMILIGHT3       = 16;
  static constexpr unsigned int DIRLIGHT0        = 17;
  static constexpr unsigned int DIRLIGHT1        = 18;
  static constexpr unsigned int DIRLIGHT2        = 19;
  static constexpr unsigned int DIRLIGHT3        = 20;
  static constexpr unsigned int POINTLIGHT0      = 21;
  static constexpr unsigned int POINTLIGHT1      = 22;
  static constexpr unsigned int POINTLIGHT2      = 23;
  static constexpr unsigned int POINTLIGHT3      = 24;
  static constexpr unsigned int SHADOW0          = 25;
  static constexpr unsigned int SHADOW1          = 26;
  static constexpr unsigned int SHADOW2          = 27;
  static constexpr unsigned int SHADOW3          = 28;
  static constexpr unsigned int SHADOWS          = 29;
  static constexpr unsigned int SHADOWESM0       = 30;
  static constexpr unsigned int SHADOWESM1       = 31;
  static constexpr unsigned int SHADOWESM2       = 32;
  static constexpr unsigned int SHADOWESM3       = 33;
  static constexpr unsigned int SHADOWPCF0       = 34;
  static constexpr unsigned int SHADOWPCF1       = 35;
  static constexpr unsigned int SHADOWPCF2       = 36;
  static constexpr unsigned int SHADOWPCF3       = 37;
  static constexpr unsigned int NORMAL           = 38;
  static constexpr unsigned int UV1              = 39;
  static constexpr unsigned int UV2              = 40;
  static constexpr unsigned int VERTEXCOLOR      = 41;
  static constexpr unsigned int VERTEXALPHA      = 42;
  static constexpr unsigned int INSTANCES        = 43;
  static constexpr unsigned int SPECULARTERM     = 44;
  static constexpr unsigned int LOGARITHMICDEPTH = 45;
  static constexpr unsigned int SHADOWFULLFLOAT  = 46;

  LavaMaterialDefines();
  ~LavaMaterialDefines();

}; // end of struct LavaMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_LAVA_LAVA_MATERIAL_DEFINES_H
