#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

PBRSpecularGlossinessMaterial::PBRSpecularGlossinessMaterial(
  const std::string& iName, Scene* scene)
    : PBRBaseSimpleMaterial{iName, scene}
    , diffuseColor{this, &PBRSpecularGlossinessMaterial::get_diffuseColor,
                   &PBRSpecularGlossinessMaterial::set_diffuseColor}
    , diffuseTexture{this, &PBRSpecularGlossinessMaterial::get_diffuseTexture,
                     &PBRSpecularGlossinessMaterial::set_diffuseTexture}
    , specularColor{this, &PBRSpecularGlossinessMaterial::get_specularColor,
                    &PBRSpecularGlossinessMaterial::set_specularColor}
    , glossiness{this, &PBRSpecularGlossinessMaterial::get_glossiness,
                 &PBRSpecularGlossinessMaterial::set_glossiness}
    , specularGlossinessTexture{
        this, &PBRSpecularGlossinessMaterial::get_specularGlossinessTexture,
        &PBRSpecularGlossinessMaterial::set_specularGlossinessTexture}
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

Color3& PBRSpecularGlossinessMaterial::get_diffuseColor()
{
  return _albedoColor;
}

void PBRSpecularGlossinessMaterial::set_diffuseColor(const Color3& value)
{
  if (_albedoColor == value) {
    return;
  }

  _albedoColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRSpecularGlossinessMaterial::get_diffuseTexture()
{
  return _albedoTexture;
}

void PBRSpecularGlossinessMaterial::set_diffuseTexture(
  const BaseTexturePtr& value)
{
  if (_albedoTexture == value) {
    return;
  }

  _albedoTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

Color3& PBRSpecularGlossinessMaterial::get_specularColor()
{
  return _reflectivityColor;
}

void PBRSpecularGlossinessMaterial::set_specularColor(const Color3& value)
{
  if (_reflectivityColor == value) {
    return;
  }

  _reflectivityColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRSpecularGlossinessMaterial::get_glossiness() const
{
  return _microSurface;
}

void PBRSpecularGlossinessMaterial::set_glossiness(float value)
{
  if (stl_util::almost_equal(_microSurface, value)) {
    return;
  }

  _microSurface = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRSpecularGlossinessMaterial::get_specularGlossinessTexture()
{
  return _reflectivityTexture;
}

void PBRSpecularGlossinessMaterial::set_specularGlossinessTexture(
  const BaseTexturePtr& value)
{
  if (_reflectivityTexture == value) {
    return;
  }

  _reflectivityTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

MaterialPtr PBRSpecularGlossinessMaterial::clone(const std::string& /*name*/,
                                                 bool /*cloneChildren*/) const
{
  return nullptr;
}

json PBRSpecularGlossinessMaterial::serialize() const
{
  return nullptr;
}

PBRSpecularGlossinessMaterial*
PBRSpecularGlossinessMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                     const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
