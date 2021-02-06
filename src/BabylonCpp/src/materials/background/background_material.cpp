#include <babylon/materials/background/background_material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/background/background_material_defines.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

float BackgroundMaterial::_standardReflectance0  = 0.05f;
float BackgroundMaterial::_standardReflectance90 = 0.5f;

float BackgroundMaterial::StandardReflectance0()
{
  return _standardReflectance0;
}

float BackgroundMaterial::StandardReflectance90()
{
  return _standardReflectance90;
}

BackgroundMaterial::BackgroundMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , primaryColor{this, &BackgroundMaterial::get_primaryColor,
                   &BackgroundMaterial::set_primaryColor}
    , _perceptualColor{this, &BackgroundMaterial::get__perceptualColor,
                       &BackgroundMaterial::set__perceptualColor}
    , primaryColorShadowLevel{this, &BackgroundMaterial::get_primaryColorShadowLevel,
                              &BackgroundMaterial::set_primaryColorShadowLevel}
    , primaryColorHighlightLevel{this, &BackgroundMaterial::get_primaryColorHighlightLevel,
                                 &BackgroundMaterial::set_primaryColorHighlightLevel}
    , reflectionTexture{this, &BackgroundMaterial::get_reflectionTexture,
                        &BackgroundMaterial::set_reflectionTexture}
    , reflectionBlur{this, &BackgroundMaterial::get_reflectionBlur,
                     &BackgroundMaterial::set_reflectionBlur}
    , diffuseTexture{this, &BackgroundMaterial::get_diffuseTexture,
                     &BackgroundMaterial::set_diffuseTexture}
    , shadowLights{this, &BackgroundMaterial::get_shadowLights,
                   &BackgroundMaterial::set_shadowLights}
    , shadowLevel{this, &BackgroundMaterial::get_shadowLevel, &BackgroundMaterial::set_shadowLevel}
    , sceneCenter{this, &BackgroundMaterial::get_sceneCenter, &BackgroundMaterial::set_sceneCenter}
    , opacityFresnel{this, &BackgroundMaterial::get_opacityFresnel,
                     &BackgroundMaterial::set_opacityFresnel}
    , reflectionFresnel{this, &BackgroundMaterial::get_reflectionFresnel,
                        &BackgroundMaterial::set_reflectionFresnel}
    , reflectionFalloffDistance{this, &BackgroundMaterial::get_reflectionFalloffDistance,
                                &BackgroundMaterial::set_reflectionFalloffDistance}
    , reflectionAmount{this, &BackgroundMaterial::get_reflectionAmount,
                       &BackgroundMaterial::set_reflectionAmount}
    , reflectionReflectance0{this, &BackgroundMaterial::get_reflectionReflectance0,
                             &BackgroundMaterial::set_reflectionReflectance0}
    , reflectionReflectance90{this, &BackgroundMaterial::get_reflectionReflectance90,
                              &BackgroundMaterial::set_reflectionReflectance90}
    , reflectionStandardFresnelWeight{this,
                                      &BackgroundMaterial::set_reflectionStandardFresnelWeight}
    , useRGBColor{this, &BackgroundMaterial::get_useRGBColor, &BackgroundMaterial::set_useRGBColor}
    , enableNoise{this, &BackgroundMaterial::get_enableNoise, &BackgroundMaterial::set_enableNoise}
    , fovMultiplier{this, &BackgroundMaterial::get_fovMultiplier,
                    &BackgroundMaterial::set_fovMultiplier}
    , useEquirectangularFOV{false}
    , maxSimultaneousLights{this, &BackgroundMaterial::get_maxSimultaneousLights,
                            &BackgroundMaterial::set_maxSimultaneousLights}
    , shadowOnly{this, &BackgroundMaterial::get_shadowOnly, &BackgroundMaterial::set_shadowOnly}
    , imageProcessingConfiguration{this, &BackgroundMaterial::get_imageProcessingConfiguration,
                                   &BackgroundMaterial::set_imageProcessingConfiguration}
    , cameraColorCurvesEnabled{this, &BackgroundMaterial::get_cameraColorCurvesEnabled,
                               &BackgroundMaterial::set_cameraColorCurvesEnabled}
    , cameraColorGradingEnabled{this, &BackgroundMaterial::get_cameraColorGradingEnabled,
                                &BackgroundMaterial::set_cameraColorGradingEnabled}
    , cameraToneMappingEnabled{this, &BackgroundMaterial::get_cameraToneMappingEnabled,
                               &BackgroundMaterial::set_cameraToneMappingEnabled}
    , cameraExposure{this, &BackgroundMaterial::get_cameraExposure,
                     &BackgroundMaterial::set_cameraExposure}
    , cameraContrast{this, &BackgroundMaterial::get_cameraContrast,
                     &BackgroundMaterial::set_cameraContrast}
    , cameraColorGradingTexture{this, &BackgroundMaterial::get_cameraColorGradingTexture,
                                &BackgroundMaterial::set_cameraColorGradingTexture}
    , cameraColorCurves{this, &BackgroundMaterial::get_cameraColorCurves,
                        &BackgroundMaterial::set_cameraColorCurves}
    , _primaryColor{Color3::White()}
    , __perceptualColor{std::nullopt}
    , _primaryColorShadowLevel{0.f}
    , _primaryColorHighlightLevel{0.f}
    , _reflectionTexture{nullptr}
    , _reflectionBlur{0.f}
    , _diffuseTexture{nullptr}
    , _shadowLevel{0.f}
    , _sceneCenter{Vector3::Zero()}
    , _opacityFresnel{true}
    , _reflectionFresnel{false}
    , _reflectionFalloffDistance{0.f}
    , _reflectionAmount{1.f}
    , _reflectionReflectance0{0.05f}
    , _reflectionReflectance90{0.5f}
    , _useRGBColor{true}
    , _enableNoise{false}
    , _imageProcessingConfiguration{nullptr}
    , _fovMultiplier{1.f}
    , _maxSimultaneousLights{4}
    , _shadowOnly{false}
    , switchToBGR{false}
    , _imageProcessingObserver{nullptr}
    , _reflectionControls{Vector4::Zero()}
    , _white{Color3::White()}
    , _primaryShadowColor{Color3::Black()}
    , _primaryHighlightColor{Color3::Black()}
{
  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);

  getRenderTargetTextures = [this]() -> std::vector<RenderTargetTexturePtr> {
    _renderTargets.clear();

    if (_diffuseTexture && _diffuseTexture->isRenderTarget) {
      _renderTargets.emplace_back(std::static_pointer_cast<RenderTargetTexture>(_diffuseTexture));
    }

    if (_reflectionTexture && _reflectionTexture->isRenderTarget) {
      _renderTargets.emplace_back(
        std::static_pointer_cast<RenderTargetTexture>(_reflectionTexture));
    }

    return _renderTargets;
  };
}

