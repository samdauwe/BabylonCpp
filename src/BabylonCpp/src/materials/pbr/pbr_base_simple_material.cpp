#include <babylon/materials/pbr/pbr_base_simple_material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

PBRBaseSimpleMaterial::PBRBaseSimpleMaterial(const std::string& iName, Scene* scene)
    : PBRBaseMaterial{iName, scene}
    , maxSimultaneousLights{this, &PBRBaseSimpleMaterial::get_maxSimultaneousLights,
                            &PBRBaseSimpleMaterial::set_maxSimultaneousLights}
    , disableLighting{this, &PBRBaseSimpleMaterial::get_disableLighting,
                      &PBRBaseSimpleMaterial::set_disableLighting}
    , environmentTexture{this, &PBRBaseSimpleMaterial::get_environmentTexture,
                         &PBRBaseSimpleMaterial::set_environmentTexture}
    , invertNormalMapX{this, &PBRBaseSimpleMaterial::get_invertNormalMapX,
                       &PBRBaseSimpleMaterial::set_invertNormalMapX}
    , invertNormalMapY{this, &PBRBaseSimpleMaterial::get_invertNormalMapY,
                       &PBRBaseSimpleMaterial::set_invertNormalMapY}
    , normalTexture{this, &PBRBaseSimpleMaterial::get_normalTexture,
                    &PBRBaseSimpleMaterial::set_normalTexture}
    , emissiveColor{this, &PBRBaseSimpleMaterial::get_emissiveColor,
                    &PBRBaseSimpleMaterial::set_emissiveColor}
    , emissiveTexture{this, &PBRBaseSimpleMaterial::get_emissiveTexture,
                      &PBRBaseSimpleMaterial::set_emissiveTexture}
    , occlusionStrength{this, &PBRBaseSimpleMaterial::get_occlusionStrength,
                        &PBRBaseSimpleMaterial::set_occlusionStrength}
    , occlusionTexture{this, &PBRBaseSimpleMaterial::get_occlusionTexture,
                       &PBRBaseSimpleMaterial::set_occlusionTexture}
    , alphaCutOff{this, &PBRBaseSimpleMaterial::get_alphaCutOff,
                  &PBRBaseSimpleMaterial::set_alphaCutOff}
    , doubleSided{this, &PBRBaseSimpleMaterial::get_doubleSided,
                  &PBRBaseSimpleMaterial::set_doubleSided}
    , lightmapTexture{this, &PBRBaseSimpleMaterial::get_lightmapTexture,
                      &PBRBaseSimpleMaterial::set_lightmapTexture}
    , useLightmapAsShadowmap{this, &PBRBaseSimpleMaterial::get_useLightmapAsShadowmap,
                             &PBRBaseSimpleMaterial::set_useLightmapAsShadowmap}
{
  // Properties
  {
    maxSimultaneousLights  = 4;
    disableLighting        = false;
    environmentTexture     = nullptr;
    invertNormalMapX       = false;
    invertNormalMapY       = false;
    normalTexture          = nullptr;
    emissiveColor          = Color3(0.f, 0.f, 0.f);
    emissiveTexture        = nullptr;
    occlusionStrength      = 1.f;
    occlusionTexture       = nullptr;
    alphaCutOff            = 0.f;
    lightmapTexture        = nullptr;
    useLightmapAsShadowmap = false;
  }
  _useAlphaFromAlbedoTexture = true;
  _useAmbientInGrayScale     = true;
}

PBRBaseSimpleMaterial::~PBRBaseSimpleMaterial() = default;

std::string PBRBaseSimpleMaterial::getClassName() const
{
  return "PBRBaseSimpleMaterial";
}

unsigned int PBRBaseSimpleMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void PBRBaseSimpleMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights == value) {
    return;
  }

  _maxSimultaneousLights = value;
  _markAllSubMeshesAsLightsDirty();
}

bool PBRBaseSimpleMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void PBRBaseSimpleMaterial::set_disableLighting(bool value)
{
  if (_disableLighting == value) {
    return;
  }

  _disableLighting = value;
  _markAllSubMeshesAsLightsDirty();
}

