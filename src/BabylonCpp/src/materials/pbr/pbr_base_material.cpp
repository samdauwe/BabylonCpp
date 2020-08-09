#include <babylon/materials/pbr/pbr_base_material.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/detail_map_configuration.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/pbr/imaterial_anisotropic_defines.h>
#include <babylon/materials/pbr/imaterial_brdf_defines.h>
#include <babylon/materials/pbr/imaterial_clear_coat_defines.h>
#include <babylon/materials/pbr/imaterial_sheen_defines.h>
#include <babylon/materials/pbr/imaterial_sub_surface_defines.h>
#include <babylon/materials/pbr/pbr_anisotropic_configuration.h>
#include <babylon/materials/pbr/pbr_brdf_configuration.h>
#include <babylon/materials/pbr/pbr_clear_coat_configuration.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/pbr/pbr_material_defines.h>
#include <babylon/materials/pbr/pbr_sheen_configuration.h>
#include <babylon/materials/pbr/pbr_sub_surface_configuration.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/refraction_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/spherical_polynomial.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/brdf_texture_tools.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

PBRBaseMaterial::PBRBaseMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , realTimeFiltering{this, &PBRBaseMaterial::get_realTimeFiltering,
                        &PBRBaseMaterial::set_realTimeFiltering}
    , realTimeFilteringQuality{this, &PBRBaseMaterial::get_realTimeFilteringQuality,
                               &PBRBaseMaterial::set_realTimeFilteringQuality}
    , transparencyMode{this, &PBRBaseMaterial::get_transparencyMode,
                       &PBRBaseMaterial::set_transparencyMode}
    , debugMode{this, &PBRBaseMaterial::get_debugMode, &PBRBaseMaterial::set_debugMode}
    , clearCoat{std::make_shared<PBRClearCoatConfiguration>(
        [this]() -> void { _markAllSubMeshesAsTexturesDirty(); })}
    , anisotropy{std::make_shared<PBRAnisotropicConfiguration>(
        [this]() -> void { _markAllSubMeshesAsTexturesDirty(); })}
    , brdf{std::make_shared<PBRBRDFConfiguration>(
        [this]() -> void { _markAllSubMeshesAsMiscDirty(); })}
    , sheen{std::make_shared<PBRSheenConfiguration>(
        [this]() -> void { _markAllSubMeshesAsTexturesDirty(); })}
    , detailMap{std::make_shared<DetailMapConfiguration>(
        [this]() -> void { _markAllSubMeshesAsTexturesDirty(); })}
    , _directIntensity{1.f}
    , _emissiveIntensity{1.f}
    , _environmentIntensity{1.f}
    , _specularIntensity{1.f}
    , _disableBumpMap{false}
    , _albedoTexture{nullptr}
    , _ambientTexture{nullptr}
    , _ambientTextureStrength{1.f}
    , _ambientTextureImpactOnAnalyticalLights{PBRBaseMaterial::DEFAULT_AO_ON_ANALYTICAL_LIGHTS}
    , _opacityTexture{nullptr}
    , _reflectionTexture{nullptr}
    , _emissiveTexture{nullptr}
    , _reflectivityTexture{nullptr}
    , _metallicTexture{nullptr}
    , _metallic{std::nullopt}
    , _roughness{std::nullopt}
    , _metallicF0Factor{1.f}
    , _metallicReflectanceColor{Color3::White()}
    , _metallicReflectanceTexture{nullptr}
    , _microSurfaceTexture{nullptr}
    , _bumpTexture{nullptr}
    , _lightmapTexture{nullptr}
    , _ambientColor{Color3(0, 0, 0)}
    , _albedoColor{Color3(1, 1, 1)}
    , _reflectivityColor{Color3(1, 1, 1)}
    , _reflectionColor{Color3(1, 1, 1)}
    , _emissiveColor{Color3(0, 0, 0)}
    , _microSurface{0.9f}
    , _useLightmapAsShadowmap{false}
    , _useHorizonOcclusion{true}
    , _useRadianceOcclusion{true}
    , _useAlphaFromAlbedoTexture{false}
    , _useSpecularOverAlpha{true}
    , _useMicroSurfaceFromReflectivityMapAlpha{false}
    , _useRoughnessFromMetallicTextureAlpha{true}
    , _useRoughnessFromMetallicTextureGreen{false}
    , _useMetallnessFromMetallicTextureBlue{false}
    , _useAmbientOcclusionFromMetallicTextureRed{false}
    , _useAmbientInGrayScale{false}
    , _useAutoMicroSurfaceFromReflectivityMap{false}
    , _lightFalloff{PBRBaseMaterial::LIGHTFALLOFF_PHYSICAL}
    , _useRadianceOverAlpha{true}
    , _useObjectSpaceNormalMap{false}
    , _useParallax{false}
    , _useParallaxOcclusion{false}
    , _parallaxScaleBias{0.05f}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _invertNormalMapX{false}
    , _invertNormalMapY{false}
    , _twoSidedLighting{false}
    , _alphaCutOff{0.4f}
    , _forceAlphaTest{false}
    , _useAlphaFresnel{false}
    , _useLinearAlphaFresnel{false}
    , _environmentBRDFTexture{nullptr}
    , _forceIrradianceInFragment{false}
    , _forceNormalForward{false}
    , _enableSpecularAntiAliasing{false}
    , _imageProcessingConfiguration{std::make_shared<ImageProcessingConfiguration>()}
    , _unlit{false}
    , _rebuildInParallel{false}
    , _lightingInfos{Vector4(_directIntensity, _emissiveIntensity, _environmentIntensity,
                             _specularIntensity)}
    , _realTimeFiltering{false}
    , _realTimeFilteringQuality{Constants::TEXTURE_FILTERING_QUALITY_LOW}
    , _imageProcessingObserver{nullptr}
    , _globalAmbientColor{Color3(0.f, 0.f, 0.f)}
    , _useLogarithmicDepth{false}
    , _debugMode{0}
    , debugLimit{-1.f}
    , debugFactor{1.f}
{
  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);

  getRenderTargetTextures = [this]() -> std::vector<RenderTargetTexturePtr> {
    _renderTargets.clear();

    if (MaterialFlags::ReflectionTextureEnabled() && _reflectionTexture
        && _reflectionTexture->isRenderTarget) {
      _renderTargets.emplace_back(
        std::static_pointer_cast<RenderTargetTexture>(_reflectionTexture));
    }

    subSurface->fillRenderTargetTextures(_renderTargets);

    return _renderTargets;
  };

  _environmentBRDFTexture = BRDFTextureTools::GetEnvironmentBRDFTexture(scene);
  subSurface              = std::make_shared<PBRSubSurfaceConfiguration>(
    [this]() -> void { _markAllSubMeshesAsTexturesDirty(); },
    [this]() -> void { _markScenePrePassDirty(); }, scene);
}

PBRBaseMaterial::~PBRBaseMaterial() = default;

void PBRBaseMaterial::_attachImageProcessingConfiguration(
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
      [this](ImageProcessingConfiguration* /*conf*/, EventState&) {
        _markAllSubMeshesAsImageProcessingDirty();
      });
  }
}

unsigned int PBRBaseMaterial::get_debugMode() const
{
  return _debugMode;
}

void PBRBaseMaterial::set_debugMode(unsigned int value)
{
  if (_debugMode == value) {
    return;
  }

  _markAllSubMeshesAsMiscDirty();
  _debugMode = value;
}

bool PBRBaseMaterial::get_realTimeFiltering() const
{
  return _realTimeFiltering;
}

