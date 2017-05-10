#ifndef BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct ICustomMaterialBuilder;

class BABYLON_SHARED_EXPORT CustomMaterial : public StandardMaterial {

public:
  CustomMaterial(const std::string& name, ICustomMaterialBuilder* builder,
                 Scene* scene);
  ~CustomMaterial();

public:
  ICustomMaterialBuilder* builder;

private:
  static constexpr const char* _mainPart = "void main(void) {";
  static constexpr const char* _diffusePart
    = "vec3 diffuseColor=vDiffuseColor.rgb;";
  static constexpr const char* _vertexPositionPart
    = "gl_Position=viewProjection*finalWorld*vec4(position,1.0);";

}; // end of class CustomMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H