BackgroundMaterial::~BackgroundMaterial() = default;

Color3& BackgroundMaterial::get_primaryColor()
{
  return _primaryColor;
}

void BackgroundMaterial::set_primaryColor(const Color3& value)
{
  if (_primaryColor == value) {
    return;
  }

  _primaryColor = value;
  _markAllSubMeshesAsLightsDirty();
}

std::optional<Color3>& BackgroundMaterial::get__perceptualColor()
{
  return __perceptualColor;
}

void BackgroundMaterial::set__perceptualColor(const std::optional<Color3>& value)
{
  __perceptualColor = value;
  _computePrimaryColorFromPerceptualColor();
  _markAllSubMeshesAsLightsDirty();
}

float BackgroundMaterial::get_primaryColorShadowLevel() const
{
  return _primaryColorShadowLevel;
}

void BackgroundMaterial::set_primaryColorShadowLevel(float value)
{
  _primaryColorShadowLevel = value;
  _computePrimaryColors();
  _markAllSubMeshesAsLightsDirty();
}

float BackgroundMaterial::get_primaryColorHighlightLevel() const
{
  return _primaryColorHighlightLevel;
}

void BackgroundMaterial::set_primaryColorHighlightLevel(float value)
{
  _primaryColorHighlightLevel = value;
  _computePrimaryColors();
  _markAllSubMeshesAsLightsDirty();
}

BaseTexturePtr& BackgroundMaterial::get_reflectionTexture()
{
  return _reflectionTexture;
}