void PBRBaseMaterial::set_realTimeFiltering(bool b)
{
  _realTimeFiltering = b;
  markAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

unsigned int PBRBaseMaterial::get_realTimeFilteringQuality() const
{
  return _realTimeFilteringQuality;
}

void PBRBaseMaterial::set_realTimeFilteringQuality(unsigned int n)
{
  _realTimeFilteringQuality = n;
  markAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool PBRBaseMaterial::get_canRenderToMRT() const
{
  return true;
}

bool PBRBaseMaterial::get_hasRenderTargetTextures() const
{
  if (MaterialFlags::ReflectionTextureEnabled() && _reflectionTexture
      && _reflectionTexture->isRenderTarget) {
    return true;
  }

  return subSurface->hasRenderTargetTextures();
}

std::string PBRBaseMaterial::getClassName() const
{
  return "PBRBaseMaterial";
}

bool PBRBaseMaterial::get_useLogarithmicDepth() const
{
  return _useLogarithmicDepth;
}

void PBRBaseMaterial::set_useLogarithmicDepth(bool value)
{
  _useLogarithmicDepth = value && getScene()->getEngine()->getCaps().fragmentDepthSupported;
}

bool PBRBaseMaterial::_disableAlphaBlending() const
{
  return (subSurface->disableAlphaBlending()
          || *_transparencyMode == PBRBaseMaterial::PBRMATERIAL_OPAQUE
          || *_transparencyMode == PBRBaseMaterial::PBRMATERIAL_ALPHATEST);
}

bool PBRBaseMaterial::needAlphaBlending() const
{
  if (_disableAlphaBlending()) {
    return false;
  }
  return (alpha() < 1.f) || (_opacityTexture != nullptr) || _shouldUseAlphaFromAlbedoTexture();
}

bool PBRBaseMaterial::needAlphaTesting() const
{
  if (_forceAlphaTest) {
    return true;
  }

  if (subSurface->disableAlphaBlending()) {
    return false;
  }

  return _albedoTexture != nullptr && _albedoTexture->hasAlpha()
         && (!_transparencyMode.has_value()
             || *_transparencyMode == PBRBaseMaterial::PBRMATERIAL_ALPHATEST);
}

bool PBRBaseMaterial::_shouldUseAlphaFromAlbedoTexture() const
{
  return _albedoTexture != nullptr && _albedoTexture->hasAlpha() && _useAlphaFromAlbedoTexture
         && *_transparencyMode != PBRBaseMaterial::PBRMATERIAL_OPAQUE;
}

BaseTexturePtr PBRBaseMaterial::getAlphaTestTexture()
{
  return _albedoTexture;
}

bool PBRBaseMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances)
{
  if (subMesh->effect() && isFrozen()) {
    if (subMesh->effect()->_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<PBRMaterialDefines>();
  }

  auto scene      = getScene();
  auto definesPtr = std::static_pointer_cast<PBRMaterialDefines>(subMesh->_materialDefines);
  auto& defines   = *definesPtr;
  if (_isReadyForSubMesh(subMesh)) {
    return true;
  }

  auto engine = scene->getEngine();

  if (defines._areTexturesDirty) {
    if (scene->texturesEnabled()) {
      if (_albedoTexture && MaterialFlags::DiffuseTextureEnabled()) {
        if (!_albedoTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_ambientTexture && MaterialFlags::AmbientTextureEnabled()) {
        if (!_ambientTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_opacityTexture && MaterialFlags::OpacityTextureEnabled()) {
        if (!_opacityTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      auto reflectionTexture = _getReflectionTexture();
      if (reflectionTexture && MaterialFlags::ReflectionTextureEnabled()) {
        if (!reflectionTexture->isReadyOrNotBlocking()) {
          return false;
        }
        if (reflectionTexture->irradianceTexture()
            && !reflectionTexture->irradianceTexture()->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_lightmapTexture && MaterialFlags::LightmapTextureEnabled()) {
        if (!_lightmapTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_emissiveTexture && MaterialFlags::EmissiveTextureEnabled()) {
        if (!_emissiveTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (MaterialFlags::SpecularTextureEnabled()) {
        if (_metallicTexture) {
          if (!_metallicTexture->isReadyOrNotBlocking()) {
            return false;
          }
        }
        else if (_reflectivityTexture) {
          if (!_reflectivityTexture->isReadyOrNotBlocking()) {
            return false;
          }
        }

        if (_metallicReflectanceTexture) {
          if (!_metallicReflectanceTexture->isReadyOrNotBlocking()) {
            return false;
          }
        }

        if (_microSurfaceTexture) {
          if (!_microSurfaceTexture->isReadyOrNotBlocking()) {
            return false;
          }
        }
      }

      if (engine->getCaps().standardDerivatives && _bumpTexture
          && MaterialFlags::BumpTextureEnabled() && !_disableBumpMap) {
        // Bump texture cannot be not blocking.
        if (!_bumpTexture->isReady()) {
          return false;
        }
      }

      if (_environmentBRDFTexture && MaterialFlags::ReflectionTextureEnabled()) {
        // This is blocking.
        if (!_environmentBRDFTexture->isReady()) {
          return false;
        }
      }
    }
  }

  if (!subSurface->isReadyForSubMesh(defines, scene)
      || !clearCoat->isReadyForSubMesh(defines, scene, engine, _disableBumpMap)
      || !sheen->isReadyForSubMesh(defines, scene) || !anisotropy->isReadyForSubMesh(defines, scene)
      || !detailMap->isReadyForSubMesh(defines, scene)) {
    return false;
  }

  if (defines._areImageProcessingDirty && _imageProcessingConfiguration) {
    if (!_imageProcessingConfiguration->isReady()) {
      return false;
    }
  }

  if (!engine->getCaps().standardDerivatives
      && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
    mesh->createNormals(true);
    BABYLON_LOGF_WARN("PBRBaseMaterial", "PBRMaterial: Normals have been created for the mesh: %s",
                      mesh->name.c_str())
  }

  auto& previousEffect     = subMesh->effect();
  const auto lightDisposed = defines._areLightsDisposed;
  auto effect = _prepareEffect(mesh, defines, onCompiled, onError, useInstances, std::nullopt,
                               subMesh->getRenderingMesh()->hasThinInstances());

  if (effect) {
    /* if (_onEffectCreatedObservable) */ {
      onCreatedEffectParameters.effect  = effect.get();
      onCreatedEffectParameters.subMesh = subMesh;
      _onEffectCreatedObservable.notifyObservers(&onCreatedEffectParameters);
    }

    // Use previous effect while new one is compiling
    if (allowShaderHotSwapping && previousEffect && !effect->isReady()) {
      effect             = previousEffect;
      _rebuildInParallel = true;
      defines.markAsUnprocessed();

      if (lightDisposed) {
        // re register in case it takes more than one frame.
        defines._areLightsDisposed = true;
        return false;
      }
    }
    else {
      _rebuildInParallel = false;
      scene->resetCachedMaterial();
      subMesh->setEffect(effect, definesPtr);
      buildUniformLayout();
    }
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  defines._renderId                      = scene->getRenderId();
  subMesh->effect()->_wasPreviouslyReady = true;

  return true;
}

bool PBRBaseMaterial::isMetallicWorkflow() const
{
  return (_metallic.has_value() && _metallic.value() != 0.f)

         || (_roughness.has_value() && _roughness.value() != 0.f)

         || _metallicTexture;
}

EffectPtr PBRBaseMaterial::_prepareEffect(
  AbstractMesh* mesh, PBRMaterialDefines& defines,
  const std::function<void(Effect* effect)>& iOnCompiled,
  std::function<void(Effect* effect, const std::string& errors)> iOnError,
  const std::optional<bool>& useInstances, const std::optional<bool>& useClipPlane,
  bool useThinInstances)
{
  _prepareDefines(mesh, defines, useInstances, useClipPlane, useThinInstances);
  if (!defines.isDirty()) {
    return nullptr;
  }

  defines.markAsProcessed();

  auto scene  = getScene();
  auto engine = scene->getEngine();

  // Fallbacks
  auto fallbacks    = std::make_unique<EffectFallbacks>();
  auto fallbackRank = 0u;
  if (defines["USESPHERICALINVERTEX"]) {
    fallbacks->addFallback(fallbackRank++, "USESPHERICALINVERTEX");
  }

  if (defines["FOG"]) {
    fallbacks->addFallback(fallbackRank, "FOG");
  }
  if (defines["SPECULARAA"]) {
    fallbacks->addFallback(fallbackRank, "SPECULARAA");
  }
  if (defines["POINTSIZE"]) {
    fallbacks->addFallback(fallbackRank, "POINTSIZE");
  }
  if (defines["LOGARITHMICDEPTH"]) {
    fallbacks->addFallback(fallbackRank, "LOGARITHMICDEPTH");
  }
  if (defines["PARALLAX"]) {
    fallbacks->addFallback(fallbackRank, "PARALLAX");
  }
  if (defines["PARALLAXOCCLUSION"]) {
    fallbacks->addFallback(fallbackRank++, "PARALLAXOCCLUSION");
  }

  fallbackRank = PBRAnisotropicConfiguration::AddFallbacks(defines, *fallbacks, fallbackRank);
  fallbackRank = PBRAnisotropicConfiguration::AddFallbacks(defines, *fallbacks, fallbackRank);
  fallbackRank = PBRSubSurfaceConfiguration::AddFallbacks(defines, *fallbacks, fallbackRank);
  fallbackRank = PBRSheenConfiguration::AddFallbacks(defines, *fallbacks, fallbackRank);

  if (defines["ENVIRONMENTBRDF"]) {
    fallbacks->addFallback(fallbackRank++, "ENVIRONMENTBRDF");
  }

  if (defines["TANGENT"]) {
    fallbacks->addFallback(fallbackRank++, "TANGENT");
  }

  if (defines["BUMP"]) {
    fallbacks->addFallback(fallbackRank++, "BUMP");
  }

  fallbackRank = MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                                           _maxSimultaneousLights, fallbackRank++);

  if (defines["SPECULARTERM"]) {
    fallbacks->addFallback(fallbackRank++, "SPECULARTERM");
  }

  if (defines["USESPHERICALFROMREFLECTIONMAP"]) {
    fallbacks->addFallback(fallbackRank++, "USESPHERICALFROMREFLECTIONMAP");
  }

  if (defines["USEIRRADIANCEMAP"]) {
    fallbacks->addFallback(fallbackRank++, "USEIRRADIANCEMAP");
  }

  if (defines["LIGHTMAP"]) {
    fallbacks->addFallback(fallbackRank++, "LIGHTMAP");
  }

  if (defines["NORMAL"]) {
    fallbacks->addFallback(fallbackRank++, "NORMAL");
  }

  if (defines["AMBIENT"]) {
    fallbacks->addFallback(fallbackRank++, "AMBIENT");
  }

  if (defines["EMISSIVE"]) {
    fallbacks->addFallback(fallbackRank++, "EMISSIVE");
  }

  if (defines["VERTEXCOLOR"]) {
    fallbacks->addFallback(fallbackRank++, "VERTEXCOLOR");
  }

  if (defines["MORPHTARGETS"]) {
    fallbacks->addFallback(fallbackRank++, "MORPHTARGETS");
  }

  if (defines["MULTIVIEW"]) {
    fallbacks->addFallback(0, "MULTIVIEW");
  }

  // Attributes
  std::vector<std::string> attribs{VertexBuffer::PositionKind};

  if (defines["NORMAL"]) {
    attribs.emplace_back(VertexBuffer::NormalKind);
  }

  if (defines["TANGENT"]) {
    attribs.emplace_back(VertexBuffer::TangentKind);
  }

  if (defines["UV1"]) {
    attribs.emplace_back(VertexBuffer::UVKind);
  }

  if (defines["UV2"]) {
    attribs.emplace_back(VertexBuffer::UV2Kind);
  }

  if (defines["VERTEXCOLOR"]) {
    attribs.emplace_back(VertexBuffer::ColorKind);
  }

  MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines, *fallbacks);
  MaterialHelper::PrepareAttributesForInstances(attribs, defines);
  MaterialHelper::PrepareAttributesForMorphTargets(attribs, mesh, defines);

  std::string shaderName = "pbr";

  std::vector<std::string> uniforms{"world",
                                    "view",
                                    "viewProjection",
                                    "vEyePosition",
                                    "vLightsType",
                                    "vAmbientColor",
                                    "vAlbedoColor",
                                    "vReflectivityColor",
                                    "vMetallicReflectanceFactors",
                                    "vEmissiveColor",
                                    "visibility",
                                    "vReflectionColor",
                                    "vFogInfos",
                                    "vFogColor",
                                    "pointSize",
                                    "vAlbedoInfos",
                                    "vAmbientInfos",
                                    "vOpacityInfos",
                                    "vReflectionInfos",
                                    "vReflectionPosition",
                                    "vReflectionSize",
                                    "vEmissiveInfos",
                                    "vReflectivityInfos",
                                    "vReflectionFilteringInfo",
                                    "vMetallicReflectanceInfos",
                                    "vMicroSurfaceSamplerInfos",
                                    "vBumpInfos",
                                    "vLightmapInfos",
                                    "mBones",
                                    "vClipPlane",
                                    "vClipPlane2",
                                    "vClipPlane3",
                                    "vClipPlane4",
                                    "vClipPlane5",
                                    "vClipPlane6",
                                    "albedoMatrix",
                                    "ambientMatrix",
                                    "opacityMatrix",
                                    "reflectionMatrix",
                                    "emissiveMatrix",
                                    "reflectivityMatrix",
                                    "normalMatrix",
                                    "microSurfaceSamplerMatrix",
                                    "bumpMatrix",
                                    "lightmapMatrix",
                                    "metallicReflectanceMatrix",
                                    "vLightingIntensity",
                                    "logarithmicDepthConstant",
                                    "vSphericalX",
                                    "vSphericalY",
                                    "vSphericalZ",
                                    "vSphericalXX_ZZ",
                                    "vSphericalYY_ZZ",
                                    "vSphericalZZ",
                                    "vSphericalXY",
                                    "vSphericalYZ",
                                    "vSphericalZX",
                                    "vSphericalL00",
                                    "vSphericalL1_1",
                                    "vSphericalL10",
                                    "vSphericalL11",
                                    "vSphericalL2_2",
                                    "vSphericalL2_1",
                                    "vSphericalL20",
                                    "vSphericalL21",
                                    "vSphericalL22",
                                    "vReflectionMicrosurfaceInfos",
                                    "vTangentSpaceParams",
                                    "boneTextureWidth",
                                    "vDebugMode"};

  std::vector<std::string> samplers{
    "albedoSampler",          "reflectivitySampler", "ambientSampler",
    "emissiveSampler",        "bumpSampler",         "lightmapSampler",
    "opacitySampler",         "reflectionSampler",   "reflectionSamplerLow",
    "reflectionSamplerHigh",  "irradianceSampler",   "microSurfaceSampler",
    "environmentBrdfSampler", "boneSampler",         "metallicReflectanceSampler"};

  std::vector<std::string> uniformBuffers{"Material", "Scene"};

  DetailMapConfiguration::AddUniforms(uniforms);
  DetailMapConfiguration::AddSamplers(samplers);

  PBRSubSurfaceConfiguration::AddUniforms(uniforms);
  PBRSubSurfaceConfiguration::AddSamplers(samplers);

  PBRClearCoatConfiguration::AddUniforms(uniforms);
  PBRClearCoatConfiguration::AddSamplers(samplers);

  PBRAnisotropicConfiguration::AddUniforms(uniforms);
  PBRAnisotropicConfiguration::AddSamplers(samplers);

  PBRSheenConfiguration::AddUniforms(uniforms);
  PBRSheenConfiguration::AddSamplers(samplers);

  // if (ImageProcessingConfiguration)
  {
    ImageProcessingConfiguration::PrepareUniforms(uniforms, defines);
    ImageProcessingConfiguration::PrepareSamplers(samplers, defines);
  }

  std::unordered_map<std::string, unsigned int> indexParameters{
    {"maxSimultaneousLights", _maxSimultaneousLights},
    {"maxSimultaneousMorphTargets", defines.intDef["NUM_MORPH_INFLUENCERS"]}};

  ICustomShaderNameResolveOptions csnrOptions{};

  if (customShaderNameResolve) {
    shaderName = customShaderNameResolve(shaderName, uniforms, uniformBuffers, samplers, &defines,
                                         nullptr, attribs, &csnrOptions);
  }

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
  options.onError               = iOnError;
  options.indexParameters       = std::move(indexParameters);
  options.processFinalCode      = csnrOptions.processFinalCode;
  options.maxSimultaneousLights = _maxSimultaneousLights;
  options.multiTarget           = defines["PREPASS"];

  MaterialHelper::PrepareUniformsAndSamplersList(options);

  return engine->createEffect(shaderName, options, engine);
}

void PBRBaseMaterial::_prepareDefines(AbstractMesh* mesh, PBRMaterialDefines& defines,
                                      const std::optional<bool>& useInstances,
                                      const std::optional<bool>& useClipPlane,
                                      bool useThinInstances)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  // Lights
  MaterialHelper::PrepareDefinesForLights(scene, mesh, defines, true, _maxSimultaneousLights,
                                          _disableLighting);
  defines._needNormals = true;

  // Multiview
  MaterialHelper::PrepareDefinesForMultiview(scene, defines);

  // PrePass
  MaterialHelper::PrepareDefinesForPrePass(scene, defines, canRenderToMRT());

  // Textures
  defines.boolDef["METALLICWORKFLOW"] = isMetallicWorkflow();
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (scene->getEngine()->getCaps().textureLOD) {
        defines.boolDef["LODBASEDMICROSFURACE"] = true;
      }

      if (_albedoTexture && MaterialFlags::DiffuseTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_albedoTexture, defines, "ALBEDO");
        defines.boolDef["GAMMAALBEDO"] = _albedoTexture->gammaSpace();
      }
      else {
        defines.boolDef["ALBEDO"] = false;
      }

      if (_ambientTexture && MaterialFlags::AmbientTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_ambientTexture, defines, "AMBIENT");
        defines.boolDef["AMBIENTINGRAYSCALE"] = _useAmbientInGrayScale;
      }
      else {
        defines.boolDef["AMBIENT"] = false;
      }

      if (_opacityTexture && MaterialFlags::OpacityTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_opacityTexture, defines, "OPACITY");
        defines.boolDef["OPACITYRGB"] = _opacityTexture->getAlphaFromRGB;
      }
      else {
        defines.boolDef["OPACITY"] = false;
      }

      auto reflectionTexture = _getReflectionTexture();
      if (reflectionTexture && MaterialFlags::ReflectionTextureEnabled()) {
        defines.boolDef["REFLECTION"]              = true;
        defines.boolDef["GAMMAREFLECTION"]         = reflectionTexture->gammaSpace;
        defines.boolDef["RGBDREFLECTION"]          = reflectionTexture->isRGBD;
        defines.boolDef["REFLECTIONMAP_OPPOSITEZ"] = getScene()->useRightHandedSystem() ?
                                                       !reflectionTexture->invertZ :
                                                       reflectionTexture->invertZ;
        defines.boolDef["LODINREFLECTIONALPHA"]     = reflectionTexture->lodLevelInAlpha;
        defines.boolDef["LINEARSPECULARREFLECTION"] = reflectionTexture->linearSpecularLOD();

        if (realTimeFiltering && realTimeFilteringQuality() > 0) {
          defines.intDef["NUM_SAMPLES"] = realTimeFilteringQuality;
          if (engine->webGLVersion > 1.f) {
            defines.intDef["NUM_SAMPLES"] = defines.intDef["NUM_SAMPLES"];
          }
          defines.boolDef["REALTIME_FILTERING"] = true;
        }
        else {
          defines.boolDef["REALTIME_FILTERING"] = false;
        }

        if (reflectionTexture->coordinatesMode() == TextureConstants::INVCUBIC_MODE) {
          defines.boolDef["INVERTCUBICMAP"] = true;
        }

        defines.boolDef["REFLECTIONMAP_3D"] = reflectionTexture->isCube();

        defines.boolDef["REFLECTIONMAP_CUBIC"]                         = false;
        defines.boolDef["REFLECTIONMAP_EXPLICIT"]                      = false;
        defines.boolDef["REFLECTIONMAP_PLANAR"]                        = false;
        defines.boolDef["REFLECTIONMAP_PROJECTION"]                    = false;
        defines.boolDef["REFLECTIONMAP_SKYBOX"]                        = false;
        defines.boolDef["REFLECTIONMAP_SPHERICAL"]                     = false;
        defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR"]               = false;
        defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR_FIXED"]         = false;
        defines.boolDef["REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED"] = false;

        switch (reflectionTexture->coordinatesMode()) {
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
            defines.boolDef["REFLECTIONMAP_CUBIC"] = true;
            defines.boolDef["USE_LOCAL_REFLECTIONMAP_CUBIC"]
              = static_cast<bool>(reflectionTexture->boundingBoxSize());
            break;
        }

        if (reflectionTexture->coordinatesMode() != TextureConstants::SKYBOX_MODE) {
          if (reflectionTexture->irradianceTexture()) {
            defines.boolDef["USEIRRADIANCEMAP"]              = true;
            defines.boolDef["USESPHERICALFROMREFLECTIONMAP"] = false;
          }
          // Assume using spherical polynomial if the reflection texture is a cube map
          else if (reflectionTexture->isCube()) {
            defines.boolDef["USESPHERICALFROMREFLECTIONMAP"] = true;
            defines.boolDef["USEIRRADIANCEMAP"]              = false;
            if (_forceIrradianceInFragment || realTimeFiltering
                || scene->getEngine()->getCaps().maxVaryingVectors <= 8) {
              defines.boolDef["USESPHERICALINVERTEX"] = false;
            }
            else {
              defines.boolDef["USESPHERICALINVERTEX"] = true;
            }
          }
        }
      }
      else {
        defines.boolDef["REFLECTION"]                                  = false;
        defines.boolDef["REFLECTIONMAP_3D"]                            = false;
        defines.boolDef["REFLECTIONMAP_SPHERICAL"]                     = false;
        defines.boolDef["REFLECTIONMAP_PLANAR"]                        = false;
        defines.boolDef["REFLECTIONMAP_CUBIC"]                         = false;
        defines.boolDef["USE_LOCAL_REFLECTIONMAP_CUBIC"]               = false;
        defines.boolDef["REFLECTIONMAP_PROJECTION"]                    = false;
        defines.boolDef["REFLECTIONMAP_SKYBOX"]                        = false;
        defines.boolDef["REFLECTIONMAP_EXPLICIT"]                      = false;
        defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR"]               = false;
        defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR_FIXED"]         = false;
        defines.boolDef["REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED"] = false;
        defines.boolDef["INVERTCUBICMAP"]                              = false;
        defines.boolDef["USESPHERICALFROMREFLECTIONMAP"]               = false;
        defines.boolDef["USEIRRADIANCEMAP"]                            = false;
        defines.boolDef["USESPHERICALINVERTEX"]                        = false;
        defines.boolDef["REFLECTIONMAP_OPPOSITEZ"]                     = false;
        defines.boolDef["LODINREFLECTIONALPHA"]                        = false;
        defines.boolDef["GAMMAREFLECTION"]                             = false;
        defines.boolDef["RGBDREFLECTION"]                              = false;
        defines.boolDef["LINEARSPECULARREFLECTION"]                    = false;
      }

      if (_lightmapTexture && MaterialFlags::LightmapTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_lightmapTexture, defines, "LIGHTMAP");
        defines.boolDef["USELIGHTMAPASSHADOWMAP"] = _useLightmapAsShadowmap;
        defines.boolDef["GAMMALIGHTMAP"]          = _lightmapTexture->gammaSpace;
        defines.boolDef["RGBDLIGHTMAP"]           = _lightmapTexture->isRGBD();
      }
      else {
        defines.boolDef["LIGHTMAP"] = false;
      }

      if (_emissiveTexture && MaterialFlags::EmissiveTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_emissiveTexture, defines, "EMISSIVE");
      }
      else {
        defines.boolDef["EMISSIVE"] = false;
      }

      if (MaterialFlags::SpecularTextureEnabled()) {
        if (_metallicTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(_metallicTexture, defines, "REFLECTIVITY");
          defines.boolDef["ROUGHNESSSTOREINMETALMAPALPHA"] = _useRoughnessFromMetallicTextureAlpha;
          defines.boolDef["ROUGHNESSSTOREINMETALMAPGREEN"]
            = !_useRoughnessFromMetallicTextureAlpha && _useRoughnessFromMetallicTextureGreen;
          defines.boolDef["METALLNESSSTOREINMETALMAPBLUE"] = _useMetallnessFromMetallicTextureBlue;
          defines.boolDef["AOSTOREINMETALMAPRED"] = _useAmbientOcclusionFromMetallicTextureRed;
        }
        else if (_reflectivityTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(_reflectivityTexture, defines, "REFLECTIVITY");
          defines.boolDef["MICROSURFACEFROMREFLECTIVITYMAP"]
            = _useMicroSurfaceFromReflectivityMapAlpha;
          defines.boolDef["MICROSURFACEAUTOMATIC"] = _useAutoMicroSurfaceFromReflectivityMap;
        }
        else {
          defines.boolDef["REFLECTIVITY"] = false;
        }

        if (_metallicReflectanceTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(_metallicReflectanceTexture, defines,
                                                    "METALLIC_REFLECTANCE");
        }
        else {
          defines.boolDef["METALLIC_REFLECTANCE"] = false;
        }

        if (_microSurfaceTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(_microSurfaceTexture, defines,
                                                    "MICROSURFACEMAP");
        }
        else {
          defines.boolDef["MICROSURFACEMAP"] = false;
        }
      }
      else {
        defines.boolDef["REFLECTIVITY"]    = false;
        defines.boolDef["MICROSURFACEMAP"] = false;
      }

      if (scene->getEngine()->getCaps().standardDerivatives && _bumpTexture
          && MaterialFlags::BumpTextureEnabled() && !_disableBumpMap) {
        MaterialHelper::PrepareDefinesForMergedUV(_bumpTexture, defines, "BUMP");

        if (_useParallax && _albedoTexture && MaterialFlags::DiffuseTextureEnabled()) {
          defines.boolDef["PARALLAX"]          = true;
          defines.boolDef["PARALLAXOCCLUSION"] = !!_useParallaxOcclusion;
        }
        else {
          defines.boolDef["PARALLAX"] = false;
        }
        defines.boolDef["OBJECTSPACE_NORMALMAP"] = _useObjectSpaceNormalMap;
      }
      else {
        defines.boolDef["BUMP"] = false;
      }

      if (_environmentBRDFTexture && MaterialFlags::ReflectionTextureEnabled()) {
        defines.boolDef["ENVIRONMENTBRDF"] = true;
        // Not actual true RGBD, only the B chanel is encoded as RGBD for sheen.
        defines.boolDef["ENVIRONMENTBRDF_RGBD"] = _environmentBRDFTexture->isRGBD();
      }
      else {
        defines.boolDef["ENVIRONMENTBRDF"]      = false;
        defines.boolDef["ENVIRONMENTBRDF_RGBD"] = false;
      }

      if (_shouldUseAlphaFromAlbedoTexture()) {
        defines.boolDef["ALPHAFROMALBEDO"] = true;
      }
      else {
        defines.boolDef["ALPHAFROMALBEDO"] = false;
      }
    }

    defines.boolDef["SPECULAROVERALPHA"] = _useSpecularOverAlpha;

    if (_lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_STANDARD) {
      defines.boolDef["USEPHYSICALLIGHTFALLOFF"] = false;
      defines.boolDef["USEGLTFLIGHTFALLOFF"]     = false;
    }
    else if (_lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_GLTF) {
      defines.boolDef["USEPHYSICALLIGHTFALLOFF"] = false;
      defines.boolDef["USEGLTFLIGHTFALLOFF"]     = true;
    }
    else {
      defines.boolDef["USEPHYSICALLIGHTFALLOFF"] = true;
      defines.boolDef["USEGLTFLIGHTFALLOFF"]     = false;
    }

    defines.boolDef["RADIANCEOVERALPHA"] = _useRadianceOverAlpha;

    if (!backFaceCulling() && _twoSidedLighting) {
      defines.boolDef["TWOSIDEDLIGHTING"] = true;
    }
    else {
      defines.boolDef["TWOSIDEDLIGHTING"] = false;
    }

    defines.boolDef["SPECULARAA"]
      = scene->getEngine()->getCaps().standardDerivatives && _enableSpecularAntiAliasing;
  }

  if (defines._areTexturesDirty || defines._areMiscDirty) {
    defines.stringDef["ALPHATESTVALUE"]
      = std::to_string(_alphaCutOff) + (std::fmod(_alphaCutOff, 1.f) == 0.f ? "." : "");
    defines.boolDef["PREMULTIPLYALPHA"]
      = (alphaMode() == Constants::ALPHA_PREMULTIPLIED
         || alphaMode() == Constants::ALPHA_PREMULTIPLIED_PORTERDUFF);
    defines.boolDef["ALPHABLEND"]         = needAlphaBlendingForMesh(*mesh);
    defines.boolDef["ALPHAFRESNEL"]       = _useAlphaFresnel || _useLinearAlphaFresnel;
    defines.boolDef["LINEARALPHAFRESNEL"] = _useLinearAlphaFresnel;
  }

  if (defines._areImageProcessingDirty && _imageProcessingConfiguration) {
    _imageProcessingConfiguration->prepareDefines(defines);
  }

  defines.boolDef["FORCENORMALFORWARD"] = _forceNormalForward;

  defines.boolDef["RADIANCEOCCLUSION"] = _useRadianceOcclusion;

  defines.boolDef["HORIZONOCCLUSION"] = _useHorizonOcclusion;

  // Misc.
  if (defines._areMiscDirty) {
    MaterialHelper::PrepareDefinesForMisc(mesh, scene, _useLogarithmicDepth, pointsCloud(),
                                          fogEnabled(),
                                          _shouldTurnAlphaTestOn(mesh) || _forceAlphaTest, defines);
    defines.boolDef["UNLIT"] = _unlit
                               || ((pointsCloud() || wireframe())
                                   && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind));
    defines.intDef["DEBUGMODE"] = static_cast<unsigned>(_debugMode);
  }

  // External config
  detailMap->prepareDefines(defines, scene);
  subSurface->prepareDefines(defines, scene);
  clearCoat->prepareDefines(defines, scene);
  anisotropy->prepareDefines(defines, *mesh, scene);
  brdf->prepareDefines(defines);
  sheen->prepareDefines(defines, scene);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(scene, engine, defines,
                                                    useInstances.has_value() && (*useInstances),
                                                    useClipPlane, useThinInstances);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, true, true, true, _transparencyMode != PBRBaseMaterial::PBRMATERIAL_OPAQUE);
}

