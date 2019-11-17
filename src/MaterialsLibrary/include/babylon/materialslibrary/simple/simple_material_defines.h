#ifndef BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT SimpleMaterialDefines : public MaterialDefines {

  SimpleMaterialDefines();
  ~SimpleMaterialDefines() override; // = default

}; // end of struct SimpleMaterialDefines

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SIMPLE_SIMPLE_MATERIAL_DEFINES_H
