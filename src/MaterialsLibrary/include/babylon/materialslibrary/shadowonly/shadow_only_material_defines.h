#ifndef BABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT ShadowOnlyMaterialDefines
  : public MaterialDefines {

  static constexpr unsigned int CLIPPLANE        = 0;
  static constexpr unsigned int POINTSIZE        = 1;
  static constexpr unsigned int FOG              = 2;
  static constexpr unsigned int NORMAL           = 3;
  static constexpr unsigned int INSTANCES        = 4;
  static constexpr unsigned int ALPHATEST        = 5;
  static constexpr unsigned int LOGARITHMICDEPTH = 6;
  static constexpr unsigned int SHADOWFULLFLOAT  = 7;
  static constexpr unsigned int SPECULARTERM     = 8;

  ShadowOnlyMaterialDefines();
  ~ShadowOnlyMaterialDefines();

}; // end of struct ShadowOnlyMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_DEFINES_H
