#include <babylon/materials/multi_material.h>

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
Material* MultiMaterial::getSubMaterial(unsigned int index)
{
  if (index >= subMaterials.size()) {
    return getScene()->defaultMaterial();
  }

  return subMaterials[index];
}

// Methods
bool MultiMaterial::isReady(AbstractMesh* mesh, bool /*useInstances*/)
{
  for (auto& subMaterial : subMaterials) {
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

  for (auto& subMaterial : subMaterials) {
    Material* newSubMaterial = nullptr;
    if (cloneChildren) {
      newSubMaterial = subMaterial->clone(name + "-" + subMaterial->name);
    }
    else {
      newSubMaterial = subMaterial;
    }
    newMultiMaterial->subMaterials.emplace_back(newSubMaterial);
  }

  return newMultiMaterial;
}

Json::object MultiMaterial::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
