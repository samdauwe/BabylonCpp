#ifndef BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT TerrainMaterialDefines : public MaterialDefines {

  static constexpr unsigned int DIFFUSE          = 0;
  static constexpr unsigned int BUMP             = 1;
  static constexpr unsigned int CLIPPLANE        = 2;
  static constexpr unsigned int ALPHATEST        = 3;
  static constexpr unsigned int POINTSIZE        = 4;
  static constexpr unsigned int FOG              = 5;
  static constexpr unsigned int SPECULARTERM     = 6;
  static constexpr unsigned int NORMAL           = 7;
  static constexpr unsigned int UV1              = 8;
  static constexpr unsigned int UV2              = 9;
  static constexpr unsigned int VERTEXCOLOR      = 10;
  static constexpr unsigned int VERTEXALPHA      = 11;
  static constexpr unsigned int INSTANCES        = 12;
  static constexpr unsigned int SHADOWS          = 13;
  static constexpr unsigned int LOGARITHMICDEPTH = 14;
  static constexpr unsigned int SHADOWFULLFLOAT  = 15;

  TerrainMaterialDefines();
  ~TerrainMaterialDefines();

}; // end of struct TerrainMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_TERRAIN_TERRAIN_MATERIAL_DEFINES_H