BaseTexturePtr& PBRBaseSimpleMaterial::get_environmentTexture()
{
  return _reflectionTexture;
}

void PBRBaseSimpleMaterial::set_environmentTexture(const BaseTexturePtr& value)
{
  if (_reflectionTexture == value) {
    return;
  }

  _reflectionTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRBaseSimpleMaterial::get_invertNormalMapX() const
{
  return _invertNormalMapX;
}

void PBRBaseSimpleMaterial::set_invertNormalMapX(bool value)
{
  if (_invertNormalMapX == value) {
    return;
  }

  _invertNormalMapX = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRBaseSimpleMaterial::get_invertNormalMapY() const
{
  return _invertNormalMapY;
}

void PBRBaseSimpleMaterial::set_invertNormalMapY(bool value)
{
  if (_invertNormalMapY == value) {
    return;
  }

  _invertNormalMapY = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRBaseSimpleMaterial::get_normalTexture()
{
  return _bumpTexture;
}

void PBRBaseSimpleMaterial::set_normalTexture(const BaseTexturePtr& value)
{
  if (_bumpTexture == value) {
    return;
  }

  _bumpTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

Color3& PBRBaseSimpleMaterial::get_emissiveColor()
{
  return _emissiveColor;
}

void PBRBaseSimpleMaterial::set_emissiveColor(const Color3& value)
{
  if (_emissiveColor == value) {
    return;
  }

  _emissiveColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRBaseSimpleMaterial::get_emissiveTexture()
{
  return _emissiveTexture;
}

void PBRBaseSimpleMaterial::set_emissiveTexture(const BaseTexturePtr& value)
{
  if (_emissiveTexture == value) {
    return;
  }

  _emissiveTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRBaseSimpleMaterial::get_occlusionStrength() const
{
  return _ambientTextureStrength;
}

void PBRBaseSimpleMaterial::set_occlusionStrength(float value)
{
  if (stl_util::almost_equal(_ambientTextureStrength, value)) {
    return;
  }

  _ambientTextureStrength = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRBaseSimpleMaterial::get_occlusionTexture()
{
  return _ambientTexture;
}

void PBRBaseSimpleMaterial::set_occlusionTexture(const BaseTexturePtr& value)
{
  if (_ambientTexture == value) {
    return;
  }

  _ambientTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRBaseSimpleMaterial::get_alphaCutOff() const
{
  return _alphaCutOff;
}

void PBRBaseSimpleMaterial::set_alphaCutOff(float value)
{
  if (stl_util::almost_equal(_alphaCutOff, value)) {
    return;
  }

  _alphaCutOff = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRBaseSimpleMaterial::get_doubleSided() const
{
  return _twoSidedLighting;
}

void PBRBaseSimpleMaterial::set_doubleSided(bool value)
{
  if (_twoSidedLighting == value) {
    return;
  }
  _twoSidedLighting = value;
  backFaceCulling   = !value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRBaseSimpleMaterial::get_lightmapTexture()
{
  return _lightmapTexture;
}

void PBRBaseSimpleMaterial::set_lightmapTexture(const BaseTexturePtr& value)
{
  if (_lightmapTexture == value) {
    return;
  }

  _lightmapTexture = value;
  _markAllSubMeshesAsLightsDirty();
}

bool PBRBaseSimpleMaterial::get_useLightmapAsShadowmap() const
{
  return _useLightmapAsShadowmap;
}

void PBRBaseSimpleMaterial::set_useLightmapAsShadowmap(bool value)
{
  if (_useLightmapAsShadowmap == value) {
    return;
  }

  _useLightmapAsShadowmap = value;
  _markAllSubMeshesAsLightsDirty();
}

bool PBRBaseSimpleMaterial::_shouldUseAlphaFromAlbedoTexture() const
{
  return _albedoTexture && _albedoTexture->hasAlpha()
         && _transparencyMode != PBRMaterial::PBRMATERIAL_OPAQUE;
}

} // end of namespace BABYLON
