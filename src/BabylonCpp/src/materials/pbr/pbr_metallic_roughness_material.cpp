#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>

#include <nlohmann/json.hpp>

namespace BABYLON {

PBRMetallicRoughnessMaterial::PBRMetallicRoughnessMaterial(
  const std::string& iName, Scene* scene)
    : PBRBaseSimpleMaterial{iName, scene}
    , baseTexture{nullptr}
    , metallic{0.f}
    , roughness{0.f}
    , metallicRoughnessTexture{nullptr}
{
  _useRoughnessFromMetallicTextureAlpha = false;
  _useRoughnessFromMetallicTextureGreen = true;
  _useMetallnessFromMetallicTextureBlue = true;

  metallic  = 1.f;
  roughness = 1.f;
}

PBRMetallicRoughnessMaterial::~PBRMetallicRoughnessMaterial()
{
}

const std::string PBRMetallicRoughnessMaterial::getClassName() const
{
  return "PBRMetallicRoughnessMaterial";
}

std::vector<BaseTexturePtr>
PBRMetallicRoughnessMaterial::getActiveTextures() const
{
  auto activeTextures = PBRBaseSimpleMaterial::getActiveTextures();

  if (baseTexture) {
    activeTextures.emplace_back(baseTexture);
  }

  if (metallicRoughnessTexture) {
    activeTextures.emplace_back(metallicRoughnessTexture);
  }

  return activeTextures;
}

bool PBRMetallicRoughnessMaterial::hasTexture(
  const BaseTexturePtr& texture) const
{
  if (PBRBaseSimpleMaterial::hasTexture(texture)) {
    return true;
  }

  if (baseTexture == texture) {
    return true;
  }

  if (metallicRoughnessTexture == texture) {
    return true;
  }

  return false;
}

MaterialPtr PBRMetallicRoughnessMaterial::clone(const std::string& /*name*/,
                                                bool /*cloneChildren*/) const
{
  return nullptr;
}

json PBRMetallicRoughnessMaterial::serialize() const
{
  return nullptr;
}

PBRMetallicRoughnessMaterial*
PBRMetallicRoughnessMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
