#include <babylon/materials/multi_material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {

MultiMaterial::MultiMaterial(const std::string iName, Scene* scene)
    : Material{iName, scene, true}
    , subMaterials{this, &MultiMaterial::get_subMaterials,
                   &MultiMaterial::set_subMaterials}
{
  // multimaterial is considered like a push material
  _storeEffectOnSubMeshes = true;
}

MultiMaterial::~MultiMaterial()
{
}

std::vector<MaterialPtr>& MultiMaterial::getChildren()
{
  return subMaterials;
}

Type MultiMaterial::type() const
{
  return Type::MULTIMATERIAL;
}

// Properties
std::vector<MaterialPtr>& MultiMaterial::get_subMaterials()
{
  return _subMaterials;
}

void MultiMaterial::set_subMaterials(const std::vector<MaterialPtr>& value)
{
  _subMaterials = value;
  _hookArray(value);
}

void MultiMaterial::_hookArray(const std::vector<MaterialPtr>& /*array*/)
{
}

MaterialPtr& MultiMaterial::getSubMaterial(unsigned int index)
{
  if (index >= _subMaterials.size()) {
    return getScene()->defaultMaterial();
  }

  return _subMaterials[index];
}

std::vector<BaseTexturePtr> MultiMaterial::getActiveTextures() const
{
  auto activeTextures = Material::getActiveTextures();
  for (auto& subMaterial : _subMaterials) {
    stl_util::concat(activeTextures, subMaterial->getActiveTextures());
  }
  return activeTextures;
}

const std::string MultiMaterial::getClassName() const
{
  return "MultiMaterial";
}

bool MultiMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                      bool useInstances)
{
  for (auto& subMaterial : _subMaterials) {
    if (subMaterial) {
      if (subMaterial->_storeEffectOnSubMeshes) {
        if (!subMaterial->isReadyForSubMesh(mesh, subMesh, useInstances)) {
          return false;
        }
        continue;
      }

      if (!subMaterial->isReady(mesh)) {
        return false;
      }
    }
  }

  return true;
}

MaterialPtr MultiMaterial::clone(const std::string& iName,
                                 bool cloneChildren) const
{
  auto newMultiMaterial = MultiMaterial::New(iName, getScene());

  for (auto& subMaterial : _subMaterials) {
    MaterialPtr newSubMaterial = nullptr;
    if (cloneChildren) {
      newSubMaterial = subMaterial->clone(name + "-" + subMaterial->name);
    }
    else {
      newSubMaterial = subMaterial;
    }
    newMultiMaterial->_subMaterials.emplace_back(newSubMaterial);
  }

  return newMultiMaterial;
}

json MultiMaterial::serialize() const
{
  return nullptr;
}

void MultiMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                            bool forceDisposeChildren)
{
  auto scene = getScene();
  if (!scene) {
    return;
  }

  if (forceDisposeChildren) {
    for (auto& subMaterial : subMaterials()) {
      if (subMaterial) {
        subMaterial->dispose(forceDisposeEffect, forceDisposeTextures);
      }
    }
  }

  // Remove from scene
  scene->multiMaterials.erase(
    std::remove_if(scene->multiMaterials.begin(), scene->multiMaterials.end(),
                   [this](const MultiMaterialPtr& multiMaterial) {
                     return multiMaterial.get() == this;
                   }),
    scene->multiMaterials.end());

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

MultiMaterialPtr
MultiMaterial::ParseMultiMaterial(const json& parsedMultiMaterial, Scene* scene)
{
  auto multiMaterial = MultiMaterial::New(
    json_util::get_string(parsedMultiMaterial, "name"), scene);

  multiMaterial->id = json_util::get_string(parsedMultiMaterial, "id");

  for (const auto& subMatId :
       json_util::get_array<json>(parsedMultiMaterial, "materials")) {

    if (subMatId.is_string()) {
      // If the same multimaterial is loaded twice, the 2nd multimaterial needs
      // to reference the latest material by that id which is why this lookup
      // should use getLastMaterialByID instead of getMaterialByID
      multiMaterial->subMaterials().emplace_back(
        scene->getLastMaterialByID(subMatId.get<std::string>()));
    }
    else {
      multiMaterial->subMaterials().emplace_back(nullptr);
    }
  }

  return multiMaterial;
}

} // end of namespace BABYLON
