#include <babylon/materials/pbr/pbr_material.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/pbr/pbr_sub_surface_configuration.h>
#include <babylon/misc/brdf_texture_tools.h>

namespace BABYLON {

PBRMaterial::PBRMaterial(const std::string& iName, Scene* scene)
    : PBRBaseMaterial{iName, scene}
    , directIntensity{this, &PBRMaterial::get_directIntensity, &PBRMaterial::set_directIntensity}
    , emissiveIntensity{this, &PBRMaterial::get_emissiveIntensity,
                        &PBRMaterial::set_emissiveIntensity}
    , environmentIntensity{this, &PBRMaterial::get_environmentIntensity,
                           &PBRMaterial::set_environmentIntensity}
    , specularIntensity{this, &PBRMaterial::get_specularIntensity,
                        &PBRMaterial::set_specularIntensity}
    , disableBumpMap{this, &PBRMaterial::get_disableBumpMap, &PBRMaterial::set_disableBumpMap}
    , albedoTexture{this, &PBRMaterial::get_albedoTexture, &PBRMaterial::set_albedoTexture}
    , ambientTexture{this, &PBRMaterial::get_ambientTexture, &PBRMaterial::set_ambientTexture}
    , ambientTextureStrength{this, &PBRMaterial::get_ambientTextureStrength,
                             &PBRMaterial::set_ambientTextureStrength}
    , ambientTextureImpactOnAnalyticalLights{this,
                                             &PBRMaterial::
                                               get_ambientTextureImpactOnAnalyticalLights,
                                             &PBRMaterial::
                                               set_ambientTextureImpactOnAnalyticalLights}
    , opacityTexture{this, &PBRMaterial::get_opacityTexture, &PBRMaterial::set_opacityTexture}
    , reflectionTexture{this, &PBRMaterial::get_reflectionTexture,
                        &PBRMaterial::set_reflectionTexture}
    , emissiveTexture{this, &PBRMaterial::get_emissiveTexture, &PBRMaterial::set_emissiveTexture}
    , reflectivityTexture{this, &PBRMaterial::get_reflectivityTexture,
                          &PBRMaterial::set_reflectivityTexture}
    , metallicTexture{this, &PBRMaterial::get_metallicTexture, &PBRMaterial::set_metallicTexture}
    , metallic{this, &PBRMaterial::get_metallic, &PBRMaterial::set_metallic}
    , roughness{this, &PBRMaterial::get_roughness, &PBRMaterial::set_roughness}
    , metallicF0Factor{this, &PBRMaterial::get_metallicF0Factor, &PBRMaterial::set_metallicF0Factor}
    , metallicReflectanceColor{this, &PBRMaterial::get_metallicReflectanceColor,
                               &PBRMaterial::set_metallicReflectanceColor}
    , metallicReflectanceTexture{this, &PBRMaterial::get_metallicReflectanceTexture,
                                 &PBRMaterial::set_metallicReflectanceTexture}
    , microSurfaceTexture{this, &PBRMaterial::get_microSurfaceTexture,
                          &PBRMaterial::set_microSurfaceTexture}
    , bumpTexture{this, &PBRMaterial::get_bumpTexture, &PBRMaterial::set_bumpTexture}
    , lightmapTexture{this, &PBRMaterial::get_lightmapTexture, &PBRMaterial::set_lightmapTexture}
    , refractionTexture{this, &PBRMaterial::get_refractionTexture,
                        &PBRMaterial::set_refractionTexture}
    , ambientColor{this, &PBRMaterial::get_ambientColor, &PBRMaterial::set_ambientColor}
    , albedoColor{this, &PBRMaterial::get_albedoColor, &PBRMaterial::set_albedoColor}
    , reflectivityColor{this, &PBRMaterial::get_reflectivityColor,
                        &PBRMaterial::set_reflectivityColor}
    , reflectionColor{this, &PBRMaterial::get_reflectionColor, &PBRMaterial::set_reflectionColor}
    , emissiveColor{this, &PBRMaterial::get_emissiveColor, &PBRMaterial::set_emissiveColor}
    , microSurface{this, &PBRMaterial::get_microSurface, &PBRMaterial::set_microSurface}
    , indexOfRefraction{this, &PBRMaterial::get_indexOfRefraction,
                        &PBRMaterial::set_indexOfRefraction}
    , invertRefractionY{this, &PBRMaterial::get_invertRefractionY,
                        &PBRMaterial::set_invertRefractionY}
    , linkRefractionWithTransparency{this, &PBRMaterial::get_linkRefractionWithTransparency,
                                     &PBRMaterial::set_linkRefractionWithTransparency}
    , useLightmapAsShadowmap{this, &PBRMaterial::get_useLightmapAsShadowmap,
                             &PBRMaterial::set_useLightmapAsShadowmap}
    , useAlphaFromAlbedoTexture{this, &PBRMaterial::get_useAlphaFromAlbedoTexture,
                                &PBRMaterial::set_useAlphaFromAlbedoTexture}
    , forceAlphaTest{this, &PBRMaterial::get_forceAlphaTest, &PBRMaterial::set_forceAlphaTest}
    , alphaCutOff{this, &PBRMaterial::get_alphaCutOff, &PBRMaterial::set_alphaCutOff}
    , useSpecularOverAlpha{this, &PBRMaterial::get_useSpecularOverAlpha,
                           &PBRMaterial::set_useSpecularOverAlpha}
    , useMicroSurfaceFromReflectivityMapAlpha{this,
                                              &PBRMaterial::
                                                get_useMicroSurfaceFromReflectivityMapAlpha,
                                              &PBRMaterial::
                                                set_useMicroSurfaceFromReflectivityMapAlpha}
    , useRoughnessFromMetallicTextureAlpha{this,
                                           &PBRMaterial::get_useRoughnessFromMetallicTextureAlpha,
                                           &PBRMaterial::set_useRoughnessFromMetallicTextureAlpha}
    , useRoughnessFromMetallicTextureGreen{this,
                                           &PBRMaterial::get_useRoughnessFromMetallicTextureGreen,
                                           &PBRMaterial::set_useRoughnessFromMetallicTextureGreen}
    , useMetallnessFromMetallicTextureBlue{this,
                                           &PBRMaterial::get_useMetallnessFromMetallicTextureBlue,
                                           &PBRMaterial::set_useMetallnessFromMetallicTextureBlue}
    , useAmbientOcclusionFromMetallicTextureRed{this,
                                                &PBRMaterial::
                                                  get_useAmbientOcclusionFromMetallicTextureRed,
                                                &PBRMaterial::
                                                  set_useAmbientOcclusionFromMetallicTextureRed}
    , useAmbientInGrayScale{this, &PBRMaterial::get_useAmbientInGrayScale,
                            &PBRMaterial::set_useAmbientInGrayScale}
    , useAutoMicroSurfaceFromReflectivityMap{this,
                                             &PBRMaterial::
                                               get_useAutoMicroSurfaceFromReflectivityMap,
                                             &PBRMaterial::
                                               set_useAutoMicroSurfaceFromReflectivityMap}
    , usePhysicalLightFalloff{this, &PBRMaterial::get_usePhysicalLightFalloff,
                              &PBRMaterial::set_usePhysicalLightFalloff}
    , useGLTFLightFalloff{this, &PBRMaterial::get_useGLTFLightFalloff,
                          &PBRMaterial::set_useGLTFLightFalloff}
    , useRadianceOverAlpha{this, &PBRMaterial::get_useRadianceOverAlpha,
                           &PBRMaterial::set_useRadianceOverAlpha}
    , useObjectSpaceNormalMap{this, &PBRMaterial::get_useObjectSpaceNormalMap,
                              &PBRMaterial::set_useObjectSpaceNormalMap}
    , useParallax{this, &PBRMaterial::get_useParallax, &PBRMaterial::set_useParallax}
    , useParallaxOcclusion{this, &PBRMaterial::get_useParallaxOcclusion,
                           &PBRMaterial::set_useParallaxOcclusion}
    , parallaxScaleBias{this, &PBRMaterial::get_parallaxScaleBias,
                        &PBRMaterial::set_parallaxScaleBias}
    , disableLighting{this, &PBRMaterial::get_disableLighting, &PBRMaterial::set_disableLighting}
    , forceIrradianceInFragment{this, &PBRMaterial::get_forceIrradianceInFragment,
                                &PBRMaterial::set_forceIrradianceInFragment}
    , maxSimultaneousLights{this, &PBRMaterial::get_maxSimultaneousLights,
                            &PBRMaterial::set_maxSimultaneousLights}
    , invertNormalMapX{this, &PBRMaterial::get_invertNormalMapX, &PBRMaterial::set_invertNormalMapX}
    , invertNormalMapY{this, &PBRMaterial::get_invertNormalMapY, &PBRMaterial::set_invertNormalMapY}
    , twoSidedLighting{this, &PBRMaterial::get_twoSidedLighting, &PBRMaterial::set_twoSidedLighting}
    , useAlphaFresnel{this, &PBRMaterial::get_useAlphaFresnel, &PBRMaterial::set_useAlphaFresnel}
    , useLinearAlphaFresnel{this, &PBRMaterial::get_useLinearAlphaFresnel,
                            &PBRMaterial::set_useLinearAlphaFresnel}
    , environmentBRDFTexture{this, &PBRMaterial::get_environmentBRDFTexture,
                             &PBRMaterial::set_environmentBRDFTexture}
    , forceNormalForward{this, &PBRMaterial::get_forceNormalForward,
                         &PBRMaterial::set_forceNormalForward}
    , enableSpecularAntiAliasing{this, &PBRMaterial::get_enableSpecularAntiAliasing,
                                 &PBRMaterial::set_enableSpecularAntiAliasing}
    , useHorizonOcclusion{this, &PBRMaterial::get_useHorizonOcclusion,
                          &PBRMaterial::set_useHorizonOcclusion}
    , useRadianceOcclusion{this, &PBRMaterial::get_useRadianceOcclusion,
                           &PBRMaterial::set_useRadianceOcclusion}
    , unlit{this, &PBRMaterial::get_unlit, &PBRMaterial::set_unlit}
    , imageProcessingConfiguration{this, &PBRMaterial::get_imageProcessingConfiguration,
                                   &PBRMaterial::set_imageProcessingConfiguration}
    , cameraColorCurvesEnabled{this, &PBRMaterial::get_cameraColorCurvesEnabled,
                               &PBRMaterial::set_cameraColorCurvesEnabled}
    , cameraColorGradingEnabled{this, &PBRMaterial::get_cameraColorGradingEnabled,
                                &PBRMaterial::set_cameraColorGradingEnabled}
    , cameraToneMappingEnabled{this, &PBRMaterial::get_cameraToneMappingEnabled,
                               &PBRMaterial::set_cameraToneMappingEnabled}
    , cameraExposure{this, &PBRMaterial::get_cameraExposure, &PBRMaterial::set_cameraExposure}
    , cameraContrast{this, &PBRMaterial::get_cameraContrast, &PBRMaterial::set_cameraContrast}
    , cameraColorGradingTexture{this, &PBRMaterial::get_cameraColorGradingTexture,
                                &PBRMaterial::set_cameraColorGradingTexture}
    , cameraColorCurves{this, &PBRMaterial::get_cameraColorCurves,
                        &PBRMaterial::set_cameraColorCurves}
{
  // Properties
  {
    directIntensity                           = 1.f;
    emissiveIntensity                         = 1.f;
    environmentIntensity                      = 1.f;
    specularIntensity                         = 1.f;
    disableBumpMap                            = false;
    albedoTexture                             = nullptr;
    ambientTexture                            = nullptr;
    ambientTextureStrength                    = 1.f;
    ambientTextureImpactOnAnalyticalLights    = PBRMaterial::DEFAULT_AO_ON_ANALYTICAL_LIGHTS;
    opacityTexture                            = nullptr;
    reflectionTexture                         = nullptr;
    emissiveTexture                           = nullptr;
    reflectivityTexture                       = nullptr;
    metallicTexture                           = nullptr;
    metallic                                  = 0.f;
    roughness                                 = 0.f;
    metallicF0Factor                          = 1.0f;
    metallicReflectanceColor                  = Color3::White();
    metallicReflectanceTexture                = nullptr;
    microSurfaceTexture                       = nullptr;
    bumpTexture                               = nullptr;
    lightmapTexture                           = nullptr;
    refractionTexture                         = nullptr;
    ambientColor                              = Color3(0.f, 0.f, 0.f);
    albedoColor                               = Color3(1.f, 1.f, 1.f);
    reflectivityColor                         = Color3(1.f, 1.f, 1.f);
    reflectionColor                           = Color3(1.f, 1.f, 1.f);
    emissiveColor                             = Color3(0.f, 0.f, 0.f);
    microSurface                              = 1.f;
    indexOfRefraction                         = 0.66f;
    invertRefractionY                         = false;
    linkRefractionWithTransparency            = false;
    useLightmapAsShadowmap                    = false;
    useAlphaFromAlbedoTexture                 = false;
    forceAlphaTest                            = false;
    alphaCutOff                               = 0.4f;
    useSpecularOverAlpha                      = true;
    useMicroSurfaceFromReflectivityMapAlpha   = false;
    useRoughnessFromMetallicTextureAlpha      = true;
    useRoughnessFromMetallicTextureGreen      = false;
    useMetallnessFromMetallicTextureBlue      = false;
    useAmbientOcclusionFromMetallicTextureRed = false;
    useAmbientInGrayScale                     = false;
    useAutoMicroSurfaceFromReflectivityMap    = false;
    useRadianceOverAlpha                      = true;
    useObjectSpaceNormalMap                   = false;
    useParallax                               = false;
    useParallaxOcclusion                      = false;
    parallaxScaleBias                         = 0.05f;
    disableLighting                           = false;
    forceIrradianceInFragment                 = false;
    maxSimultaneousLights                     = 4;
    invertNormalMapX                          = false;
    invertNormalMapY                          = false;
    twoSidedLighting                          = false;
    useAlphaFresnel                           = false;
    useLinearAlphaFresnel                     = false;
    environmentBRDFTexture                    = nullptr;
    forceNormalForward                        = false;
    enableSpecularAntiAliasing                = false;
    useHorizonOcclusion                       = true;
    useRadianceOcclusion                      = true;
    unlit                                     = false;
  }
  _environmentBRDFTexture = BRDFTextureTools::GetEnvironmentBRDFTexture(scene);
}

PBRMaterial::~PBRMaterial() = default;

float PBRMaterial::get_directIntensity() const
{
  return _directIntensity;
}

void PBRMaterial::set_directIntensity(float value)
{
  if (stl_util::almost_equal(_directIntensity, value)) {
    return;
  }

  _directIntensity = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRMaterial::get_emissiveIntensity() const
{
  return _emissiveIntensity;
}

void PBRMaterial::set_emissiveIntensity(float value)
{
  if (stl_util::almost_equal(_emissiveIntensity, value)) {
    return;
  }

  _emissiveIntensity = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRMaterial::get_environmentIntensity() const
{
  return _environmentIntensity;
}

void PBRMaterial::set_environmentIntensity(float value)
{
  if (stl_util::almost_equal(_environmentIntensity, value)) {
    return;
  }

  _environmentIntensity = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRMaterial::get_specularIntensity() const
{
  return _specularIntensity;
}

void PBRMaterial::set_specularIntensity(float value)
{
  if (stl_util::almost_equal(_specularIntensity, value)) {
    return;
  }

  _specularIntensity = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_disableBumpMap() const
{
  return _disableBumpMap;
}

void PBRMaterial::set_disableBumpMap(bool value)
{
  if (_disableBumpMap == value) {
    return;
  }

  _disableBumpMap = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_albedoTexture()
{
  return _albedoTexture;
}

void PBRMaterial::set_albedoTexture(const BaseTexturePtr& value)
{
  if (_albedoTexture == value) {
    return;
  }

  _albedoTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_ambientTexture()
{
  return _ambientTexture;
}

void PBRMaterial::set_ambientTexture(const BaseTexturePtr& value)
{
  if (_ambientTexture == value) {
    return;
  }

  _ambientTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRMaterial::get_ambientTextureStrength() const
{
  return _ambientTextureStrength;
}

void PBRMaterial::set_ambientTextureStrength(float value)
{
  if (stl_util::almost_equal(_ambientTextureStrength, value)) {
    return;
  }

  _ambientTextureStrength = value;
  _markAllSubMeshesAsTexturesDirty();
}

unsigned int PBRMaterial::get_ambientTextureImpactOnAnalyticalLights() const
{
  return _ambientTextureImpactOnAnalyticalLights;
}

void PBRMaterial::set_ambientTextureImpactOnAnalyticalLights(unsigned int value)
{
  if (_ambientTextureImpactOnAnalyticalLights == value) {
    return;
  }

  _ambientTextureImpactOnAnalyticalLights = value;
  _markAllSubMeshesAsTexturesDirty();
}
BaseTexturePtr& PBRMaterial::get_opacityTexture()
{
  return _opacityTexture;
}

void PBRMaterial::set_opacityTexture(const BaseTexturePtr& value)
{
  if (_opacityTexture == value) {
    return;
  }

  _opacityTexture = value;
  _markAllSubMeshesAsTexturesAndMiscDirty();
}

BaseTexturePtr& PBRMaterial::get_reflectionTexture()
{
  return _reflectionTexture;
}

void PBRMaterial::set_reflectionTexture(const BaseTexturePtr& value)
{
  if (_reflectionTexture == value) {
    return;
  }

  _reflectionTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_emissiveTexture()
{
  return _emissiveTexture;
}

void PBRMaterial::set_emissiveTexture(const BaseTexturePtr& value)
{
  if (_emissiveTexture == value) {
    return;
  }

  _emissiveTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_reflectivityTexture()
{
  return _reflectivityTexture;
}

void PBRMaterial::set_reflectivityTexture(const BaseTexturePtr& value)
{
  if (_reflectivityTexture == value) {
    return;
  }

  _reflectivityTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_metallicTexture()
{
  return _metallicTexture;
}

void PBRMaterial::set_metallicTexture(const BaseTexturePtr& value)
{
  if (_metallicTexture == value) {
    return;
  }

  _metallicTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

std::optional<float>& PBRMaterial::get_metallic()
{
  return _metallic;
}

void PBRMaterial::set_metallic(const std::optional<float>& value)
{
  if (_metallic == value) {
    return;
  }

  _metallic = value;
  _markAllSubMeshesAsTexturesDirty();
}

std::optional<float>& PBRMaterial::get_roughness()
{
  return _roughness;
}

void PBRMaterial::set_roughness(const std::optional<float>& value)
{
  if (_roughness == value) {
    return;
  }

  _roughness = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRMaterial::get_metallicF0Factor() const
{
  return _metallicF0Factor;
}

void PBRMaterial::set_metallicF0Factor(float value)
{
  if (stl_util::almost_equal(_metallicF0Factor, value)) {
    return;
  }

  _metallicF0Factor = value;
  _markAllSubMeshesAsTexturesDirty();
}

Color3& PBRMaterial::get_metallicReflectanceColor()
{
  return _metallicReflectanceColor;
}

void PBRMaterial::set_metallicReflectanceColor(const Color3& value)
{
  if (_metallicReflectanceColor == value) {
    return;
  }

  _metallicReflectanceColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_metallicReflectanceTexture()
{
  return _metallicReflectanceTexture;
}

void PBRMaterial::set_metallicReflectanceTexture(const BaseTexturePtr& value)
{
  if (_metallicReflectanceTexture == value) {
    return;
  }

  _metallicReflectanceTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_microSurfaceTexture()
{
  return _microSurfaceTexture;
}

void PBRMaterial::set_microSurfaceTexture(const BaseTexturePtr& value)
{
  if (_microSurfaceTexture == value) {
    return;
  }

  _microSurfaceTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_bumpTexture()
{
  return _bumpTexture;
}

void PBRMaterial::set_bumpTexture(const BaseTexturePtr& value)
{
  if (_bumpTexture == value) {
    return;
  }

  _bumpTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_lightmapTexture()
{
  return _lightmapTexture;
}

void PBRMaterial::set_lightmapTexture(const BaseTexturePtr& value)
{
  if (_lightmapTexture == value) {
    return;
  }

  _lightmapTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_refractionTexture()
{
  return subSurface->refractionTexture;
}

void PBRMaterial::set_refractionTexture(const BaseTexturePtr& value)
{
  subSurface->refractionTexture = value;
  if (value) {
    subSurface->isRefractionEnabled = true;
  }
  else if (!subSurface->linkRefractionWithTransparency()) {
    subSurface->isRefractionEnabled = false;
  }
}

Color3& PBRMaterial::get_ambientColor()
{
  return _ambientColor;
}

void PBRMaterial::set_ambientColor(const Color3& value)
{
  if (_ambientColor == value) {
    return;
  }

  _ambientColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

Color3& PBRMaterial::get_albedoColor()
{
  return _albedoColor;
}

void PBRMaterial::set_albedoColor(const Color3& value)
{
  if (_albedoColor == value) {
    return;
  }

  _albedoColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

Color3& PBRMaterial::get_reflectivityColor()
{
  return _reflectivityColor;
}

void PBRMaterial::set_reflectivityColor(const Color3& value)
{
  if (_reflectivityColor == value) {
    return;
  }

  _reflectivityColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

Color3& PBRMaterial::get_reflectionColor()
{
  return _reflectionColor;
}

void PBRMaterial::set_reflectionColor(const Color3& value)
{
  if (_reflectionColor == value) {
    return;
  }

  _reflectionColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

Color3& PBRMaterial::get_emissiveColor()
{
  return _emissiveColor;
}

void PBRMaterial::set_emissiveColor(const Color3& value)
{
  if (_emissiveColor == value) {
    return;
  }

  _emissiveColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRMaterial::get_microSurface() const
{
  return _microSurface;
}

void PBRMaterial::set_microSurface(float value)
{
  if (stl_util::almost_equal(_microSurface, value)) {
    return;
  }

  _microSurface = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRMaterial::get_indexOfRefraction() const
{
  return subSurface->indexOfRefraction();
}

void PBRMaterial::set_indexOfRefraction(float value)
{
  subSurface->indexOfRefraction = value;
}

bool PBRMaterial::get_invertRefractionY() const
{
  return subSurface->invertRefractionY();
}

void PBRMaterial::set_invertRefractionY(bool value)
{
  subSurface->invertRefractionY = value;
}

bool PBRMaterial::get_linkRefractionWithTransparency() const
{
  return subSurface->linkRefractionWithTransparency();
}

void PBRMaterial::set_linkRefractionWithTransparency(bool value)
{
  subSurface->linkRefractionWithTransparency = value;
  if (value) {
    subSurface->isRefractionEnabled = true;
  }
}

bool PBRMaterial::get_useLightmapAsShadowmap() const
{
  return _useLightmapAsShadowmap;
}

void PBRMaterial::set_useLightmapAsShadowmap(bool value)
{
  if (_useLightmapAsShadowmap == value) {
    return;
  }

  _useLightmapAsShadowmap = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useAlphaFromAlbedoTexture() const
{
  return _useAlphaFromAlbedoTexture;
}

void PBRMaterial::set_useAlphaFromAlbedoTexture(bool value)
{
  if (_useAlphaFromAlbedoTexture == value) {
    return;
  }

  _useAlphaFromAlbedoTexture = value;
  _markAllSubMeshesAsTexturesAndMiscDirty();
}

bool PBRMaterial::get_forceAlphaTest() const
{
  return _forceAlphaTest;
}

void PBRMaterial::set_forceAlphaTest(bool value)
{
  if (_forceAlphaTest == value) {
    return;
  }

  _forceAlphaTest = value;
  _markAllSubMeshesAsTexturesAndMiscDirty();
}

float PBRMaterial::get_alphaCutOff() const
{
  return _alphaCutOff;
}

void PBRMaterial::set_alphaCutOff(float value)
{
  if (stl_util::almost_equal(_alphaCutOff, value)) {
    return;
  }

  _alphaCutOff = value;
  _markAllSubMeshesAsTexturesAndMiscDirty();
}

bool PBRMaterial::get_useSpecularOverAlpha() const
{
  return _useSpecularOverAlpha;
}

void PBRMaterial::set_useSpecularOverAlpha(bool value)
{
  if (_useSpecularOverAlpha == value) {
    return;
  }

  _useSpecularOverAlpha = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useMicroSurfaceFromReflectivityMapAlpha() const
{
  return _useMicroSurfaceFromReflectivityMapAlpha;
}

void PBRMaterial::set_useMicroSurfaceFromReflectivityMapAlpha(bool value)
{
  if (_useMicroSurfaceFromReflectivityMapAlpha == value) {
    return;
  }

  _useMicroSurfaceFromReflectivityMapAlpha = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useRoughnessFromMetallicTextureAlpha() const
{
  return _useRoughnessFromMetallicTextureAlpha;
}

void PBRMaterial::set_useRoughnessFromMetallicTextureAlpha(bool value)
{
  if (_useRoughnessFromMetallicTextureAlpha == value) {
    return;
  }

  _useRoughnessFromMetallicTextureAlpha = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useRoughnessFromMetallicTextureGreen() const
{
  return _useRoughnessFromMetallicTextureGreen;
}

void PBRMaterial::set_useRoughnessFromMetallicTextureGreen(bool value)
{
  if (_useRoughnessFromMetallicTextureGreen == value) {
    return;
  }

  _useRoughnessFromMetallicTextureGreen = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useMetallnessFromMetallicTextureBlue() const
{
  return _useMetallnessFromMetallicTextureBlue;
}

void PBRMaterial::set_useMetallnessFromMetallicTextureBlue(bool value)
{
  if (_useMetallnessFromMetallicTextureBlue == value) {
    return;
  }

  _useMetallnessFromMetallicTextureBlue = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useAmbientOcclusionFromMetallicTextureRed() const
{
  return _useAmbientOcclusionFromMetallicTextureRed;
}

void PBRMaterial::set_useAmbientOcclusionFromMetallicTextureRed(bool value)
{
  if (_useAmbientOcclusionFromMetallicTextureRed == value) {
    return;
  }

  _useAmbientOcclusionFromMetallicTextureRed = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useAmbientInGrayScale() const
{
  return _useAmbientInGrayScale;
}

void PBRMaterial::set_useAmbientInGrayScale(bool value)
{
  if (_useAmbientInGrayScale == value) {
    return;
  }

  _useAmbientInGrayScale = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useAutoMicroSurfaceFromReflectivityMap() const
{
  return _useAutoMicroSurfaceFromReflectivityMap;
}

void PBRMaterial::set_useAutoMicroSurfaceFromReflectivityMap(bool value)
{
  if (_useAutoMicroSurfaceFromReflectivityMap == value) {
    return;
  }

  _useAutoMicroSurfaceFromReflectivityMap = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_usePhysicalLightFalloff() const
{
  return _lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_PHYSICAL;
}

void PBRMaterial::set_usePhysicalLightFalloff(bool value)
{
  if (value != usePhysicalLightFalloff()) {
    // Ensure the effect will be rebuilt.
    _markAllSubMeshesAsTexturesDirty();

    if (value) {
      _lightFalloff = PBRBaseMaterial::LIGHTFALLOFF_PHYSICAL;
    }
    else {
      _lightFalloff = PBRBaseMaterial::LIGHTFALLOFF_STANDARD;
    }
  }
}

bool PBRMaterial::get_useGLTFLightFalloff() const
{
  return _lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_GLTF;
}

void PBRMaterial::set_useGLTFLightFalloff(bool value)
{
  if (value != useGLTFLightFalloff()) {
    // Ensure the effect will be rebuilt.
    _markAllSubMeshesAsTexturesDirty();

    if (value) {
      _lightFalloff = PBRBaseMaterial::LIGHTFALLOFF_GLTF;
    }
    else {
      _lightFalloff = PBRBaseMaterial::LIGHTFALLOFF_STANDARD;
    }
  }
}

bool PBRMaterial::get_useRadianceOverAlpha() const
{
  return _useRadianceOverAlpha;
}

void PBRMaterial::set_useRadianceOverAlpha(bool value)
{
  if (_useRadianceOverAlpha == value) {
    return;
  }

  _useRadianceOverAlpha = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useObjectSpaceNormalMap() const
{
  return _useObjectSpaceNormalMap;
}

void PBRMaterial::set_useObjectSpaceNormalMap(bool value)
{
  if (_useObjectSpaceNormalMap == value) {
    return;
  }

  _useObjectSpaceNormalMap = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useParallax() const
{
  return _useParallax;
}

void PBRMaterial::set_useParallax(bool value)
{
  if (_useParallax == value) {
    return;
  }

  _useParallax = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useParallaxOcclusion() const
{
  return _useParallaxOcclusion;
}

void PBRMaterial::set_useParallaxOcclusion(bool value)
{
  if (_useParallaxOcclusion == value) {
    return;
  }

  _useParallaxOcclusion = value;
  _markAllSubMeshesAsTexturesDirty();
}

float PBRMaterial::get_parallaxScaleBias() const
{
  return _parallaxScaleBias;
}

void PBRMaterial::set_parallaxScaleBias(float value)
{
  if (stl_util::almost_equal(_parallaxScaleBias, value)) {
    return;
  }

  _parallaxScaleBias = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void PBRMaterial::set_disableLighting(bool value)
{
  if (_disableLighting == value) {
    return;
  }

  _disableLighting = value;
  _markAllSubMeshesAsLightsDirty();
}

bool PBRMaterial::get_forceIrradianceInFragment() const
{
  return _forceIrradianceInFragment;
}

void PBRMaterial::set_forceIrradianceInFragment(bool value)
{
  if (_forceIrradianceInFragment == value) {
    return;
  }

  _forceIrradianceInFragment = value;
  _markAllSubMeshesAsTexturesDirty();
}

unsigned int PBRMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void PBRMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights == value) {
    return;
  }

  _maxSimultaneousLights = value;
  _markAllSubMeshesAsLightsDirty();
}

bool PBRMaterial::get_invertNormalMapX() const
{
  return _invertNormalMapX;
}

void PBRMaterial::set_invertNormalMapX(bool value)
{
  if (_invertNormalMapX == value) {
    return;
  }

  _invertNormalMapX = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_invertNormalMapY() const
{
  return _invertNormalMapY;
}

void PBRMaterial::set_invertNormalMapY(bool value)
{
  if (_invertNormalMapY == value) {
    return;
  }

  _invertNormalMapY = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_twoSidedLighting() const
{
  return _twoSidedLighting;
}

void PBRMaterial::set_twoSidedLighting(bool value)
{
  if (_twoSidedLighting == value) {
    return;
  }

  _twoSidedLighting = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useAlphaFresnel() const
{
  return _useAlphaFresnel;
}

void PBRMaterial::set_useAlphaFresnel(bool value)
{
  if (_useAlphaFresnel == value) {
    return;
  }

  _useAlphaFresnel = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useLinearAlphaFresnel() const
{
  return _useLinearAlphaFresnel;
}

void PBRMaterial::set_useLinearAlphaFresnel(bool value)
{
  if (_useLinearAlphaFresnel == value) {
    return;
  }

  _useLinearAlphaFresnel = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& PBRMaterial::get_environmentBRDFTexture()
{
  return _environmentBRDFTexture;
}

void PBRMaterial::set_environmentBRDFTexture(const BaseTexturePtr& value)
{
  if (_environmentBRDFTexture == value) {
    return;
  }

  _environmentBRDFTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_forceNormalForward() const
{
  return _forceNormalForward;
}

void PBRMaterial::set_forceNormalForward(bool value)
{
  if (_forceNormalForward == value) {
    return;
  }

  _forceNormalForward = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_enableSpecularAntiAliasing() const
{
  return _enableSpecularAntiAliasing;
}

void PBRMaterial::set_enableSpecularAntiAliasing(bool value)
{
  if (_enableSpecularAntiAliasing == value) {
    return;
  }

  _enableSpecularAntiAliasing = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useHorizonOcclusion() const
{
  return _useHorizonOcclusion;
}

void PBRMaterial::set_useHorizonOcclusion(bool value)
{
  if (_useHorizonOcclusion == value) {
    return;
  }

  _useHorizonOcclusion = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_useRadianceOcclusion() const
{
  return _useRadianceOcclusion;
}

void PBRMaterial::set_useRadianceOcclusion(bool value)
{
  if (_useRadianceOcclusion == value) {
    return;
  }

  _useRadianceOcclusion = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_unlit() const
{
  return _unlit;
}

void PBRMaterial::set_unlit(bool value)
{
  if (_unlit == value) {
    return;
  }

  _unlit = value;
  _markAllSubMeshesAsMiscDirty();
}

ImageProcessingConfigurationPtr& PBRMaterial::get_imageProcessingConfiguration()
{
  return _imageProcessingConfiguration;
}

void PBRMaterial::set_imageProcessingConfiguration(const ImageProcessingConfigurationPtr& value)
{
  _attachImageProcessingConfiguration(value);

  // Ensure the effect will be rebuilt.
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::get_cameraColorCurvesEnabled() const
{
  return imageProcessingConfiguration()->colorCurvesEnabled();
}

void PBRMaterial::set_cameraColorCurvesEnabled(bool value)
{
  imageProcessingConfiguration()->colorCurvesEnabled = value;
}

bool PBRMaterial::get_cameraColorGradingEnabled() const
{
  return imageProcessingConfiguration()->colorGradingEnabled();
}

void PBRMaterial::set_cameraColorGradingEnabled(bool value)
{
  imageProcessingConfiguration()->colorGradingEnabled = value;
}

bool PBRMaterial::get_cameraToneMappingEnabled() const
{
  return _imageProcessingConfiguration->toneMappingEnabled();
}

void PBRMaterial::set_cameraToneMappingEnabled(bool value)
{
  _imageProcessingConfiguration->toneMappingEnabled = value;
}

float PBRMaterial::get_cameraExposure() const
{
  return _imageProcessingConfiguration->exposure();
}

void PBRMaterial::set_cameraExposure(float value)
{
  _imageProcessingConfiguration->exposure = value;
}

float PBRMaterial::get_cameraContrast() const
{
  return _imageProcessingConfiguration->contrast();
}

void PBRMaterial::set_cameraContrast(float value)
{
  _imageProcessingConfiguration->contrast = value;
}

BaseTexturePtr& PBRMaterial::get_cameraColorGradingTexture()
{
  return _imageProcessingConfiguration->colorGradingTexture;
}

void PBRMaterial::set_cameraColorGradingTexture(const BaseTexturePtr& value)
{
  _imageProcessingConfiguration->colorGradingTexture = value;
}

ColorCurvesPtr& PBRMaterial::get_cameraColorCurves()
{
  return _imageProcessingConfiguration->colorCurves;
}

void PBRMaterial::set_cameraColorCurves(const ColorCurvesPtr& value)
{
  _imageProcessingConfiguration->colorCurves = value;
}

std::string PBRMaterial::getClassName() const
{
  return "PBRMaterial";
}

MaterialPtr PBRMaterial::clone(const std::string& /*name*/, bool /*cloneChildren*/) const
{
  return nullptr;
}

json PBRMaterial::serialize() const
{
  return nullptr;
}

// Statics
PBRMaterial* PBRMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
