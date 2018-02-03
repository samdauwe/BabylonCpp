#include <babylon/materials/pbr/pbr_base_simple_material.h>

#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {
namespace Internals {

PBRBaseSimpleMaterial::PBRBaseSimpleMaterial(const string_t& iName,
                                             Scene* scene)
    : PBRBaseMaterial{iName, scene}
    , maxSimultaneousLights{4}
    , disableLighting{false}
    , environmentTexture{nullptr}
    , invertNormalMapX{false}
    , invertNormalMapY{false}
    , normalTexture{nullptr}
    , emissiveColor{Color3(0, 0, 0)}
    , emissiveTexture{nullptr}
    , occlusionStrength{1.f}
    , occlusionTexture{nullptr}
    , alphaCutOff{0.f}
    , lightmapTexture{nullptr}
    , useLightmapAsShadowmap{false}
{
  _useAlphaFromAlbedoTexture = true;
  _useAmbientInGrayScale     = true;
}

PBRBaseSimpleMaterial::~PBRBaseSimpleMaterial()
{
}

const char* PBRBaseSimpleMaterial::getClassName() const
{
  return "PBRBaseSimpleMaterial";
}

bool PBRBaseSimpleMaterial::doubleSided() const
{
  return _twoSidedLighting;
}

void PBRBaseSimpleMaterial::setDoubleSided(bool value)
{
  if (_twoSidedLighting == value) {
    return;
  }
  _twoSidedLighting = value;
  setBackFaceCulling(!value);
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRBaseSimpleMaterial::_shouldUseAlphaFromAlbedoTexture() const
{
  return _albedoTexture && _albedoTexture->hasAlpha()
         && _transparencyMode != PBRMaterial::PBRMATERIAL_OPAQUE;
}

vector_t<BaseTexture*> PBRBaseSimpleMaterial::getActiveTextures() const
{
  auto activeTextures = PBRBaseMaterial::getActiveTextures();

  if (environmentTexture) {
    activeTextures.emplace_back(environmentTexture);
  }

  if (normalTexture) {
    activeTextures.emplace_back(normalTexture);
  }

  if (emissiveTexture) {
    activeTextures.emplace_back(emissiveTexture);
  }

  if (occlusionTexture) {
    activeTextures.emplace_back(occlusionTexture);
  }

  if (lightmapTexture) {
    activeTextures.emplace_back(lightmapTexture);
  }

  return activeTextures;
}

bool PBRBaseSimpleMaterial::hasTexture(BaseTexture* texture) const
{
  if (PBRBaseMaterial::hasTexture(texture)) {
    return true;
  }

  if (lightmapTexture == texture) {
    return true;
  }

  return false;
}

} // end of namespace Internals
} // end of namespace BABYLON
