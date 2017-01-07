#ifndef BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT TriPlanarMaterialDefines : public MaterialDefines {

  static constexpr unsigned int DIFFUSEX     = 0;
  static constexpr unsigned int DIFFUSEY     = 1;
  static constexpr unsigned int DIFFUSEZ     = 2;
  static constexpr unsigned int BUMPX        = 3;
  static constexpr unsigned int BUMPY        = 4;
  static constexpr unsigned int BUMPZ        = 5;
  static constexpr unsigned int CLIPPLANE    = 6;
  static constexpr unsigned int ALPHATEST    = 7;
  static constexpr unsigned int POINTSIZE    = 8;
  static constexpr unsigned int FOG          = 9;
  static constexpr unsigned int SPECULARTERM = 10;
  static constexpr unsigned int NORMAL       = 11;
  static constexpr unsigned int VERTEXCOLOR  = 12;
  static constexpr unsigned int VERTEXALPHA  = 13;
  static constexpr unsigned int INSTANCES    = 14;
  static constexpr unsigned int SHADOWS      = 15;

  TriPlanarMaterialDefines();
  ~TriPlanarMaterialDefines();

}; // end of struct TriPlanarMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_LIBRARY_TRI_PLANAR_TRI_PLANAR_MATERIAL_DEFINES_H
