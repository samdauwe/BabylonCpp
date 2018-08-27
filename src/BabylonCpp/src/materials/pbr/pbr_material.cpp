#include <babylon/materials/pbr/pbr_material.h>

#include <babylon/core/json.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/tools/texture_tools.h>

namespace BABYLON {

PBRMaterial::PBRMaterial(const string_t& iName, Scene* scene)
    : PBRBaseMaterial{iName, scene}
    , directIntensity{1.f}
    , emissiveIntensity{1.f}
    , environmentIntensity{1.f}
    , specularIntensity{1.f}
    , disableBumpMap{false}
    , albedoTexture{nullptr}
    , ambientTexture{nullptr}
    , ambientTextureStrength{1.f}
    , ambientTextureImpactOnAnalyticalLights{PBRMaterial::
                                               DEFAULT_AO_ON_ANALYTICAL_LIGHTS}
    , opacityTexture{nullptr}
    , reflectionTexture{nullptr}
    , emissiveTexture{nullptr}
    , reflectivityTexture{nullptr}
    , metallicTexture{nullptr}
    , metallic{0.f}
    , roughness{0.f}
    , microSurfaceTexture{nullptr}
    , bumpTexture{nullptr}
    , lightmapTexture{nullptr}
    , refractionTexture{nullptr}
    , ambientColor{Color3(0.f, 0.f, 0.f)}
    , albedoColor{Color3(1.f, 1.f, 1.f)}
    , reflectivityColor{Color3(1.f, 1.f, 1.f)}
    , reflectionColor{Color3(1.f, 1.f, 1.f)}
    , emissiveColor{Color3(0.f, 0.f, 0.f)}
    , microSurface{1.f}
    , indexOfRefraction{0.66f}
    , invertRefractionY{false}
    , linkRefractionWithTransparency{false}
    , useLightmapAsShadowmap{false}
    , useAlphaFromAlbedoTexture{false}
    , forceAlphaTest{false}
    , alphaCutOff{0.4f}
    , useSpecularOverAlpha{true}
    , useMicroSurfaceFromReflectivityMapAlpha{false}
    , useRoughnessFromMetallicTextureAlpha{true}
    , useRoughnessFromMetallicTextureGreen{false}
    , useMetallnessFromMetallicTextureBlue{false}
    , useAmbientOcclusionFromMetallicTextureRed{false}
    , useAmbientInGrayScale{false}
    , useAutoMicroSurfaceFromReflectivityMap{false}
    , useRadianceOverAlpha{true}
    , useObjectSpaceNormalMap{false}
    , useParallax{false}
    , useParallaxOcclusion{false}
    , parallaxScaleBias{0.05f}
    , disableLighting{false}
    , forceIrradianceInFragment{false}
    , maxSimultaneousLights{4}
    , invertNormalMapX{false}
    , invertNormalMapY{false}
    , twoSidedLighting{false}
    , preMultiplyAlpha{false}
    , useAlphaFresnel{false}
    , useLinearAlphaFresnel{false}
    , environmentBRDFTexture{nullptr}
    , forceNormalForward{false}
    , enableSpecularAntiAliasing{false}
    , useHorizonOcclusion{true}
    , useRadianceOcclusion{true}
    , unlit{false}
{
  _environmentBRDFTexture = TextureTools::GetEnvironmentBRDFTexture(scene);
}

PBRMaterial::~PBRMaterial()
{
}

bool PBRMaterial::usePhysicalLightFalloff() const
{
  return _lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_PHYSICAL;
}

void PBRMaterial::setUsePhysicalLightFalloff(bool value)
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

bool PBRMaterial::useGLTFLightFalloff() const
{
  return _lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_GLTF;
}

void PBRMaterial::setUseGLTFLightFalloff(bool value)
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

ImageProcessingConfiguration* PBRMaterial::imageProcessingConfiguration()
{
  return _imageProcessingConfiguration;
}

const ImageProcessingConfiguration*
PBRMaterial::imageProcessingConfiguration() const
{
  return _imageProcessingConfiguration;
}

void PBRMaterial::setImageProcessingConfiguration(
  ImageProcessingConfiguration* value)
{
  _attachImageProcessingConfiguration(value);

  // Ensure the effect will be rebuilt.
  _markAllSubMeshesAsTexturesDirty();
}

bool PBRMaterial::cameraColorCurvesEnabled() const
{
  return imageProcessingConfiguration()->colorCurvesEnabled();
}

void PBRMaterial::setCameraColorCurvesEnabled(bool value)
{
  imageProcessingConfiguration()->colorCurvesEnabled = value;
}

bool PBRMaterial::cameraColorGradingEnabled() const
{
  return imageProcessingConfiguration()->colorGradingEnabled();
}

void PBRMaterial::setCameraColorGradingEnabled(bool value)
{
  imageProcessingConfiguration()->colorGradingEnabled = value;
}

bool PBRMaterial::cameraToneMappingEnabled() const
{
  return _imageProcessingConfiguration->toneMappingEnabled();
}

void PBRMaterial::setCameraToneMappingEnabled(bool value)
{
  _imageProcessingConfiguration->toneMappingEnabled = value;
}

float PBRMaterial::cameraExposure() const
{
  return _imageProcessingConfiguration->exposure();
}

void PBRMaterial::setCameraExposure(float value)
{
  _imageProcessingConfiguration->exposure = value;
}

float PBRMaterial::cameraContrast() const
{
  return _imageProcessingConfiguration->contrast();
}

void PBRMaterial::setCameraContrast(float value)
{
  _imageProcessingConfiguration->contrast = value;
}

BaseTexturePtr& PBRMaterial::cameraColorGradingTexture()
{
  return _imageProcessingConfiguration->colorGradingTexture;
}

void PBRMaterial::setCameraColorGradingTexture(const BaseTexturePtr& value)
{
  _imageProcessingConfiguration->colorGradingTexture = value;
}

ColorCurvesPtr& PBRMaterial::cameraColorCurves()
{
  return _imageProcessingConfiguration->colorCurves;
}

void PBRMaterial::setCameraColorCurves(ColorCurves* /*value*/)
{
  // _imageProcessingConfiguration.colorCurves = value;
}

const string_t PBRMaterial::getClassName() const
{
  return "PBRMaterial";
}

vector_t<BaseTexturePtr> PBRMaterial::getActiveTextures() const
{
  auto activeTextures = PBRBaseMaterial::getActiveTextures();

  if (_albedoTexture) {
    activeTextures.emplace_back(_albedoTexture);
  }

  if (_ambientTexture) {
    activeTextures.emplace_back(_ambientTexture);
  }

  if (_opacityTexture) {
    activeTextures.emplace_back(_opacityTexture);
  }

  if (_reflectionTexture) {
    activeTextures.emplace_back(_reflectionTexture);
  }

  if (_emissiveTexture) {
    activeTextures.emplace_back(_emissiveTexture);
  }

  if (_reflectivityTexture) {
    activeTextures.emplace_back(_reflectivityTexture);
  }

  if (_metallicTexture) {
    activeTextures.emplace_back(_metallicTexture);
  }

  if (_microSurfaceTexture) {
    activeTextures.emplace_back(_microSurfaceTexture);
  }

  if (_bumpTexture) {
    activeTextures.emplace_back(_bumpTexture);
  }

  if (_lightmapTexture) {
    activeTextures.emplace_back(_lightmapTexture);
  }

  if (_refractionTexture) {
    activeTextures.emplace_back(_refractionTexture);
  }

  return activeTextures;
}

bool PBRMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PBRBaseMaterial::hasTexture(texture)) {
    return true;
  }

  if (_albedoTexture == texture) {
    return true;
  }

  if (_ambientTexture == texture) {
    return true;
  }

  if (_opacityTexture == texture) {
    return true;
  }

  if (_reflectionTexture == texture) {
    return true;
  }

  if (_reflectivityTexture == texture) {
    return true;
  }

  if (_metallicTexture == texture) {
    return true;
  }

  if (_microSurfaceTexture == texture) {
    return true;
  }

  if (_bumpTexture == texture) {
    return true;
  }

  if (_lightmapTexture == texture) {
    return true;
  }

  if (_refractionTexture == texture) {
    return true;
  }

  return false;
}

MaterialPtr PBRMaterial::clone(const string_t& /*name*/,
                               bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object PBRMaterial::serialize() const
{
  return Json::object();
}

// Statics
PBRMaterial* PBRMaterial::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                                const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
