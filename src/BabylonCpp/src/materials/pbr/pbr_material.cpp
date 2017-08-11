#include <babylon/materials/pbr/pbr_material.h>

#include <babylon/core/json.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/tools/texture_tools.h>

namespace BABYLON {

PBRMaterial::PBRMaterial(const std::string& iName, Scene* scene)
    : PBRBaseMaterial{iName, scene}
    , directIntensity{1.f}
    , emissiveIntensity{1.f}
    , environmentIntensity{1.f}
    , specularIntensity{1.f}
    , disableBumpMap{false}
    , albedoTexture{nullptr}
    , ambientTexture{nullptr}
    , ambientTextureStrength{1.f}
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
    , usePhysicalLightFalloff{true}
    , useRadianceOverAlpha{true}
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
    , environmentBRDFTexture{nullptr}
    , forceNormalForward{false}
{
  _environmentBRDFTexture = TextureTools::GetEnvironmentBRDFTexture(scene);
}

PBRMaterial::~PBRMaterial()
{
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
  imageProcessingConfiguration()->setColorCurvesEnabled(value);
}

bool PBRMaterial::cameraColorGradingEnabled() const
{
  return imageProcessingConfiguration()->colorGradingEnabled();
}

void PBRMaterial::setCameraColorGradingEnabled(bool value)
{
  imageProcessingConfiguration()->setColorGradingEnabled(value);
}

bool PBRMaterial::cameraToneMappingEnabled() const
{
  return _imageProcessingConfiguration->toneMappingEnabled();
}

void PBRMaterial::setCameraToneMappingEnabled(bool value)
{
  _imageProcessingConfiguration->setToneMappingEnabled(value);
}

float PBRMaterial::cameraExposure() const
{
  return _imageProcessingConfiguration->exposure();
}

void PBRMaterial::setCameraExposure(float value)
{
  _imageProcessingConfiguration->setExposure(value);
}

float PBRMaterial::cameraContrast() const
{
  return _imageProcessingConfiguration->contrast();
}

void PBRMaterial::setCameraContrast(float value)
{
  _imageProcessingConfiguration->setContrast(value);
}

BaseTexture* PBRMaterial::cameraColorGradingTexture() const
{
  return _imageProcessingConfiguration->colorGradingTexture;
}

void PBRMaterial::setCameraColorGradingTexture(BaseTexture* value)
{
  _imageProcessingConfiguration->colorGradingTexture = value;
}

ColorCurves* PBRMaterial::cameraColorCurves()
{
  return _imageProcessingConfiguration->colorCurves.get();
}

void PBRMaterial::setCameraColorCurves(ColorCurves* /*value*/)
{
  // _imageProcessingConfiguration.colorCurves = value;
}

const char* PBRMaterial::getClassName() const
{
  return "PBRMaterial";
}

std::vector<BaseTexture*> PBRMaterial::getActiveTextures() const
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

bool PBRMaterial::hasTexture(BaseTexture* texture) const
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

PBRMaterial* PBRMaterial::clone(const std::string& /*name*/,
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
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
