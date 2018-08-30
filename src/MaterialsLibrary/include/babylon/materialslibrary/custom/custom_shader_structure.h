#ifndef BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_SHADER_STRUCTURE_H
#define BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_SHADER_STRUCTURE_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT CustomShaderStructure {

  CustomShaderStructure();
  ~CustomShaderStructure();

  string_t FragmentStore;
  string_t VertexStore;

}; // end of class CustomShaderStructure

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_SHADER_STRUCTURE_H
