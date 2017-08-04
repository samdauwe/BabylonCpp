#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>

namespace BABYLON {

PBRSpecularGlossinessMaterial::PBRSpecularGlossinessMaterial(
  const std::string& iName, Scene* scene)
    : Internals::PBRBaseSimpleMaterial{iName, scene}
    , diffuseTexture{nullptr}
    , glossiness{0.f}
    , specularGlossinessTexture{nullptr}
{
  _useMicroSurfaceFromReflectivityMapAlpha = true;
}

PBRSpecularGlossinessMaterial::~PBRSpecularGlossinessMaterial()
{
}

const char* PBRSpecularGlossinessMaterial::getClassName() const
{
  return "PBRSpecularGlossinessMaterial";
}

std::vector<BaseTexture*>
PBRSpecularGlossinessMaterial::getActiveTextures() const
{
  auto activeTextures = Internals::PBRBaseSimpleMaterial::getActiveTextures();

  if (diffuseTexture) {
    activeTextures.emplace_back(diffuseTexture);
  }

  if (specularGlossinessTexture) {
    activeTextures.emplace_back(specularGlossinessTexture);
  }

  return activeTextures;
}

bool PBRSpecularGlossinessMaterial::hasTexture(BaseTexture* texture) const
{
  if (Internals::PBRBaseSimpleMaterial::hasTexture(texture)) {
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

PBRSpecularGlossinessMaterial*
PBRSpecularGlossinessMaterial::clone(const std::string& /*name*/)
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