void BackgroundMaterial::set_reflectionTexture(const BaseTexturePtr& value)
{
  if (_reflectionTexture == value) {
    return;
  }

  _reflectionTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

float BackgroundMaterial::get_reflectionBlur() const
{
  return _reflectionBlur;
}

void BackgroundMaterial::set_reflectionBlur(float value)
{
  if (stl_util::almost_equal(_reflectionBlur, value)) {
    return;
  }

  _reflectionBlur = value;
  _markAllSubMeshesAsTexturesDirty();
}

BaseTexturePtr& BackgroundMaterial::get_diffuseTexture()
{
  return _diffuseTexture;
}

void BackgroundMaterial::set_diffuseTexture(const BaseTexturePtr& value)
{
  if (_diffuseTexture == value) {
    return;
  }

  _diffuseTexture = value;
  _markAllSubMeshesAsTexturesDirty();
}

std::vector<IShadowLightPtr>& BackgroundMaterial::get_shadowLights()
{
  return _shadowLights;
}

void BackgroundMaterial::set_shadowLights(const std::vector<IShadowLightPtr>& iShadowLights)
{
  _shadowLights = iShadowLights;
  _markAllSubMeshesAsTexturesDirty();
}

float BackgroundMaterial::get_shadowLevel() const
{
  return _shadowLevel;
}

void BackgroundMaterial::set_shadowLevel(float value)
{
  if (stl_util::almost_equal(_shadowLevel, value)) {
    return;
  }

  _shadowLevel = value;
  _markAllSubMeshesAsTexturesDirty();
}

Vector3& BackgroundMaterial::get_sceneCenter()
{
  return _sceneCenter;
}
void BackgroundMaterial::set_sceneCenter(const Vector3& value)
{
  if (_sceneCenter == value) {
    return;
  }

  _sceneCenter = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool BackgroundMaterial::get_opacityFresnel() const
{
  return _opacityFresnel;
}

void BackgroundMaterial::set_opacityFresnel(bool value)
{
  if (_opacityFresnel == value) {
    return;
  }

  _opacityFresnel = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool BackgroundMaterial::get_reflectionFresnel() const
{
  return _reflectionFresnel;
}

void BackgroundMaterial::set_reflectionFresnel(bool value)
{
  if (_reflectionFresnel == value) {
    return;
  }

  _reflectionFresnel = value;
  _markAllSubMeshesAsTexturesDirty();
}

float BackgroundMaterial::get_reflectionFalloffDistance() const
{
  return _reflectionFalloffDistance;
}

void BackgroundMaterial::set_reflectionFalloffDistance(float value)
{
  if (stl_util::almost_equal(_reflectionFalloffDistance, value)) {
    return;
  }

  _reflectionFalloffDistance = value;
  _markAllSubMeshesAsTexturesDirty();
}

float BackgroundMaterial::get_reflectionAmount() const
{
  return _reflectionAmount;
}

void BackgroundMaterial::set_reflectionAmount(float value)
{
  if (stl_util::almost_equal(_reflectionAmount, value)) {
    return;
  }

  _reflectionAmount = value;
  _markAllSubMeshesAsTexturesDirty();
}

float BackgroundMaterial::get_reflectionReflectance0() const
{
  return _reflectionReflectance0;
}

void BackgroundMaterial::set_reflectionReflectance0(float value)
{
  if (stl_util::almost_equal(_reflectionReflectance0, value)) {
    return;
  }

  _reflectionReflectance0 = value;
  _markAllSubMeshesAsTexturesDirty();
}

float BackgroundMaterial::get_reflectionReflectance90() const
{
  return _reflectionReflectance90;
}

void BackgroundMaterial::set_reflectionReflectance90(float value)
{
  if (stl_util::almost_equal(_reflectionReflectance90, value)) {
    return;
  }

  _reflectionReflectance90 = value;
  _markAllSubMeshesAsTexturesDirty();
}

void BackgroundMaterial::set_reflectionStandardFresnelWeight(float value)
{
  auto reflectionWeight = value;

  if (reflectionWeight < 0.5f) {
    reflectionWeight        = reflectionWeight * 2.f;
    reflectionReflectance0  = BackgroundMaterial::StandardReflectance0() * reflectionWeight;
    reflectionReflectance90 = BackgroundMaterial::StandardReflectance90() * reflectionWeight;
  }
  else {
    reflectionWeight = reflectionWeight * 2.f - 1.f;
    reflectionReflectance0
      = BackgroundMaterial::StandardReflectance0()
        + (1.f - BackgroundMaterial::StandardReflectance0()) * reflectionWeight;
    reflectionReflectance90
      = BackgroundMaterial::StandardReflectance90()
        + (1.f - BackgroundMaterial::StandardReflectance90()) * reflectionWeight;
  }
}

bool BackgroundMaterial::get_useRGBColor() const
{
  return _useRGBColor;
}

void BackgroundMaterial::set_useRGBColor(bool value)
{
  if (_useRGBColor == value) {
    return;
  }

  _useRGBColor = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool BackgroundMaterial::get_enableNoise() const
{
  return _enableNoise;
}

void BackgroundMaterial::set_enableNoise(bool value)
{
  if (_enableNoise == value) {
    return;
  }

  _enableNoise = value;
  _markAllSubMeshesAsTexturesDirty();
}

float BackgroundMaterial::get_fovMultiplier() const
{
  return _fovMultiplier;
}

void BackgroundMaterial::set_fovMultiplier(float value)
{
  if (std::isnan(value)) {
    value = 1.f;
  }
  _fovMultiplier = std::max(0.f, std::min(2.f, value));
}

unsigned int BackgroundMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void BackgroundMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights == value) {
    return;
  }

  _maxSimultaneousLights = value;
  _markAllSubMeshesAsTexturesDirty();
}

bool BackgroundMaterial::get_shadowOnly() const
{
  return _shadowOnly;
}

void BackgroundMaterial::set_shadowOnly(bool value)
{
  if (_shadowOnly == value) {
    return;
  }

  _shadowOnly = value;
  _markAllSubMeshesAsLightsDirty();
}

void BackgroundMaterial::_attachImageProcessingConfiguration(
  const ImageProcessingConfigurationPtr& configuration)
{
  if (configuration == _imageProcessingConfiguration) {
    return;
  }

  // Detaches observer.
  if (_imageProcessingConfiguration && _imageProcessingObserver) {
    _imageProcessingConfiguration->onUpdateParameters.remove(_imageProcessingObserver);
  }

  // Pick the scene configuration if needed.
  if (!configuration) {
    _imageProcessingConfiguration = getScene()->imageProcessingConfiguration();
  }
  else {
    _imageProcessingConfiguration = configuration;
  }

  // Attaches observer.
  if (_imageProcessingConfiguration) {
    _imageProcessingObserver = _imageProcessingConfiguration->onUpdateParameters.add(
      [this](ImageProcessingConfiguration* /*conf*/, EventState /*es*/) {
        _computePrimaryColorFromPerceptualColor();
        _markAllSubMeshesAsImageProcessingDirty();
      });
  }
}

ImageProcessingConfigurationPtr& BackgroundMaterial::get_imageProcessingConfiguration()
{
  return _imageProcessingConfiguration;
}

void BackgroundMaterial::set_imageProcessingConfiguration(
  const ImageProcessingConfigurationPtr& value)
{
  _attachImageProcessingConfiguration(value);

  // Ensure the effect will be rebuilt.
  _markAllSubMeshesAsTexturesDirty();
}

bool BackgroundMaterial::get_cameraColorCurvesEnabled() const
{
  return _imageProcessingConfiguration->colorCurvesEnabled();
}

void BackgroundMaterial::set_cameraColorCurvesEnabled(bool value)
{
  _imageProcessingConfiguration->colorCurvesEnabled = value;
}

bool BackgroundMaterial::get_cameraColorGradingEnabled() const
{
  return _imageProcessingConfiguration->colorGradingEnabled();
}

void BackgroundMaterial::set_cameraColorGradingEnabled(bool value)
{
  _imageProcessingConfiguration->colorGradingEnabled = value;
}

bool BackgroundMaterial::get_cameraToneMappingEnabled() const
{
  return _imageProcessingConfiguration->toneMappingEnabled();
}

void BackgroundMaterial::set_cameraToneMappingEnabled(bool value)
{
  _imageProcessingConfiguration->toneMappingEnabled = value;
}

float BackgroundMaterial::get_cameraExposure() const
{
  return _imageProcessingConfiguration->exposure();
}

void BackgroundMaterial::set_cameraExposure(float value)
{
  _imageProcessingConfiguration->exposure = value;
}

float BackgroundMaterial::get_cameraContrast() const
{
  return _imageProcessingConfiguration->contrast();
}

void BackgroundMaterial::set_cameraContrast(float value)
{
  _imageProcessingConfiguration->contrast = value;
}

BaseTexturePtr& BackgroundMaterial::get_cameraColorGradingTexture()
{
  return _imageProcessingConfiguration->colorGradingTexture;
}

void BackgroundMaterial::set_cameraColorGradingTexture(const BaseTexturePtr& value)
{
  _imageProcessingConfiguration->colorGradingTexture = value;
}

ColorCurvesPtr& BackgroundMaterial::get_cameraColorCurves()
{
  return _imageProcessingConfiguration->colorCurves;
}

void BackgroundMaterial::set_cameraColorCurves(const ColorCurvesPtr& value)
{
  _imageProcessingConfiguration->colorCurves = std::make_shared<ColorCurves>(*value);
}

bool BackgroundMaterial::get_hasRenderTargetTextures() const
{
  if (_diffuseTexture && _diffuseTexture->isRenderTarget) {
    return true;
  }

  if (_reflectionTexture && _reflectionTexture->isRenderTarget) {
    return true;
  }

  return false;
}

bool BackgroundMaterial::needAlphaTesting() const
{
  return true;
}

bool BackgroundMaterial::needAlphaBlending() const
{
  return ((alpha() < 1.f) || (_diffuseTexture != nullptr && _diffuseTexture->hasAlpha())
          || _shadowOnly);
}

bool BackgroundMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances)
{
  if (subMesh->effect() && isFrozen()) {
    if (subMesh->effect()->_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<BackgroundMaterialDefines>();
  }

  auto scene      = getScene();
  auto definesPtr = std::static_pointer_cast<BackgroundMaterialDefines>(subMesh->_materialDefines);
  auto& defines   = *definesPtr;

  if (_isReadyForSubMesh(subMesh)) {
    return true;
  }

  auto engine = scene->getEngine();

  // Lights
  MaterialHelper::PrepareDefinesForLights(scene, mesh, defines, false, _maxSimultaneousLights);
  defines._needNormals = true;

  // Multiview
  MaterialHelper::PrepareDefinesForMultiview(scene, defines);

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (scene->getEngine()->getCaps().textureLOD) {
        defines.boolDef["TEXTURELODSUPPORT"] = true;
      }

      if (_diffuseTexture && MaterialFlags::DiffuseTextureEnabled()) {
        if (!_diffuseTexture->isReadyOrNotBlocking()) {
          return false;
        }

        MaterialHelper::PrepareDefinesForMergedUV(_diffuseTexture, defines, "DIFFUSE");
        defines.boolDef["DIFFUSEHASALPHA"] = _diffuseTexture->hasAlpha();
        defines.boolDef["GAMMADIFFUSE"]    = _diffuseTexture->gammaSpace;
        defines.boolDef["OPACITYFRESNEL"]  = _opacityFresnel;
      }
      else {
        defines.boolDef["DIFFUSE"]         = false;
        defines.boolDef["DIFFUSEHASALPHA"] = false;
        defines.boolDef["GAMMADIFFUSE"]    = false;
        defines.boolDef["OPACITYFRESNEL"]  = false;
      }

      auto iReflectionTexture = _reflectionTexture;
      if (iReflectionTexture && MaterialFlags::ReflectionTextureEnabled()) {
        if (!iReflectionTexture->isReadyOrNotBlocking()) {
          return false;
        }

        defines.boolDef["REFLECTION"]                     = true;
        defines.boolDef["GAMMAREFLECTION"]                = iReflectionTexture->gammaSpace;
        defines.boolDef["RGBDREFLECTION"]                 = iReflectionTexture->isRGBD();
        defines.boolDef["REFLECTIONBLUR"]                 = _reflectionBlur > 0.f;
        defines.boolDef["REFLECTIONMAP_OPPOSITEZ"]        = getScene()->useRightHandedSystem() ?
                                                              !iReflectionTexture->invertZ :
                                                              iReflectionTexture->invertZ;
        defines.boolDef["LODINREFLECTIONALPHA"]           = iReflectionTexture->lodLevelInAlpha;
        defines.boolDef["EQUIRECTANGULAR_RELFECTION_FOV"] = useEquirectangularFOV;
        defines.boolDef["REFLECTIONBGR"]                  = switchToBGR;

        if (iReflectionTexture->coordinatesMode == TextureConstants::INVCUBIC_MODE) {
          defines.boolDef["INVERTCUBICMAP"] = true;
        }

        defines.boolDef["REFLECTIONMAP_3D"] = iReflectionTexture->isCube;

        switch (iReflectionTexture->coordinatesMode) {
          case TextureConstants::EXPLICIT_MODE:
            defines.boolDef["REFLECTIONMAP_EXPLICIT"] = true;
            break;
          case TextureConstants::PLANAR_MODE:
            defines.boolDef["REFLECTIONMAP_PLANAR"] = true;
            break;
          case TextureConstants::PROJECTION_MODE:
            defines.boolDef["REFLECTIONMAP_PROJECTION"] = true;
            break;
          case TextureConstants::SKYBOX_MODE:
            defines.boolDef["REFLECTIONMAP_SKYBOX"] = true;
            break;
          case TextureConstants::SPHERICAL_MODE:
            defines.boolDef["REFLECTIONMAP_SPHERICAL"] = true;
            break;
          case TextureConstants::EQUIRECTANGULAR_MODE:
            defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR"] = true;
            break;
          case TextureConstants::FIXED_EQUIRECTANGULAR_MODE:
            defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR_FIXED"] = true;
            break;
          case TextureConstants::FIXED_EQUIRECTANGULAR_MIRRORED_MODE:
            defines.boolDef["REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED"] = true;
            break;
          case TextureConstants::CUBIC_MODE:
          case TextureConstants::INVCUBIC_MODE:
          default:
            defines.boolDef["REFLECTIONMAP_CUBIC"] = true;
            break;
        }

        if (reflectionFresnel()) {
          defines.boolDef["REFLECTIONFRESNEL"] = true;
          defines.boolDef["REFLECTIONFALLOFF"] = reflectionFalloffDistance() > 0.f;

          _reflectionControls.x = reflectionAmount();
          _reflectionControls.y = reflectionReflectance0();
          _reflectionControls.z = reflectionReflectance90();
          _reflectionControls.w = 1.f / reflectionFalloffDistance();
        }
        else {
          defines.boolDef["REFLECTIONFRESNEL"] = false;
          defines.boolDef["REFLECTIONFALLOFF"] = false;
        }
      }
      else {
        defines.boolDef["REFLECTION"]                                  = false;
        defines.boolDef["REFLECTIONFRESNEL"]                           = false;
        defines.boolDef["REFLECTIONFALLOFF"]                           = false;
        defines.boolDef["REFLECTIONBLUR"]                              = false;
        defines.boolDef["REFLECTIONMAP_3D"]                            = false;
        defines.boolDef["REFLECTIONMAP_SPHERICAL"]                     = false;
        defines.boolDef["REFLECTIONMAP_PLANAR"]                        = false;
        defines.boolDef["REFLECTIONMAP_CUBIC"]                         = false;
        defines.boolDef["REFLECTIONMAP_PROJECTION"]                    = false;
        defines.boolDef["REFLECTIONMAP_SKYBOX"]                        = false;
        defines.boolDef["REFLECTIONMAP_EXPLICIT"]                      = false;
        defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR"]               = false;
        defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR_FIXED"]         = false;
        defines.boolDef["REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED"] = false;
        defines.boolDef["INVERTCUBICMAP"]                              = false;
        defines.boolDef["REFLECTIONMAP_OPPOSITEZ"]                     = false;
        defines.boolDef["LODINREFLECTIONALPHA"]                        = false;
        defines.boolDef["GAMMAREFLECTION"]                             = false;
        defines.boolDef["RGBDREFLECTION"]                              = false;
      }
    }

    defines.boolDef["PREMULTIPLYALPHA"]
      = (alphaMode() == Constants::ALPHA_PREMULTIPLIED
         || alphaMode() == Constants::ALPHA_PREMULTIPLIED_PORTERDUFF);
    defines.boolDef["USERGBCOLOR"] = _useRGBColor;
    defines.boolDef["NOISE"]       = _enableNoise;
  }

  if (defines._areLightsDirty) {
    defines.boolDef["USEHIGHLIGHTANDSHADOWCOLORS"]
      = !_useRGBColor && (_primaryColorShadowLevel != 0.f || _primaryColorHighlightLevel != 0.f);
    defines.boolDef["BACKMAT_SHADOWONLY"] = _shadowOnly;
  }

  if (defines._areImageProcessingDirty && _imageProcessingConfiguration) {
    if (!_imageProcessingConfiguration->isReady()) {
      return false;
    }

    _imageProcessingConfiguration->prepareDefines(defines);
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(), fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, std::nullopt,
    subMesh->getRenderingMesh()->hasThinInstances());

  // Attribs
  if (MaterialHelper::PrepareDefinesForAttributes(mesh, defines, false, true, false)) {
    if (mesh) {
      if (!scene->getEngine()->getCaps().standardDerivatives
          && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
        mesh->createNormals(true);
        BABYLON_LOGF_WARN("BackgroundMaterial",
                          "BackgroundMaterial: Normals have been created for the mesh: %s",
                          mesh->name.c_str())
      }
    }
  }

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines["FOG"]) {
      fallbacks->addFallback(0, "FOG");
    }

    if (defines["POINTSIZE"]) {
      fallbacks->addFallback(1, "POINTSIZE");
    }

    if (defines["MULTIVIEW"]) {
      fallbacks->addFallback(0, "MULTIVIEW");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks, _maxSimultaneousLights);

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKind};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKind);
    }

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKind);
    }

    if (defines["UV2"]) {
      attribs.emplace_back(VertexBuffer::UV2Kind);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines, *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    std::vector<std::string> uniforms{"world",
                                      "view",
                                      "viewProjection",
                                      "vEyePosition",
                                      "vLightsType",
                                      "vFogInfos",
                                      "vFogColor",
                                      "pointSize",
                                      "vClipPlane",
                                      "vClipPlane2",
                                      "vClipPlane3",
                                      "vClipPlane4",
                                      "vClipPlane5",
                                      "vClipPlane6",
                                      "mBones",
                                      "vPrimaryColor",
                                      "vPrimaryColorShadow",
                                      "vReflectionInfos",
                                      "reflectionMatrix",
                                      "vReflectionMicrosurfaceInfos",
                                      "fFovMultiplier",
                                      "shadowLevel",
                                      "alpha",
                                      "vBackgroundCenter",
                                      "vReflectionControl",
                                      "vDiffuseInfos",
                                      "diffuseMatrix"};
    std::vector<std::string> samplers{"diffuseSampler", "reflectionSampler", "reflectionSamplerLow",
                                      "reflectionSamplerHigh"};
    std::vector<std::string> uniformBuffers{"Material", "Scene"};

    // if (ImageProcessingConfiguration)
    {
      ImageProcessingConfiguration::PrepareUniforms(uniforms, defines);
      ImageProcessingConfiguration::PrepareSamplers(samplers, defines);
    }

    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", _maxSimultaneousLights}};

    std::function<void(Effect * effect)> iOnCompiled = [this](Effect* effect) {
      if (onCompiled) {
        onCompiled(effect);
      }

      MaterialHelper::BindSceneUniformBuffer(effect, getScene()->getSceneUniformBuffer());
    };

    auto join = defines.toString();

    IEffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.uniformBuffersNames   = std::move(uniformBuffers);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = iOnCompiled;
    options.onError               = onError;
    options.indexParameters       = std::move(indexParameters);
    options.maxSimultaneousLights = _maxSimultaneousLights;

    MaterialHelper::PrepareUniformsAndSamplersList(options);

    subMesh->setEffect(scene->getEngine()->createEffect("background", options, engine), definesPtr);

    buildUniformLayout();
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  defines._renderId                      = scene->getRenderId();
  subMesh->effect()->_wasPreviouslyReady = true;

  return true;
}