void PBRBaseMaterial::forceCompilation(AbstractMesh* mesh,
                                       std::function<void(Material* material)>& iOnCompiled,
                                       const IMaterialCompilationOptions& options)
{
  const auto& localOptions = options;
  PBRMaterialDefines defines;
  auto effect = _prepareEffect(mesh, defines, nullptr, nullptr, localOptions.useInstances,
                               localOptions.clipPlane, mesh->hasThinInstances());
  /* if (_onEffectCreatedObservable) */ {
    onCreatedEffectParameters.effect  = effect.get();
    onCreatedEffectParameters.subMesh = nullptr;
    _onEffectCreatedObservable.notifyObservers(&onCreatedEffectParameters);
  }
  if (effect->isReady()) {
    if (iOnCompiled) {
      iOnCompiled(this);
    }
  }
  else {
    effect->onCompileObservable.add([&](Effect* /*effect*/, EventState& /*es*/) {
      if (iOnCompiled) {
        iOnCompiled(this);
      }
    });
  }
}

void PBRBaseMaterial::buildUniformLayout()
{
  // Order is important !
  auto& ubo = *_uniformBuffer;
  ubo.addUniform("vAlbedoInfos", 2);
  ubo.addUniform("vAmbientInfos", 4);
  ubo.addUniform("vOpacityInfos", 2);
  ubo.addUniform("vEmissiveInfos", 2);
  ubo.addUniform("vLightmapInfos", 2);
  ubo.addUniform("vReflectivityInfos", 3);
  ubo.addUniform("vMicroSurfaceSamplerInfos", 2);
  ubo.addUniform("vReflectionInfos", 2);
  ubo.addUniform("vReflectionFilteringInfo", 2);
  ubo.addUniform("vReflectionPosition", 3);
  ubo.addUniform("vReflectionSize", 3);
  ubo.addUniform("vBumpInfos", 3);
  ubo.addUniform("albedoMatrix", 16);
  ubo.addUniform("ambientMatrix", 16);
  ubo.addUniform("opacityMatrix", 16);
  ubo.addUniform("emissiveMatrix", 16);
  ubo.addUniform("lightmapMatrix", 16);
  ubo.addUniform("reflectivityMatrix", 16);
  ubo.addUniform("microSurfaceSamplerMatrix", 16);
  ubo.addUniform("bumpMatrix", 16);
  ubo.addUniform("vTangentSpaceParams", 2);
  ubo.addUniform("reflectionMatrix", 16);

  ubo.addUniform("vReflectionColor", 3);
  ubo.addUniform("vAlbedoColor", 4);
  ubo.addUniform("vLightingIntensity", 4);

  ubo.addUniform("vReflectionMicrosurfaceInfos", 3);
  ubo.addUniform("pointSize", 1);
  ubo.addUniform("vReflectivityColor", 4);
  ubo.addUniform("vEmissiveColor", 3);
  ubo.addUniform("visibility", 1);
  ubo.addUniform("vMetallicReflectanceFactors", 4);
  ubo.addUniform("vMetallicReflectanceInfos", 2);
  ubo.addUniform("metallicReflectanceMatrix", 16);

  PBRClearCoatConfiguration::PrepareUniformBuffer(ubo);
  PBRAnisotropicConfiguration::PrepareUniformBuffer(ubo);
  PBRSheenConfiguration::PrepareUniformBuffer(ubo);
  PBRSubSurfaceConfiguration::PrepareUniformBuffer(ubo);
  DetailMapConfiguration::PrepareUniformBuffer(ubo);

  ubo.create();
}

