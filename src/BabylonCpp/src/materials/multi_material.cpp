#include <babylon/materials/multi_material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {

MultiMaterial::MultiMaterial(const std::string iName, Scene* scene)
    : Material{iName, scene, true}
{
}

MultiMaterial::~MultiMaterial()
{
}

const char* MultiMaterial::getClassName() const
{
  return "MultiMaterial";
}

IReflect::Type MultiMaterial::type() const
{
  return IReflect::Type::MULTIMATERIAL;
}

// Properties
std::vector<Material*>& MultiMaterial::subMaterials()
{
  return _subMaterials;
}

void MultiMaterial::setSubMaterials(const std::vector<Material*>& value)
{
  _subMaterials = value;
  _hookArray(value);
}

Material* MultiMaterial::getSubMaterial(unsigned int index)
{
  if (index >= _subMaterials.size()) {
    return getScene()->defaultMaterial();
  }

  return _subMaterials[index];
}

std::vector<BaseTexture*> MultiMaterial::getActiveTextures() const
{
  auto activeTextures = Material::getActiveTextures();
  for (auto& subMaterial : _subMaterials) {
    stl_util::concat(activeTextures, subMaterial->getActiveTextures());
  }
  return activeTextures;
}

// Methods
void MultiMaterial::_hookArray(const std::vector<Material*>& /*array*/)
{
}

bool MultiMaterial::isReady(AbstractMesh* mesh, bool /*useInstances*/)
{
  for (auto& subMaterial : _subMaterials) {
    if (subMaterial) {
      if (!subMaterial->isReady(mesh)) {
        return false;
      }
    }
  }

  return true;
}

Material* MultiMaterial::clone(const std::string& iName,
                               bool cloneChildren) const
{
  auto newMultiMaterial = MultiMaterial::New(iName, getScene());

  for (auto& subMaterial : _subMaterials) {
    Material* newSubMaterial = nullptr;
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

Json::object MultiMaterial::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
