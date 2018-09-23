#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>

#include <babylon/core/json.h>

namespace BABYLON {

PBRSpecularGlossinessMaterial::PBRSpecularGlossinessMaterial(
  const std::string& iName, Scene* scene)
    : PBRBaseSimpleMaterial{iName, scene}
    , diffuseTexture{nullptr}
    , glossiness{0.f}
    , specularGlossinessTexture{nullptr}
{
  _useMicroSurfaceFromReflectivityMapAlpha = true;
}

PBRSpecularGlossinessMaterial::~PBRSpecularGlossinessMaterial()
{
}

const std::string PBRSpecularGlossinessMaterial::getClassName() const
{
  return "PBRSpecularGlossinessMaterial";
}

std::vector<BaseTexturePtr>
PBRSpecularGlossinessMaterial::getActiveTextures() const
{
  auto activeTextures = PBRBaseSimpleMaterial::getActiveTextures();

  if (diffuseTexture) {
    activeTextures.emplace_back(diffuseTexture);
  }

  if (specularGlossinessTexture) {
    activeTextures.emplace_back(specularGlossinessTexture);
  }

  return activeTextures;
}

bool PBRSpecularGlossinessMaterial::hasTexture(
  const BaseTexturePtr& texture) const
{
  if (PBRBaseSimpleMaterial::hasTexture(texture)) {
    return true;
  }

  if (diffuseTexture == texture) {
    return true;
  }

  if (specularGlossinessTexture == texture) {
    return true;
  }

  return false;
}

MaterialPtr PBRSpecularGlossinessMaterial::clone(const std::string& /*name*/,
                                                 bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object PBRSpecularGlossinessMaterial::serialize() const
{
  return Json::object();
}

PBRSpecularGlossinessMaterial*
PBRSpecularGlossinessMaterial::Parse(const Json::value& /*source*/,
                                     Scene* /*scene*/,
                                     const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