void PBRBaseMaterial::unbind()
{
  if (_activeEffect) {
    auto needFlag = false;
    if (_reflectionTexture && _reflectionTexture->isRenderTarget) {
      _activeEffect->setTexture("reflection2DSampler", nullptr);
      needFlag = true;
    }

    if (subSurface->unbind(_activeEffect.get())) {
      needFlag = true;
    }

    if (needFlag) {
      _markAllSubMeshesAsTexturesDirty();
    }
  }

  PushMaterial::unbind();
}

void PBRBaseMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto definesTmp = static_cast<PBRMaterialDefines*>(subMesh->_materialDefines.get());
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
  if (!defines["INSTANCES"] || defines["THIN_INSTANCES"]) {
    bindOnlyWorldMatrix(world);
  }

  // Normal Matrix
  if (defines["OBJECTSPACE_NORMALMAP"]) {
    world.toNormalMatrix(_normalMatrix);
    bindOnlyNormalMatrix(_normalMatrix);
  }

  const auto mustRebind = _mustRebind(scene, effect, mesh->visibility());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  BaseTexturePtr reflectionTexture = nullptr;
  auto& ubo                        = *_uniformBuffer;
  if (mustRebind) {
    auto engine = scene->getEngine();
    ubo.bindToEffect(effect.get(), "Material");

    bindViewProjection(effect);
    reflectionTexture = _getReflectionTexture();

    if (!ubo.useUbo() || !isFrozen() || !ubo.isSync()) {

      // Texture uniforms
      if (scene->texturesEnabled()) {
        if (_albedoTexture && MaterialFlags::DiffuseTextureEnabled()) {
          ubo.updateFloat2("vAlbedoInfos", static_cast<float>(_albedoTexture->coordinatesIndex),
                           _albedoTexture->level, "");
          MaterialHelper::BindTextureMatrix(*_albedoTexture, ubo, "albedo");
        }

        if (_ambientTexture && MaterialFlags::AmbientTextureEnabled()) {
          ubo.updateFloat4("vAmbientInfos", static_cast<float>(_ambientTexture->coordinatesIndex),
                           _ambientTexture->level, _ambientTextureStrength,
                           static_cast<float>(_ambientTextureImpactOnAnalyticalLights), "");
          MaterialHelper::BindTextureMatrix(*_ambientTexture, ubo, "ambient");
        }

        if (_opacityTexture && MaterialFlags::OpacityTextureEnabled()) {
          ubo.updateFloat2("vOpacityInfos", static_cast<float>(_opacityTexture->coordinatesIndex),
                           _opacityTexture->level, "");
          MaterialHelper::BindTextureMatrix(*_opacityTexture, ubo, "opacity");
        }

        if (reflectionTexture && MaterialFlags::ReflectionTextureEnabled()) {
          ubo.updateMatrix("reflectionMatrix", *reflectionTexture->getReflectionTextureMatrix());
          ubo.updateFloat2("vReflectionInfos", reflectionTexture->level, 0, "");

          if (reflectionTexture->boundingBoxSize()) {
            auto cubeTexture = std::static_pointer_cast<CubeTexture>(reflectionTexture);
            if (cubeTexture) {
              ubo.updateVector3("vReflectionPosition", cubeTexture->boundingBoxPosition);
              ubo.updateVector3("vReflectionSize", *cubeTexture->boundingBoxSize());
            }
          }

          if (realTimeFiltering) {
            const auto width = static_cast<float>(reflectionTexture->getSize().width);
            ubo.updateFloat2("vReflectionFilteringInfo", width, Scalar::Log2(width), "");
          }

          if (!defines["USEIRRADIANCEMAP"]) {
            auto _polynomials = reflectionTexture->sphericalPolynomial();
            if (defines["USESPHERICALFROMREFLECTIONMAP"] && _polynomials) {
              auto polynomials = *_polynomials;
              if (defines["SPHERICAL_HARMONICS"]) {
                auto& preScaledHarmonics = polynomials.preScaledHarmonics();
                _activeEffect->setVector3("vSphericalL00", preScaledHarmonics.l00);
                _activeEffect->setVector3("vSphericalL1_1", preScaledHarmonics.l1_1);
                _activeEffect->setVector3("vSphericalL10", preScaledHarmonics.l10);
                _activeEffect->setVector3("vSphericalL11", preScaledHarmonics.l11);
                _activeEffect->setVector3("vSphericalL2_2", preScaledHarmonics.l2_2);
                _activeEffect->setVector3("vSphericalL2_1", preScaledHarmonics.l2_1);
                _activeEffect->setVector3("vSphericalL20", preScaledHarmonics.l20);
                _activeEffect->setVector3("vSphericalL21", preScaledHarmonics.l21);
                _activeEffect->setVector3("vSphericalL22", preScaledHarmonics.l22);
              }
              else {
                _activeEffect->setFloat3("vSphericalX", polynomials.x.x, polynomials.x.y,
                                         polynomials.x.z);
                _activeEffect->setFloat3("vSphericalY", polynomials.y.x, polynomials.y.y,
                                         polynomials.y.z);
                _activeEffect->setFloat3("vSphericalZ", polynomials.z.x, polynomials.z.y,
                                         polynomials.z.z);
                _activeEffect->setFloat3("vSphericalXX_ZZ", polynomials.xx.x - polynomials.zz.x,
                                         polynomials.xx.y - polynomials.zz.y,
                                         polynomials.xx.z - polynomials.zz.z);
                _activeEffect->setFloat3("vSphericalYY_ZZ", polynomials.yy.x - polynomials.zz.x,
                                         polynomials.yy.y - polynomials.zz.y,
                                         polynomials.yy.z - polynomials.zz.z);
                _activeEffect->setFloat3("vSphericalZZ", polynomials.zz.x, polynomials.zz.y,
                                         polynomials.zz.z);
                _activeEffect->setFloat3("vSphericalXY", polynomials.xy.x, polynomials.xy.y,
                                         polynomials.xy.z);
                _activeEffect->setFloat3("vSphericalYZ", polynomials.yz.x, polynomials.yz.y,
                                         polynomials.yz.z);
                _activeEffect->setFloat3("vSphericalZX", polynomials.zx.x, polynomials.zx.y,
                                         polynomials.zx.z);
              }
            }
          }

          ubo.updateFloat3(
            "vReflectionMicrosurfaceInfos", static_cast<float>(reflectionTexture->getSize().width),
            reflectionTexture->lodGenerationScale(), reflectionTexture->lodGenerationOffset(), "");
        }

        if (_emissiveTexture && MaterialFlags::EmissiveTextureEnabled()) {
          ubo.updateFloat2("vEmissiveInfos", static_cast<float>(_emissiveTexture->coordinatesIndex),
                           _emissiveTexture->level, "");
          MaterialHelper::BindTextureMatrix(*_emissiveTexture, ubo, "emissive");
        }

        if (_lightmapTexture && MaterialFlags::LightmapTextureEnabled()) {
          ubo.updateFloat2("vLightmapInfos", static_cast<float>(_lightmapTexture->coordinatesIndex),
                           _lightmapTexture->level, "");
          MaterialHelper::BindTextureMatrix(*_lightmapTexture, ubo, "lightmap");
        }

        if (MaterialFlags::SpecularTextureEnabled()) {
          if (_metallicTexture) {
            ubo.updateFloat3("vReflectivityInfos",
                             static_cast<float>(_metallicTexture->coordinatesIndex),
                             _metallicTexture->level, _ambientTextureStrength, "");
            MaterialHelper::BindTextureMatrix(*_metallicTexture, ubo, "reflectivity");
          }
          else if (_reflectivityTexture) {
            ubo.updateFloat3("vReflectivityInfos",
                             static_cast<float>(_reflectivityTexture->coordinatesIndex),
                             _reflectivityTexture->level, 1.f, "");
            MaterialHelper::BindTextureMatrix(*_reflectivityTexture, ubo, "reflectivity");
          }

          if (_metallicReflectanceTexture) {
            ubo.updateFloat2("vMetallicReflectanceInfos",
                             static_cast<float>(_metallicReflectanceTexture->coordinatesIndex),
                             _metallicReflectanceTexture->level, "");
            MaterialHelper::BindTextureMatrix(*_metallicReflectanceTexture, ubo,
                                              "metallicReflectance");
          }

          if (_microSurfaceTexture) {
            ubo.updateFloat2("vMicroSurfaceSamplerInfos",
                             static_cast<float>(_microSurfaceTexture->coordinatesIndex),
                             _microSurfaceTexture->level, "");
            MaterialHelper::BindTextureMatrix(*_microSurfaceTexture, ubo, "microSurfaceSampler");
          }
        }

        if (_bumpTexture && engine->getCaps().standardDerivatives
            && MaterialFlags::BumpTextureEnabled() && !_disableBumpMap) {
          ubo.updateFloat3("vBumpInfos", static_cast<float>(_bumpTexture->coordinatesIndex),
                           _bumpTexture->level, _parallaxScaleBias, "");
          MaterialHelper::BindTextureMatrix(*_bumpTexture, ubo, "bump");

          if (scene->_mirroredCameraPosition) {
            ubo.updateFloat2("vTangentSpaceParams", _invertNormalMapX ? 1.f : -1.f,
                             _invertNormalMapY ? 1.f : -1.f, "");
          }
          else {
            ubo.updateFloat2("vTangentSpaceParams", _invertNormalMapX ? -1.f : 1.f,
                             _invertNormalMapY ? -1.f : 1.f, "");
          }
        }
      }

      // Point size
      if (pointsCloud) {
        ubo.updateFloat("pointSize", pointSize);
      }

      // Colors
      if (defines["METALLICWORKFLOW"]) {
        TmpVectors::Color3Array[0].r = !_metallic.has_value() ? 1.f : *_metallic;
        TmpVectors::Color3Array[0].g = !_roughness.has_value() ? 1.f : *_roughness;
        ubo.updateColor4("vReflectivityColor", TmpVectors::Color3Array[0], 1, "");

        const auto ior = subSurface->indexOfRefraction();
        const auto outside_ior
          = 1.f; // consider air as clear coat and other layaers would remap in the shader.

        // We are here deriving our default reflectance from a common value for none metallic
        // surface. Based of the schlick fresnel approximation model for dielectrics.
        const auto f0 = std::pow((ior - outside_ior) / (ior + outside_ior), 2);

        // Tweak the default F0 and F90 based on our given setup
        _metallicReflectanceColor.scaleToRef(f0 * _metallicF0Factor, TmpVectors::Color3Array[0]);
        const auto metallicF90 = _metallicF0Factor;

        ubo.updateColor4("vMetallicReflectanceFactors", TmpVectors::Color3Array[0], metallicF90,
                         "");
      }
      else {
        ubo.updateColor4("vReflectivityColor", _reflectivityColor, _microSurface, "");
      }

      ubo.updateColor3(
        "vEmissiveColor",
        MaterialFlags::EmissiveTextureEnabled() ? _emissiveColor : Color3::BlackReadOnly(), "");
      ubo.updateColor3("vReflectionColor", _reflectionColor, "");
      if (!defines["SS_REFRACTION"] && subSurface->linkRefractionWithTransparency()) {
        ubo.updateColor4("vAlbedoColor", _albedoColor, 1.f, "");
      }
      else {
        ubo.updateColor4("vAlbedoColor", _albedoColor, alpha, "");
      }

      // Misc
      _lightingInfos.x = _directIntensity;
      _lightingInfos.y = _emissiveIntensity;
      _lightingInfos.z = _environmentIntensity * scene->environmentIntensity();
      _lightingInfos.w = _specularIntensity;

      ubo.updateVector4("vLightingIntensity", _lightingInfos);
    }

    // Visibility
    ubo.updateFloat("visibility", mesh->visibility());

    // Textures
    if (scene->texturesEnabled()) {
      if (_albedoTexture && MaterialFlags::DiffuseTextureEnabled()) {
        ubo.setTexture("albedoSampler", _albedoTexture);
      }

      if (_ambientTexture && MaterialFlags::AmbientTextureEnabled()) {
        ubo.setTexture("ambientSampler", _ambientTexture);
      }

      if (_opacityTexture && MaterialFlags::OpacityTextureEnabled()) {
        ubo.setTexture("opacitySampler", _opacityTexture);
      }

      if (reflectionTexture && MaterialFlags::ReflectionTextureEnabled()) {
        if (defines["LODBASEDMICROSFURACE"]) {
          ubo.setTexture("reflectionSampler", reflectionTexture);
        }
        else {
          ubo.setTexture("reflectionSampler", reflectionTexture->_lodTextureMid() ?
                                                reflectionTexture->_lodTextureMid() :
                                                reflectionTexture);
          ubo.setTexture("reflectionSamplerLow", reflectionTexture->_lodTextureLow() ?
                                                   reflectionTexture->_lodTextureLow() :
                                                   reflectionTexture);
          ubo.setTexture("reflectionSamplerHigh", reflectionTexture->_lodTextureHigh() ?
                                                    reflectionTexture->_lodTextureHigh() :
                                                    reflectionTexture);
        }

        if (defines["USEIRRADIANCEMAP"]) {
          ubo.setTexture("irradianceSampler", reflectionTexture->irradianceTexture());
        }
      }

      if (defines["ENVIRONMENTBRDF"]) {
        ubo.setTexture("environmentBrdfSampler", _environmentBRDFTexture);
      }

      if (_emissiveTexture && MaterialFlags::EmissiveTextureEnabled()) {
        ubo.setTexture("emissiveSampler", _emissiveTexture);
      }

      if (_lightmapTexture && MaterialFlags::LightmapTextureEnabled()) {
        ubo.setTexture("lightmapSampler", _lightmapTexture);
      }

      if (MaterialFlags::SpecularTextureEnabled()) {
        if (_metallicTexture) {
          ubo.setTexture("reflectivitySampler", _metallicTexture);
        }
        else if (_reflectivityTexture) {
          ubo.setTexture("reflectivitySampler", _reflectivityTexture);
        }

        if (_metallicReflectanceTexture) {
          ubo.setTexture("metallicReflectanceSampler", _metallicReflectanceTexture);
        }

        if (_microSurfaceTexture) {
          ubo.setTexture("microSurfaceSampler", _microSurfaceTexture);
        }
      }

      if (_bumpTexture && engine->getCaps().standardDerivatives
          && MaterialFlags::BumpTextureEnabled() && !_disableBumpMap) {
        ubo.setTexture("bumpSampler", _bumpTexture);
      }
    }

    detailMap->bindForSubMesh(ubo, scene, isFrozen());
    subSurface->bindForSubMesh(ubo, scene, engine, isFrozen(), defines["LODBASEDMICROSFURACE"],
                               _realTimeFiltering);
    clearCoat->bindForSubMesh(ubo, scene, engine, _disableBumpMap, isFrozen(), _invertNormalMapX,
                              _invertNormalMapY);
    anisotropy->bindForSubMesh(ubo, scene, isFrozen());
    sheen->bindForSubMesh(ubo, scene, isFrozen());

    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Colors
    scene->ambientColor.multiplyToRef(_ambientColor, _globalAmbientColor);

    auto eyePosition = scene->_forcedViewPosition ?
                         *scene->_forcedViewPosition :
                         (scene->_mirroredCameraPosition ? *scene->_mirroredCameraPosition :
                                                           scene->activeCamera()->globalPosition());
    auto invertNormal
      = (scene->useRightHandedSystem() == (scene->_mirroredCameraPosition != nullptr));
    effect->setFloat4("vEyePosition", eyePosition.x, eyePosition.y, eyePosition.z,
                      invertNormal ? -1.f : 1.f);
    effect->setColor3("vAmbientColor", _globalAmbientColor);

    effect->setFloat2("vDebugMode", debugLimit, debugFactor);
  }

  if (mustRebind || !isFrozen()) {
    // Lights
    if (scene->lightsEnabled() && !_disableLighting) {
      MaterialHelper::BindLights(scene, mesh, _activeEffect, defines, _maxSimultaneousLights,
                                 _rebuildInParallel);
    }

    // View
    if ((scene->fogEnabled() && mesh->applyFog() && scene->fogMode() != Scene::FOGMODE_NONE)
        || reflectionTexture) {
      bindView(effect.get());
    }

    // Fog
    MaterialHelper::BindFogParameters(scene, mesh, _activeEffect, true);

    // Morph targets
    if (defines.intDef["NUM_MORPH_INFLUENCERS"]) {
      MaterialHelper::BindMorphTargetParameters(mesh, _activeEffect);
    }

    // image processing
    _imageProcessingConfiguration->bind(_activeEffect.get());

    // Log. depth
    MaterialHelper::BindLogDepth(defines, _activeEffect, scene);
  }

  ubo.update();

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> PBRBaseMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_albedoTexture && !_albedoTexture->animations.empty()) {
    results.emplace_back(_albedoTexture);
  }

  if (_ambientTexture && !_ambientTexture->animations.empty()) {
    results.emplace_back(_ambientTexture);
  }

  if (_opacityTexture && !_opacityTexture->animations.empty()) {
    results.emplace_back(_opacityTexture);
  }

  if (_reflectionTexture && !_reflectionTexture->animations.empty()) {
    results.emplace_back(_reflectionTexture);
  }

  if (_emissiveTexture && !_emissiveTexture->animations.empty()) {
    results.emplace_back(_emissiveTexture);
  }

  if (_metallicTexture && !_metallicTexture->animations.empty()) {
    results.emplace_back(_metallicTexture);
  }
  else if (_reflectivityTexture && !_reflectivityTexture->animations.empty()) {
    results.emplace_back(_reflectivityTexture);
  }

  if (_bumpTexture && !_bumpTexture->animations.empty()) {
    results.emplace_back(_bumpTexture);
  }

  if (_lightmapTexture && !_lightmapTexture->animations.empty()) {
    results.emplace_back(_lightmapTexture);
  }

  detailMap->getAnimatables(results);
  subSurface->getAnimatables(results);
  clearCoat->getAnimatables(results);
  sheen->getAnimatables(results);
  anisotropy->getAnimatables(results);

  return results;
}

