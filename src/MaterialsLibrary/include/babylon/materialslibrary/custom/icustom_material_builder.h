#ifndef BABYLON_MATERIALS_LIBRARY_CUSTOM_ICUSTOM_MATERIAL_BUILDER_H
#define BABYLON_MATERIALS_LIBRARY_CUSTOM_ICUSTOM_MATERIAL_BUILDER_H

#include <babylon/babylon_global.h>
#include <babylon/materialslibrary/custom/custom_shader_helper.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT ICustomMaterialBuilder {
  virtual std::string
  build(CustomShaderHelper& builder, const std::string& name,
        const std::string& mainPart, const std::string& diffusePart,
        const std::string& vertexPositionPart)
    = 0;
}; // end of class ICustomMaterialBuilder

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CUSTOM_ICUSTOM_MATERIAL_BUILDER_H
