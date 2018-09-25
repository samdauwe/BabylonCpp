#ifndef BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_SHADER_STRUCTURE_H
#define BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_SHADER_STRUCTURE_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT CustomShaderStructure {

  CustomShaderStructure();
  ~CustomShaderStructure();

  std::string FragmentStore;
  std::string VertexStore;

}; // end of class CustomShaderStructure

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_SHADER_STRUCTURE_H
