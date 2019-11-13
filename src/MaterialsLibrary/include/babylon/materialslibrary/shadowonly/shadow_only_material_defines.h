#ifndef BABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT ShadowOnlyMaterialDefines
    : public MaterialDefines {

  ShadowOnlyMaterialDefines();
  ~ShadowOnlyMaterialDefines(); // = default

}; // end of struct ShadowOnlyMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_LIBRARY_SHADOW_ONLY_SHADOW_ONLY_MATERIAL_DEFINES_H
