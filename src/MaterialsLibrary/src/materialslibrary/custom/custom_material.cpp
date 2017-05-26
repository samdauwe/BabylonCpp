#include <babylon/materialslibrary/custom/custom_material.h>

#include <babylon/materialslibrary/custom/custom_shader_helper.h>
#include <babylon/materialslibrary/custom/icustom_material_builder.h>

namespace BABYLON {
namespace MaterialsLibrary {

CustomMaterial::CustomMaterial(const std::string& iName,
                               ICustomMaterialBuilder* iBuilder, Scene* scene)
    : StandardMaterial{iName, scene}, builder{iBuilder}
{
}

CustomMaterial::~CustomMaterial()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
