#include <babylon/materials/multi_material.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {

MultiMaterial::MultiMaterial(const std::string iName, Scene* scene)
    : Material{iName, scene, true}
    , subMaterials{this, &MultiMaterial::get_subMaterials,
                   &MultiMaterial::set_subMaterials}
{
  // multimaterial is considered like a push material
  storeEffectOnSubMeshes = true;
}

MultiMaterial::~MultiMaterial()
{
}

const std::string MultiMaterial::getClassName() const
{
  return "MultiMaterial";
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

bool MultiMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                      bool useInstances)
{
  for (auto& subMaterial : _subMaterials) {
    if (subMaterial) {
      if (subMaterial->storeEffectOnSubMeshes) {
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

void MultiMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  auto scene = getScene();
  if (!scene) {
    return;
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

} // end of namespace BABYLON
