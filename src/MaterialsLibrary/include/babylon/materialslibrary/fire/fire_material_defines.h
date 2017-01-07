#ifndef BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT FireMaterialDefines : public MaterialDefines {

  static constexpr unsigned int DIFFUSE     = 0;
  static constexpr unsigned int CLIPPLANE   = 1;
  static constexpr unsigned int ALPHATEST   = 2;
  static constexpr unsigned int POINTSIZE   = 3;
  static constexpr unsigned int FOG         = 4;
  static constexpr unsigned int UV1         = 5;
  static constexpr unsigned int VERTEXCOLOR = 6;
  static constexpr unsigned int VERTEXALPHA = 7;
  static constexpr unsigned int INSTANCES   = 8;

  FireMaterialDefines();
  ~FireMaterialDefines();

}; // end of struct FireMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_FIRE_FIRE_MATERIAL_DEFINES_H