void BackgroundMaterial::_computePrimaryColorFromPerceptualColor()
{
  if (!__perceptualColor) {
    return;
  }

  _primaryColor.copyFrom(*__perceptualColor);

  // Revert gamma space.
  _primaryColor.toLinearSpaceToRef(_primaryColor);

  // Revert image processing configuration.
  if (_imageProcessingConfiguration) {
    // Revert Exposure.
    _primaryColor.scaleToRef(1.f / _imageProcessingConfiguration->exposure(), _primaryColor);
  }

  _computePrimaryColors();
}

void BackgroundMaterial::_computePrimaryColors()
{
  if (_primaryColorShadowLevel == 0.f && _primaryColorHighlightLevel == 0.f) {
    return;
  }

  // Find the highlight color based on the configuration.
  _primaryColor.scaleToRef(_primaryColorShadowLevel, _primaryShadowColor);
  _primaryColor.subtractToRef(_primaryShadowColor, _primaryShadowColor);

  // Find the shadow color based on the configuration.
  _white.subtractToRef(_primaryColor, _primaryHighlightColor);
  _primaryHighlightColor.scaleToRef(_primaryColorHighlightLevel, _primaryHighlightColor);
  _primaryColor.addToRef(_primaryHighlightColor, _primaryHighlightColor);
}