BaseTexturePtr PBRBaseMaterial::_getReflectionTexture() const
{
  if (_reflectionTexture) {
    return _reflectionTexture;
  }

  return getScene()->environmentTexture();
}

std::vector<BaseTexturePtr> PBRBaseMaterial::getActiveTextures() const
{
  auto activeTextures = PushMaterial::getActiveTextures();

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

  if (_metallicReflectanceTexture) {
    activeTextures.emplace_back(_metallicReflectanceTexture);
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

  detailMap->getActiveTextures(activeTextures);
  subSurface->getActiveTextures(activeTextures);
  clearCoat->getActiveTextures(activeTextures);
  sheen->getActiveTextures(activeTextures);
  anisotropy->getActiveTextures(activeTextures);

  return activeTextures;
}

bool PBRBaseMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
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

  if (_metallicReflectanceTexture == texture) {
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

  return detailMap->hasTexture(texture) || subSurface->hasTexture(texture)
         || clearCoat->hasTexture(texture) || sheen->hasTexture(texture)
         || anisotropy->hasTexture(texture);
}

bool PBRBaseMaterial::setPrePassRenderer(const PrePassRendererPtr& prePassRenderer)
{
  if (subSurface->isScatteringEnabled()) {
    prePassRenderer->subSurfaceConfiguration->enabled = true;
    prePassRenderer->materialsShouldRenderIrradiance  = true;
  }

  return true;
}

void PBRBaseMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                              bool /*notBoundToMesh*/)
{
  if (forceDisposeTextures) {
    if (_environmentBRDFTexture && getScene()->environmentBRDFTexture != _environmentBRDFTexture) {
      _environmentBRDFTexture->dispose();
    }

    for (const auto& texture :
         {_albedoTexture, _ambientTexture, _opacityTexture, _reflectionTexture, _emissiveTexture,
          _metallicTexture, _reflectivityTexture, _bumpTexture, _lightmapTexture,
          _metallicReflectanceTexture, _microSurfaceTexture}) {
      if (texture) {
        texture->dispose();
      }
    }
  }

  detailMap->dispose(forceDisposeTextures);
  subSurface->dispose(forceDisposeTextures);
  clearCoat->dispose(forceDisposeTextures);
  sheen->dispose(forceDisposeTextures);
  anisotropy->dispose(forceDisposeTextures);

  _renderTargets.clear();

  if (_imageProcessingConfiguration && _imageProcessingObserver) {
    _imageProcessingConfiguration->onUpdateParameters.remove(_imageProcessingObserver);
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

} // end of namespace BABYLON
