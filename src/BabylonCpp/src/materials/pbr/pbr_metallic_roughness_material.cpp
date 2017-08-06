#include <babylon/materials/pbr/pbr_metallic_roughness_material.h>

#include <babylon/core/json.h>

namespace BABYLON {

PBRMetallicRoughnessMaterial::PBRMetallicRoughnessMaterial(
  const std::string& iName, Scene* scene)
    : Internals::PBRBaseSimpleMaterial{iName, scene}
    , baseTexture{nullptr}
    , metallic{0.f}
    , roughness{0.f}
    , metallicRoughnessTexture{nullptr}
{
  _useRoughnessFromMetallicTextureAlpha = false;
  _useRoughnessFromMetallicTextureGreen = true;
  _useMetallnessFromMetallicTextureBlue = true;
}

PBRMetallicRoughnessMaterial::~PBRMetallicRoughnessMaterial()
{
}

const char* PBRMetallicRoughnessMaterial::getClassName() const
{
  return "PBRMetallicRoughnessMaterial";
}

std::vector<BaseTexture*>
PBRMetallicRoughnessMaterial::getActiveTextures() const
{
  auto activeTextures = Internals::PBRBaseSimpleMaterial::getActiveTextures();

  if (baseTexture) {
    activeTextures.emplace_back(baseTexture);
  }

  if (metallicRoughnessTexture) {
    activeTextures.emplace_back(metallicRoughnessTexture);
  }

  return activeTextures;
}

bool PBRMetallicRoughnessMaterial::hasTexture(BaseTexture* texture) const
{
  if (Internals::PBRBaseSimpleMaterial::hasTexture(texture)) {
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

PBRMetallicRoughnessMaterial*
PBRMetallicRoughnessMaterial::clone(const std::string& /*name*/,
                                    bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object PBRMetallicRoughnessMaterial::serialize() const
{
  return Json::object();
}

PBRMetallicRoughnessMaterial*
PBRMetallicRoughnessMaterial::Parse(const Json::value& /*source*/,
                                    Scene* /*scene*/,
                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