void BackgroundMaterial::buildUniformLayout()
{
  // Order is important !
  _uniformBuffer->addUniform("vPrimaryColor", 4);
  _uniformBuffer->addUniform("vPrimaryColorShadow", 4);
  _uniformBuffer->addUniform("vDiffuseInfos", 2);
  _uniformBuffer->addUniform("vReflectionInfos", 2);
  _uniformBuffer->addUniform("diffuseMatrix", 16);
  _uniformBuffer->addUniform("reflectionMatrix", 16);
  _uniformBuffer->addUniform("vReflectionMicrosurfaceInfos", 3);
  _uniformBuffer->addUniform("fFovMultiplier", 1);
  _uniformBuffer->addUniform("pointSize", 1);
  _uniformBuffer->addUniform("shadowLevel", 1);
  _uniformBuffer->addUniform("alpha", 1);
  _uniformBuffer->addUniform("vBackgroundCenter", 3);
  _uniformBuffer->addUniform("vReflectionControl", 4);

  _uniformBuffer->create();
}

void BackgroundMaterial::unbind()
{
  if (_diffuseTexture && _diffuseTexture->isRenderTarget) {
    _uniformBuffer->setTexture("diffuseSampler", nullptr);
  }

  if (_reflectionTexture && _reflectionTexture->isRenderTarget) {
    _uniformBuffer->setTexture("reflectionSampler", nullptr);
  }

  PushMaterial::unbind();
}

