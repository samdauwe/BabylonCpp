#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>

#include <babylon/core/json.h>

namespace BABYLON {

PBRSpecularGlossinessMaterial::PBRSpecularGlossinessMaterial(
  const string_t& iName, Scene* scene)
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

const string_t PBRSpecularGlossinessMaterial::getClassName() const
{
  return "PBRSpecularGlossinessMaterial";
}

vector_t<BaseTexture*> PBRSpecularGlossinessMaterial::getActiveTextures() const
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

bool PBRSpecularGlossinessMaterial::hasTexture(BaseTexture* texture) const
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

PBRSpecularGlossinessMaterial*
PBRSpecularGlossinessMaterial::clone(const string_t& /*name*/,
                                     bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object PBRSpecularGlossinessMaterial::serialize() const
{
  return Json::object();
}

PBRSpecularGlossinessMaterial* PBRSpecularGlossinessMaterial::Parse(
  const Json::value& /*source*/, Scene* /*scene*/, const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