void BackgroundMaterial::bindOnlyWorldMatrix(Matrix& world, const EffectPtr& /*effectOverride*/)
{
  _activeEffect->setMatrix("world", world);
}

void BackgroundMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto definesTmp = static_cast<BackgroundMaterialDefines*>(subMesh->_materialDefines.get());
  if (!definesTmp) {
    return;
  }
  auto defines = *definesTmp;

  auto effect = subMesh->effect();
  if (!effect) {
    return;
  }
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(world);

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect.get());

  auto mustRebind = _mustRebind(scene, effect, mesh->visibility());
  if (mustRebind) {
    _uniformBuffer->bindToEffect(effect.get(), "Material");

    bindViewProjection(effect);

    auto iReflectionTexture = _reflectionTexture;
    if (!_uniformBuffer->useUbo() || !isFrozen() || !_uniformBuffer->isSync()) {

      // Texture uniforms
      if (scene->texturesEnabled()) {
        if (_diffuseTexture && MaterialFlags::DiffuseTextureEnabled()) {
          _uniformBuffer->updateFloat2("vDiffuseInfos",
                                       static_cast<float>(_diffuseTexture->coordinatesIndex),
                                       _diffuseTexture->level, "");
          MaterialHelper::BindTextureMatrix(*_diffuseTexture, *_uniformBuffer, "diffuse");
        }

        if (iReflectionTexture && MaterialFlags::ReflectionTextureEnabled()) {
          _uniformBuffer->updateMatrix("reflectionMatrix",
                                       *iReflectionTexture->getReflectionTextureMatrix());
          _uniformBuffer->updateFloat2("vReflectionInfos", iReflectionTexture->level,
                                       _reflectionBlur, "");

          _uniformBuffer->updateFloat3(
            "vReflectionMicrosurfaceInfos", static_cast<float>(iReflectionTexture->getSize().width),
            iReflectionTexture->lodGenerationScale, iReflectionTexture->lodGenerationOffset, "");
        }
      }

      if (shadowLevel() > 0) {
        _uniformBuffer->updateFloat("shadowLevel", shadowLevel());
      }
      _uniformBuffer->updateFloat("alpha", alpha());

      // Point size
      if (pointsCloud()) {
        _uniformBuffer->updateFloat("pointSize", pointSize);
      }

      if (defines.boolDef["USEHIGHLIGHTANDSHADOWCOLORS"]) {
        _uniformBuffer->updateColor4("vPrimaryColor", _primaryHighlightColor, 1.f, "");
        _uniformBuffer->updateColor4("vPrimaryColorShadow", _primaryShadowColor, 1.f, "");
      }
      else {
        _uniformBuffer->updateColor4("vPrimaryColor", _primaryColor, 1.f, "");
      }
    }

    _uniformBuffer->updateFloat("fFovMultiplier", _fovMultiplier);

    // Textures
    if (scene->texturesEnabled()) {
      if (_diffuseTexture && MaterialFlags::DiffuseTextureEnabled()) {
        _uniformBuffer->setTexture("diffuseSampler", _diffuseTexture);
      }

      if (iReflectionTexture && MaterialFlags::ReflectionTextureEnabled()) {
        if (defines["REFLECTIONBLUR"] && defines["TEXTURELODSUPPORT"]) {
          _uniformBuffer->setTexture("reflectionSampler", iReflectionTexture);
        }
        else if (!defines["REFLECTIONBLUR"]) {
          _uniformBuffer->setTexture("reflectionSampler", iReflectionTexture);
        }
        else {
          _uniformBuffer->setTexture("reflectionSampler", iReflectionTexture->_lodTextureMid() ?
                                                            iReflectionTexture->_lodTextureMid() :
                                                            iReflectionTexture);
          _uniformBuffer->setTexture("reflectionSamplerLow",
                                     iReflectionTexture->_lodTextureLow() ?
                                       iReflectionTexture->_lodTextureLow() :
                                       iReflectionTexture);
          _uniformBuffer->setTexture("reflectionSamplerHigh",
                                     iReflectionTexture->_lodTextureHigh() ?
                                       iReflectionTexture->_lodTextureHigh() :
                                       iReflectionTexture);
        }

        if (defines["REFLECTIONFRESNEL"]) {
          _uniformBuffer->updateFloat3("vBackgroundCenter", sceneCenter().x, sceneCenter().y,
                                       sceneCenter().z, "");
          _uniformBuffer->updateFloat4("vReflectionControl", _reflectionControls.x,
                                       _reflectionControls.y, _reflectionControls.z,
                                       _reflectionControls.w, "");
        }
      }
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    MaterialHelper::BindEyePosition(effect.get(), scene);
  }

  if (mustRebind || !isFrozen()) {
    if (scene->lightsEnabled()) {
      MaterialHelper::BindLights(scene, mesh, _activeEffect.get(), defines, _maxSimultaneousLights,
                                 false);
    }

    // View
    bindView(effect.get());

    // Fog
    MaterialHelper::BindFogParameters(scene, mesh, _activeEffect, true);

    // image processing
    if (_imageProcessingConfiguration) {
      _imageProcessingConfiguration->bind(_activeEffect.get());
    }
  }

  Material::_afterBind(mesh /*, _activeEffect*/);

  _uniformBuffer->update();
}

bool BackgroundMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  if (_reflectionTexture == texture) {
    return true;
  }

  if (_diffuseTexture == texture) {
    return true;
  }

  return false;
}

void BackgroundMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                                 bool /*notBoundToMesh*/)
{
  if (forceDisposeTextures) {
    if (diffuseTexture()) {
      diffuseTexture()->dispose();
    }
    if (reflectionTexture()) {
      reflectionTexture()->dispose();
    }
  }

  _renderTargets.clear();

  if (_imageProcessingConfiguration && _imageProcessingObserver) {
    _imageProcessingConfiguration->onUpdateParameters.remove(_imageProcessingObserver);
  }

  PushMaterial::dispose(forceDisposeEffect);
}

MaterialPtr BackgroundMaterial::clone(const std::string& /*name*/, bool /*cloneChildren*/) const
{
  return nullptr;
}

json BackgroundMaterial::serialize() const
{
  return nullptr;
}

std::string BackgroundMaterial::getClassName() const
{
  return "BackgroundMaterial";
}

std::unique_ptr<BackgroundMaterial>
BackgroundMaterial::Parse(const json& /*source*/, Scene* /*scene*/, const std::string& /*url*/)
{
  return nullptr;
}

} // end of namespace BABYLON
