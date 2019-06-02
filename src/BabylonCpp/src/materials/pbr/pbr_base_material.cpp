#include <babylon/materials/pbr/pbr_base_material.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/pbr/pbr_material_defines.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/refraction_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/base_sub_mesh.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/texture_tools.h>

namespace BABYLON {

Color3 PBRBaseMaterial::_scaledReflectivity = Color3();

PBRBaseMaterial::PBRBaseMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , transparencyMode{this, &PBRBaseMaterial::get_transparencyMode,
                       &PBRBaseMaterial::set_transparencyMode}
    , _directIntensity{1.f}
    , _emissiveIntensity{1.f}
    , _environmentIntensity{1.f}
    , _specularIntensity{1.f}
    , _disableBumpMap{false}
    , _albedoTexture{nullptr}
    , _ambientTexture{nullptr}
    , _ambientTextureStrength{1.f}
    , _ambientTextureImpactOnAnalyticalLights{PBRMaterial::
                                                DEFAULT_AO_ON_ANALYTICAL_LIGHTS}
    , _opacityTexture{nullptr}
    , _reflectionTexture{nullptr}
    , _refractionTexture{nullptr}
    , _emissiveTexture{nullptr}
    , _reflectivityTexture{nullptr}
    , _metallicTexture{nullptr}
    , _metallic{std::nullopt}
    , _roughness{std::nullopt}
    , _microSurfaceTexture{nullptr}
    , _bumpTexture{nullptr}
    , _lightmapTexture{nullptr}
    , _ambientColor{Color3(0, 0, 0)}
    , _albedoColor{Color3(1, 1, 1)}
    , _reflectivityColor{Color3(1, 1, 1)}
    , _reflectionColor{Color3(1, 1, 1)}
    , _emissiveColor{Color3(0, 0, 0)}
    , _microSurface{0.9f}
    , _indexOfRefraction{0.66f}
    , _invertRefractionY{false}
    , _linkRefractionWithTransparency{false}
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
    , _transparencyMode{std::nullopt}
    , _environmentBRDFTexture{nullptr}
    , _forceIrradianceInFragment{false}
    , _forceNormalForward{false}
    , _enableSpecularAntiAliasing{false}
    , _imageProcessingConfiguration{std::make_shared<
        ImageProcessingConfiguration>()}
    , _unlit{false}
    , _lightingInfos{Vector4(_directIntensity, _emissiveIntensity,
                             _environmentIntensity, _specularIntensity)}
    , _imageProcessingObserver{nullptr}
    , _globalAmbientColor{Color3(0, 0, 0)}
    , _useLogarithmicDepth{false}
{
  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);

  getRenderTargetTextures = [this]() -> std::vector<RenderTargetTexturePtr> {
    _renderTargets.clear();

    if (StandardMaterial::ReflectionTextureEnabled() && _reflectionTexture
        && _reflectionTexture->isRenderTarget) {
      _renderTargets.emplace_back(
        std::static_pointer_cast<RenderTargetTexture>(_reflectionTexture));
    }

    if (StandardMaterial::RefractionTextureEnabled() && _refractionTexture
        && _refractionTexture->isRenderTarget) {
      _renderTargets.emplace_back(
        std::static_pointer_cast<RenderTargetTexture>(_refractionTexture));
    }

    return _renderTargets;
  };

  _environmentBRDFTexture = TextureTools::GetEnvironmentBRDFTexture(scene);
}

PBRBaseMaterial::~PBRBaseMaterial()
{
}

void PBRBaseMaterial::_attachImageProcessingConfiguration(
  const ImageProcessingConfigurationPtr& configuration)
{
  if (configuration == _imageProcessingConfiguration) {
    return;
  }

  // Detaches observer.
  if (_imageProcessingConfiguration && _imageProcessingObserver) {
    _imageProcessingConfiguration->onUpdateParameters.remove(
      _imageProcessingObserver);
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
    _imageProcessingObserver
      = _imageProcessingConfiguration->onUpdateParameters.add(
        [this](ImageProcessingConfiguration* /*conf*/, EventState&) {
          _markAllSubMeshesAsImageProcessingDirty();
        });
  }
}

bool PBRBaseMaterial::get_hasRenderTargetTextures() const
{
  if (StandardMaterial::ReflectionTextureEnabled() && _reflectionTexture
      && _reflectionTexture->isRenderTarget) {
    return true;
  }

  if (StandardMaterial::RefractionTextureEnabled() && _refractionTexture
      && _refractionTexture->isRenderTarget) {
    return true;
  }

  return false;
}

const std::string PBRBaseMaterial::getClassName() const
{
  return "PBRBaseMaterial";
}

bool PBRBaseMaterial::get_useLogarithmicDepth() const
{
  return _useLogarithmicDepth;
}

void PBRBaseMaterial::set_useLogarithmicDepth(bool value)
{
  _useLogarithmicDepth
    = value && getScene()->getEngine()->getCaps().fragmentDepthSupported;
}

std::optional<unsigned int>& PBRBaseMaterial::get_transparencyMode()
{
  return _transparencyMode;
}

void PBRBaseMaterial::set_transparencyMode(
  const std::optional<unsigned int>& value)
{
  if (_transparencyMode == value) {
    return;
  }

  _transparencyMode = value;

  _forceAlphaTest = (*value == PBRMaterial::PBRMATERIAL_ALPHATESTANDBLEND);

  _markAllSubMeshesAsTexturesAndMiscDirty();
}

bool PBRBaseMaterial::_disableAlphaBlending() const
{
  return (_linkRefractionWithTransparency
          || _transparencyMode == PBRMaterial::PBRMATERIAL_OPAQUE
          || _transparencyMode == PBRMaterial::PBRMATERIAL_ALPHATEST);
}

bool PBRBaseMaterial::needAlphaBlending() const
{
  if (_disableAlphaBlending()) {
    return false;
  }
  return (alpha() < 1.f) || (_opacityTexture != nullptr)
         || _shouldUseAlphaFromAlbedoTexture();
}

bool PBRBaseMaterial::needAlphaBlendingForMesh(const AbstractMesh& mesh) const
{
  if (_disableAlphaBlending()) {
    return false;
  }

  return PushMaterial::needAlphaBlendingForMesh(mesh);
}

bool PBRBaseMaterial::needAlphaTesting() const
{
  if (_forceAlphaTest) {
    return true;
  }

  if (_linkRefractionWithTransparency) {
    return false;
  }

  return _albedoTexture != nullptr && _albedoTexture->hasAlpha()
         && (!_transparencyMode.has_value()
             || _transparencyMode == PBRMaterial::PBRMATERIAL_ALPHATEST);
}

bool PBRBaseMaterial::_shouldUseAlphaFromAlbedoTexture() const
{
  return _albedoTexture != nullptr && _albedoTexture->hasAlpha()
         && _useAlphaFromAlbedoTexture
         && _transparencyMode != PBRMaterial::PBRMATERIAL_OPAQUE;
}

BaseTexturePtr PBRBaseMaterial::getAlphaTestTexture()
{
  return _albedoTexture;
}

bool PBRBaseMaterial::isReadyForSubMesh(AbstractMesh* mesh,
                                        BaseSubMesh* subMesh, bool useInstances)
{
  if (subMesh->effect() && isFrozen()) {
    if (_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<PBRMaterialDefines>();
  }

  auto scene = getScene();
  auto definesPtr
    = std::static_pointer_cast<PBRMaterialDefines>(subMesh->_materialDefines);
  auto& defines = *definesPtr.get();
  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (defines._renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  if (defines._areTexturesDirty) {
    if (scene->texturesEnabled()) {
      if (_albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
        if (!_albedoTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
        if (!_ambientTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
        if (!_opacityTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      auto reflectionTexture = _getReflectionTexture();
      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        if (!reflectionTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
        if (!_lightmapTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
        if (!_emissiveTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (StandardMaterial::SpecularTextureEnabled()) {
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

        if (_microSurfaceTexture) {
          if (!_microSurfaceTexture->isReadyOrNotBlocking()) {
            return false;
          }
        }
      }

      if (engine->getCaps().standardDerivatives && _bumpTexture
          && StandardMaterial::BumpTextureEnabled() && !_disableBumpMap) {
        // Bump texture cannot be not blocking.
        if (!_bumpTexture->isReady()) {
          return false;
        }
      }

      auto refractionTexture = _getRefractionTexture();
      if (refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
        if (!refractionTexture->isReadyOrNotBlocking()) {
          return false;
        }
      }

      if (_environmentBRDFTexture
          && StandardMaterial::ReflectionTextureEnabled()) {
        // This is blocking.
        if (!_environmentBRDFTexture->isReady()) {
          return false;
        }
      }
    }
  }

  if (defines._areImageProcessingDirty && _imageProcessingConfiguration) {
    if (!_imageProcessingConfiguration->isReady()) {
      return false;
    }
  }

  if (!engine->getCaps().standardDerivatives
      && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
    mesh->createNormals(true);
    BABYLON_LOGF_WARN("PBRBaseMaterial",
                      "PBRMaterial: Normals have been created for the mesh: %s",
                      mesh->name.c_str());
  }

  auto effect
    = _prepareEffect(mesh, defines, onCompiled, onError, useInstances);
  if (effect) {
    scene->resetCachedMaterial();
    subMesh->setEffect(effect, definesPtr);
    buildUniformLayout();
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  defines._renderId   = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

bool PBRBaseMaterial::isMetallicWorkflow() const
{
  if ((_metallic.has_value() && _metallic.value() != 0.f)
      || (_roughness.has_value() && _roughness.value() != 0.f)
      || _metallicTexture) {
    return true;
  }

  return false;
}

EffectPtr PBRBaseMaterial::_prepareEffect(
  AbstractMesh* mesh, PBRMaterialDefines& defines,
  const std::function<void(Effect* effect)>& iOnCompiled,
  std::function<void(Effect* effect, const std::string& errors)> iOnError,
  const std::optional<bool>& useInstances,
  const std::optional<bool>& useClipPlane)
{
  _prepareDefines(mesh, defines, useInstances, useClipPlane);
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

  if (defines["ENVIRONMENTBRDF"]) {
    fallbacks->addFallback(fallbackRank++, "ENVIRONMENTBRDF");
  }

  if (defines["TANGENT"]) {
    fallbacks->addFallback(fallbackRank++, "TANGENT");
  }

  if (defines["BUMP"]) {
    fallbacks->addFallback(fallbackRank++, "BUMP");
  }

  fallbackRank = MaterialHelper::HandleFallbacksForShadows(
    defines, *fallbacks, _maxSimultaneousLights, fallbackRank++);

  if (defines["SPECULARTERM"]) {
    fallbacks->addFallback(fallbackRank++, "SPECULARTERM");
  }

  if (defines["USESPHERICALFROMREFLECTIONMAP"]) {
    fallbacks->addFallback(fallbackRank++, "USESPHERICALFROMREFLECTIONMAP");
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

  if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
    fallbacks->addCPUSkinningFallback(fallbackRank++, mesh);
  }

  if (defines["MORPHTARGETS"]) {
    fallbacks->addFallback(fallbackRank++, "MORPHTARGETS");
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

  std::vector<std::string> uniforms{"world",
                                    "view",
                                    "viewProjection",
                                    "vEyePosition",
                                    "vLightsType",
                                    "vAmbientColor",
                                    "vAlbedoColor",
                                    "vReflectivityColor",
                                    "vEmissiveColor",
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
                                    "vMicroSurfaceSamplerInfos",
                                    "vBumpInfos",
                                    "vLightmapInfos",
                                    "vRefractionInfos",
                                    "mBones",
                                    "vClipPlane",
                                    "vClipPlane2",
                                    "vClipPlane3",
                                    "vClipPlane4",
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
                                    "refractionMatrix",
                                    "vLightingIntensity",
                                    "logarithmicDepthConstant",
                                    "vSphericalX",
                                    "vSphericalY",
                                    "vSphericalZ",
                                    "vSphericalXX",
                                    "vSphericalYY",
                                    "vSphericalZZ",
                                    "vSphericalXY",
                                    "vSphericalYZ",
                                    "vSphericalZX",
                                    "vReflectionMicrosurfaceInfos",
                                    "vRefractionMicrosurfaceInfos",
                                    "vTangentSpaceParams"};

  std::vector<std::string> samplers{
    "albedoSampler",         "reflectivitySampler", "ambientSampler",
    "emissiveSampler",       "bumpSampler",         "lightmapSampler",
    "opacitySampler",        "refractionSampler",   "refractionSamplerLow",
    "refractionSamplerHigh", "reflectionSampler",   "reflectionSamplerLow",
    "reflectionSamplerHigh", "microSurfaceSampler", "environmentBrdfSampler"};
  std::vector<std::string> uniformBuffers{"Material", "Scene"};

  // if (ImageProcessingConfiguration)
  {
    ImageProcessingConfiguration::PrepareUniforms(uniforms, defines);
    ImageProcessingConfiguration::PrepareSamplers(samplers, defines);
  }

  std::unordered_map<std::string, unsigned int> indexParameters{
    {"maxSimultaneousLights", _maxSimultaneousLights},
    {"maxSimultaneousMorphTargets", defines.intDef["NUM_MORPH_INFLUENCERS"]}};

  auto join = defines.toString();

  EffectCreationOptions options;
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
  options.maxSimultaneousLights = _maxSimultaneousLights;

  MaterialHelper::PrepareUniformsAndSamplersList(options);

  return engine->createEffect("pbr", options, engine);
}

void PBRBaseMaterial::_prepareDefines(AbstractMesh* mesh,
                                      PBRMaterialDefines& defines,
                                      const std::optional<bool>& useInstances,
                                      const std::optional<bool>& useClipPlane)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  // Lights
  MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, true, _maxSimultaneousLights, _disableLighting);
  defines._needNormals = true;

  // Textures
  defines.boolDef["METALLICWORKFLOW"] = isMetallicWorkflow();
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (scene->getEngine()->getCaps().textureLOD) {
        defines.boolDef["LODBASEDMICROSFURACE"] = true;
      }

      if (_albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_albedoTexture, defines,
                                                  "ALBEDO");
      }
      else {
        defines.boolDef["ALBEDO"] = false;
      }

      if (_ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_ambientTexture, defines,
                                                  "AMBIENT");
        defines.boolDef["AMBIENTINGRAYSCALE"] = _useAmbientInGrayScale;
      }
      else {
        defines.boolDef["AMBIENT"] = false;
      }

      if (_opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_opacityTexture, defines,
                                                  "OPACITY");
        defines.boolDef["OPACITYRGB"] = _opacityTexture->getAlphaFromRGB;
      }
      else {
        defines.boolDef["OPACITY"] = false;
      }

      auto reflectionTexture = _getReflectionTexture();
      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        defines.boolDef["REFLECTION"]      = true;
        defines.boolDef["GAMMAREFLECTION"] = reflectionTexture->gammaSpace;
        defines.boolDef["RGBDREFLECTION"]  = reflectionTexture->isRGBD;
        defines.boolDef["REFLECTIONMAP_OPPOSITEZ"]
          = getScene()->useRightHandedSystem() ? !reflectionTexture->invertZ :
                                                 reflectionTexture->invertZ;
        defines.boolDef["LODINREFLECTIONALPHA"]
          = reflectionTexture->lodLevelInAlpha;

        if (reflectionTexture->coordinatesMode()
            == TextureConstants::INVCUBIC_MODE) {
          defines.boolDef["INVERTCUBICMAP"] = true;
        }

        defines.boolDef["REFLECTIONMAP_3D"] = reflectionTexture->isCube;

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
            defines.boolDef["REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED"]
              = true;
            break;
          case TextureConstants::CUBIC_MODE:
          case TextureConstants::INVCUBIC_MODE:
            defines.boolDef["REFLECTIONMAP_CUBIC"] = true;
            defines.boolDef["USE_LOCAL_REFLECTIONMAP_CUBIC"]
              = reflectionTexture->boundingBoxSize() ? true : false;
            break;
        }

        if (reflectionTexture->coordinatesMode()
            != TextureConstants::SKYBOX_MODE) {
          if (reflectionTexture->sphericalPolynomial()) {
            defines.boolDef["USESPHERICALFROMREFLECTIONMAP"] = true;
            if (_forceIrradianceInFragment
                || scene->getEngine()->getCaps().maxVaryingVectors <= 8) {
              defines.boolDef["USESPHERICALINVERTEX"] = false;
            }
            else {
              defines.boolDef["USESPHERICALINVERTEX"] = true;
            }
          }
        }
        else {
          defines.boolDef["REFLECTIONMAP_SKYBOX_TRANSFORMED"]
            = !reflectionTexture->getReflectionTextureMatrix()->isIdentity();
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
        defines.boolDef["REFLECTIONMAP_SKYBOX_TRANSFORMED"]            = false;
        defines.boolDef["REFLECTIONMAP_EXPLICIT"]                      = false;
        defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR"]               = false;
        defines.boolDef["REFLECTIONMAP_EQUIRECTANGULAR_FIXED"]         = false;
        defines.boolDef["REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED"] = false;
        defines.boolDef["INVERTCUBICMAP"]                              = false;
        defines.boolDef["USESPHERICALFROMREFLECTIONMAP"]               = false;
        defines.boolDef["USESPHERICALINVERTEX"]                        = false;
        defines.boolDef["REFLECTIONMAP_OPPOSITEZ"]                     = false;
        defines.boolDef["LODINREFLECTIONALPHA"]                        = false;
        defines.boolDef["GAMMAREFLECTION"]                             = false;
        defines.boolDef["RGBDREFLECTION"]                              = false;
      }

      if (_lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_lightmapTexture, defines,
                                                  "LIGHTMAP");
        defines.boolDef["USELIGHTMAPASSHADOWMAP"] = _useLightmapAsShadowmap;
        defines.boolDef["GAMMALIGHTMAP"] = _lightmapTexture->gammaSpace;
      }
      else {
        defines.boolDef["LIGHTMAP"] = false;
      }

      if (_emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_emissiveTexture, defines,
                                                  "EMISSIVE");
      }
      else {
        defines.boolDef["EMISSIVE"] = false;
      }

      if (StandardMaterial::SpecularTextureEnabled()) {
        if (_metallicTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(_metallicTexture, defines,
                                                    "REFLECTIVITY");
          defines.boolDef["ROUGHNESSSTOREINMETALMAPALPHA"]
            = _useRoughnessFromMetallicTextureAlpha;
          defines.boolDef["ROUGHNESSSTOREINMETALMAPGREEN"]
            = !_useRoughnessFromMetallicTextureAlpha
              && _useRoughnessFromMetallicTextureGreen;
          defines.boolDef["METALLNESSSTOREINMETALMAPBLUE"]
            = _useMetallnessFromMetallicTextureBlue;
          defines.boolDef["AOSTOREINMETALMAPRED"]
            = _useAmbientOcclusionFromMetallicTextureRed;
        }
        else if (_reflectivityTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(_reflectivityTexture,
                                                    defines, "REFLECTIVITY");
          defines.boolDef["MICROSURFACEFROMREFLECTIVITYMAP"]
            = _useMicroSurfaceFromReflectivityMapAlpha;
          defines.boolDef["MICROSURFACEAUTOMATIC"]
            = _useAutoMicroSurfaceFromReflectivityMap;
        }
        else {
          defines.boolDef["REFLECTIVITY"] = false;
        }

        if (_microSurfaceTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(_microSurfaceTexture,
                                                    defines, "MICROSURFACEMAP");
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
          && StandardMaterial::BumpTextureEnabled() && !_disableBumpMap) {
        MaterialHelper::PrepareDefinesForMergedUV(_bumpTexture, defines,
                                                  "BUMP");

        if (_useParallax && _albedoTexture
            && StandardMaterial::DiffuseTextureEnabled()) {
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

      auto refractionTexture = _getRefractionTexture();
      if (refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
        defines.boolDef["REFRACTION"]       = true;
        defines.boolDef["REFRACTIONMAP_3D"] = refractionTexture->isCube;
        defines.boolDef["GAMMAREFRACTION"]  = refractionTexture->gammaSpace;
        defines.boolDef["RGBDREFRACTION"]   = refractionTexture->isRGBD;
        defines.boolDef["REFRACTIONMAP_OPPOSITEZ"] = refractionTexture->invertZ;
        defines.boolDef["LODINREFRACTIONALPHA"]
          = refractionTexture->lodLevelInAlpha;

        if (_linkRefractionWithTransparency) {
          defines.boolDef["LINKREFRACTIONTOTRANSPARENCY"] = true;
        }
      }
      else {
        defines.boolDef["REFRACTION"] = false;
      }

      if (_environmentBRDFTexture
          && StandardMaterial::ReflectionTextureEnabled()) {
        defines.boolDef["ENVIRONMENTBRDF"] = true;
      }
      else {
        defines.boolDef["ENVIRONMENTBRDF"] = false;
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

    defines.stringDef["ALPHATESTVALUE"]
      = std::to_string(_alphaCutOff)
        + (std::fmod(_alphaCutOff, 1.f) == 0.f ? "." : "");
    defines.boolDef["PREMULTIPLYALPHA"]
      = (alphaMode() == EngineConstants::ALPHA_PREMULTIPLIED
         || alphaMode() == EngineConstants::ALPHA_PREMULTIPLIED_PORTERDUFF);
    defines.boolDef["ALPHABLEND"] = needAlphaBlendingForMesh(*mesh);
    defines.boolDef["ALPHAFRESNEL"]
      = _useAlphaFresnel || _useLinearAlphaFresnel;
    defines.boolDef["LINEARALPHAFRESNEL"] = _useLinearAlphaFresnel;

    defines.boolDef["SPECULARAA"]
      = scene->getEngine()->getCaps().standardDerivatives
        && _enableSpecularAntiAliasing;
  }

  if (defines._areImageProcessingDirty && _imageProcessingConfiguration) {
    _imageProcessingConfiguration->prepareDefines(defines);
  }

  defines.boolDef["FORCENORMALFORWARD"] = _forceNormalForward;

  defines.boolDef["RADIANCEOCCLUSION"] = _useRadianceOcclusion;

  defines.boolDef["HORIZONOCCLUSION"] = _useHorizonOcclusion;

  // Misc.
  if (defines._areMiscDirty) {
    MaterialHelper::PrepareDefinesForMisc(
      mesh, scene, _useLogarithmicDepth, pointsCloud(), fogEnabled(),
      _shouldTurnAlphaTestOn(mesh) || _forceAlphaTest, defines);
    defines.boolDef["UNLIT"]
      = _unlit
        || ((pointsCloud() || wireframe())
            && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind));
  }

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines,
    (useInstances.has_value() && (*useInstances)) ? true : false, useClipPlane);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, true, true, true,
    _transparencyMode != PBRMaterial::PBRMATERIAL_OPAQUE);
}

void PBRBaseMaterial::forceCompilation(
  AbstractMesh* mesh, std::function<void(Material* material)>& iOnCompiled,
  bool clipPlane)
{
  PBRMaterialDefines defines;
  auto effect
    = _prepareEffect(mesh, defines, nullptr, nullptr, std::nullopt, clipPlane);
  if (effect->isReady()) {
    if (iOnCompiled) {
      iOnCompiled(this);
    }
  }
  else {
    effect->onCompileObservable.add(
      [&](Effect* /*effect*/, EventState& /*es*/) {
        if (iOnCompiled) {
          iOnCompiled(this);
        }
      });
  }
}

void PBRBaseMaterial::buildUniformLayout()
{
  // Order is important !
  _uniformBuffer->addUniform("vAlbedoInfos", 2);
  _uniformBuffer->addUniform("vAmbientInfos", 4);
  _uniformBuffer->addUniform("vOpacityInfos", 2);
  _uniformBuffer->addUniform("vEmissiveInfos", 2);
  _uniformBuffer->addUniform("vLightmapInfos", 2);
  _uniformBuffer->addUniform("vReflectivityInfos", 3);
  _uniformBuffer->addUniform("vMicroSurfaceSamplerInfos", 2);
  _uniformBuffer->addUniform("vRefractionInfos", 4);
  _uniformBuffer->addUniform("vReflectionInfos", 2);
  _uniformBuffer->addUniform("vReflectionPosition", 3);
  _uniformBuffer->addUniform("vReflectionSize", 3);
  _uniformBuffer->addUniform("vBumpInfos", 3);
  _uniformBuffer->addUniform("albedoMatrix", 16);
  _uniformBuffer->addUniform("ambientMatrix", 16);
  _uniformBuffer->addUniform("opacityMatrix", 16);
  _uniformBuffer->addUniform("emissiveMatrix", 16);
  _uniformBuffer->addUniform("lightmapMatrix", 16);
  _uniformBuffer->addUniform("reflectivityMatrix", 16);
  _uniformBuffer->addUniform("microSurfaceSamplerMatrix", 16);
  _uniformBuffer->addUniform("bumpMatrix", 16);
  _uniformBuffer->addUniform("vTangentSpaceParams", 2);
  _uniformBuffer->addUniform("refractionMatrix", 16);
  _uniformBuffer->addUniform("reflectionMatrix", 16);

  _uniformBuffer->addUniform("vReflectionColor", 3);
  _uniformBuffer->addUniform("vAlbedoColor", 4);
  _uniformBuffer->addUniform("vLightingIntensity", 4);

  _uniformBuffer->addUniform("vRefractionMicrosurfaceInfos", 3);
  _uniformBuffer->addUniform("vReflectionMicrosurfaceInfos", 3);
  _uniformBuffer->addUniform("vReflectivityColor", 4);
  _uniformBuffer->addUniform("vEmissiveColor", 3);

  _uniformBuffer->addUniform("pointSize", 1);
  _uniformBuffer->create();
}

void PBRBaseMaterial::unbind()
{
  if (_reflectionTexture && _reflectionTexture->isRenderTarget) {
    _uniformBuffer->setTexture("reflectionSampler", nullptr);
  }

  if (_refractionTexture && _refractionTexture->isRenderTarget) {
    _uniformBuffer->setTexture("refractionSampler", nullptr);
  }

  PushMaterial::unbind();
}

void PBRBaseMaterial::bindForSubMesh(Matrix& world, Mesh* mesh,
                                     SubMesh* subMesh)
{
  auto scene = getScene();

  auto definesTmp
    = static_cast<PBRMaterialDefines*>(subMesh->_materialDefines.get());
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

  // Normal Matrix
  if (defines["OBJECTSPACE_NORMALMAP"]) {
    world.toNormalMatrix(_normalMatrix);
    bindOnlyNormalMatrix(_normalMatrix);
  }

  const auto mustRebind = _mustRebind(scene, effect, mesh->visibility());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, effect);

  if (mustRebind) {
    _uniformBuffer->bindToEffect(effect.get(), "Material");

    bindViewProjection(effect);

    auto reflectionTexture = _getReflectionTexture();
    auto refractionTexture = _getRefractionTexture();

    if (!_uniformBuffer->useUbo() || !isFrozen() || !_uniformBuffer->isSync()) {

      // Texture uniforms
      if (scene->texturesEnabled()) {
        if (_albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
          _uniformBuffer->updateFloat2(
            "vAlbedoInfos",
            static_cast<float>(_albedoTexture->coordinatesIndex),
            static_cast<float>(_albedoTexture->level), "");
          MaterialHelper::BindTextureMatrix(*_albedoTexture, *_uniformBuffer,
                                            "albedo");
        }

        if (_ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
          _uniformBuffer->updateFloat4(
            "vAmbientInfos",
            static_cast<float>(_ambientTexture->coordinatesIndex),
            static_cast<float>(_ambientTexture->level), _ambientTextureStrength,
            static_cast<float>(_ambientTextureImpactOnAnalyticalLights), "");
          MaterialHelper::BindTextureMatrix(*_ambientTexture, *_uniformBuffer,
                                            "ambient");
        }

        if (_opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
          _uniformBuffer->updateFloat2(
            "vOpacityInfos",
            static_cast<float>(_opacityTexture->coordinatesIndex),
            static_cast<float>(_opacityTexture->level), "");
          MaterialHelper::BindTextureMatrix(*_opacityTexture, *_uniformBuffer,
                                            "opacity");
        }

        if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
          _uniformBuffer->updateMatrix(
            "reflectionMatrix",
            *reflectionTexture->getReflectionTextureMatrix());
          _uniformBuffer->updateFloat2("vReflectionInfos",
                                       reflectionTexture->level, 0, "");

          if (reflectionTexture->boundingBoxSize()) {
            if (auto cubeTexture
                = std::static_pointer_cast<CubeTexture>(reflectionTexture)) {
              _uniformBuffer->updateVector3("vReflectionPosition",
                                            cubeTexture->boundingBoxPosition);
              _uniformBuffer->updateVector3("vReflectionSize",
                                            *cubeTexture->boundingBoxSize());
            }
          }

          auto _polynomials = reflectionTexture->sphericalPolynomial();
          if (defines["USESPHERICALFROMREFLECTIONMAP"] && _polynomials) {
            auto polynomials = *_polynomials;
            _activeEffect->setFloat3("vSphericalX", polynomials.x.x,
                                     polynomials.x.y, polynomials.x.z);
            _activeEffect->setFloat3("vSphericalY", polynomials.y.x,
                                     polynomials.y.y, polynomials.y.z);
            _activeEffect->setFloat3("vSphericalZ", polynomials.z.x,
                                     polynomials.z.y, polynomials.z.z);
            _activeEffect->setFloat3("vSphericalXX_ZZ",
                                     polynomials.xx.x - polynomials.zz.x,
                                     polynomials.xx.y - polynomials.zz.y,
                                     polynomials.xx.z - polynomials.zz.z);
            _activeEffect->setFloat3("vSphericalYY_ZZ",
                                     polynomials.yy.x - polynomials.zz.x,
                                     polynomials.yy.y - polynomials.zz.y,
                                     polynomials.yy.z - polynomials.zz.z);
            _activeEffect->setFloat3("vSphericalZZ", polynomials.zz.x,
                                     polynomials.zz.y, polynomials.zz.z);
            _activeEffect->setFloat3("vSphericalXY", polynomials.xy.x,
                                     polynomials.xy.y, polynomials.xy.z);
            _activeEffect->setFloat3("vSphericalYZ", polynomials.yz.x,
                                     polynomials.yz.y, polynomials.yz.z);
            _activeEffect->setFloat3("vSphericalZX", polynomials.zx.x,
                                     polynomials.zx.y, polynomials.zx.z);
          }

          _uniformBuffer->updateFloat3(
            "vReflectionMicrosurfaceInfos",
            static_cast<float>(reflectionTexture->getSize().width),
            reflectionTexture->lodGenerationScale,
            reflectionTexture->lodGenerationOffset, "");
        }

        if (_emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
          _uniformBuffer->updateFloat2(
            "vEmissiveInfos",
            static_cast<float>(_emissiveTexture->coordinatesIndex),
            static_cast<float>(_emissiveTexture->level), "");
          MaterialHelper::BindTextureMatrix(*_emissiveTexture, *_uniformBuffer,
                                            "emissive");
        }

        if (_lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
          _uniformBuffer->updateFloat2(
            "vLightmapInfos",
            static_cast<float>(_lightmapTexture->coordinatesIndex),
            static_cast<float>(_lightmapTexture->level), "");
          MaterialHelper::BindTextureMatrix(*_lightmapTexture, *_uniformBuffer,
                                            "lightmap");
        }

        if (StandardMaterial::SpecularTextureEnabled()) {
          if (_metallicTexture) {
            _uniformBuffer->updateFloat3(
              "vReflectivityInfos",
              static_cast<float>(_metallicTexture->coordinatesIndex),
              static_cast<float>(_metallicTexture->level),
              _ambientTextureStrength, "");
            MaterialHelper::BindTextureMatrix(*_metallicTexture,
                                              *_uniformBuffer, "reflectivity");
          }
          else if (_reflectivityTexture) {
            _uniformBuffer->updateFloat3(
              "vReflectivityInfos",
              static_cast<float>(_reflectivityTexture->coordinatesIndex),
              static_cast<float>(_reflectivityTexture->level), 1.f, "");
            MaterialHelper::BindTextureMatrix(*_reflectivityTexture,
                                              *_uniformBuffer, "reflectivity");
          }

          if (_microSurfaceTexture) {
            _uniformBuffer->updateFloat2(
              "vMicroSurfaceSamplerInfos",
              static_cast<float>(_microSurfaceTexture->coordinatesIndex),
              static_cast<float>(_microSurfaceTexture->level), "");
            MaterialHelper::BindTextureMatrix(
              *_microSurfaceTexture, *_uniformBuffer, "microSurfaceSampler");
          }
        }

        if (_bumpTexture && scene->getEngine()->getCaps().standardDerivatives
            && StandardMaterial::BumpTextureEnabled() && !_disableBumpMap) {
          _uniformBuffer->updateFloat3(
            "vBumpInfos", static_cast<float>(_bumpTexture->coordinatesIndex),
            static_cast<float>(_bumpTexture->level), _parallaxScaleBias, "");
          MaterialHelper::BindTextureMatrix(*_bumpTexture, *_uniformBuffer,
                                            "bump");
          if (scene->_mirroredCameraPosition) {
            _uniformBuffer->updateFloat2("vTangentSpaceParams",
                                         _invertNormalMapX ? 1.f : -1.f,
                                         _invertNormalMapY ? 1.f : -1.f, "");
          }
          else {
            _uniformBuffer->updateFloat2("vTangentSpaceParams",
                                         _invertNormalMapX ? -1.f : 1.f,
                                         _invertNormalMapY ? -1.f : 1.f, "");
          }
        }

        if (refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
          _uniformBuffer->updateMatrix(
            "refractionMatrix",
            *refractionTexture->getReflectionTextureMatrix());

          float depth = 1.f;
          if (!_refractionTexture->isCube) {
            auto refractionTextureTmp
              = std::static_pointer_cast<RefractionTexture>(_refractionTexture);
            if (refractionTextureTmp) {
              depth = refractionTextureTmp->depth;
            }
          }
          _uniformBuffer->updateFloat4(
            "vRefractionInfos", refractionTexture->level, _indexOfRefraction,
            depth, _invertRefractionY ? -1.f : 1.f, "");
          _uniformBuffer->updateFloat3(
            "vRefractionMicrosurfaceInfos",
            static_cast<float>(refractionTexture->getSize().width),
            refractionTexture->lodGenerationScale,
            refractionTexture->lodGenerationOffset, "");
        }
      }

      // Point size
      if (pointsCloud()) {
        _uniformBuffer->updateFloat("pointSize", pointSize);
      }

      // Colors
      if (defines["METALLICWORKFLOW"]) {
        PBRMaterial::_scaledReflectivity.r
          = (!_metallic.has_value()) ? 1.f : *_metallic;
        PBRMaterial::_scaledReflectivity.g
          = (!_roughness.has_value()) ? 1.f : *_roughness;
        _uniformBuffer->updateColor4("vReflectivityColor",
                                     PBRMaterial::_scaledReflectivity, 0, "");
      }
      else {
        _uniformBuffer->updateColor4("vReflectivityColor", _reflectivityColor,
                                     _microSurface, "");
      }

      _uniformBuffer->updateColor3("vEmissiveColor", _emissiveColor, "");
      _uniformBuffer->updateColor3("vReflectionColor", _reflectionColor, "");
      _uniformBuffer->updateColor4("vAlbedoColor", _albedoColor,
                                   alpha() * mesh->visibility(), "");

      // Misc
      _lightingInfos.x = _directIntensity;
      _lightingInfos.y = _emissiveIntensity;
      _lightingInfos.z = _environmentIntensity;
      _lightingInfos.w = _specularIntensity;

      _uniformBuffer->updateVector4("vLightingIntensity", _lightingInfos);
    }

    // Textures
    if (scene->texturesEnabled()) {
      if (_albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
        _uniformBuffer->setTexture("albedoSampler", _albedoTexture);
      }

      if (_ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
        _uniformBuffer->setTexture("ambientSampler", _ambientTexture);
      }

      if (_opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
        _uniformBuffer->setTexture("opacitySampler", _opacityTexture);
      }

      if (_reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        if (defines["LODBASEDMICROSFURACE"]) {
          _uniformBuffer->setTexture("reflectionSampler", _reflectionTexture);
        }
        else {
          _uniformBuffer->setTexture("reflectionSampler",
                                     _reflectionTexture->_lodTextureMid() ?
                                       _reflectionTexture->_lodTextureMid() :
                                       _reflectionTexture);
          _uniformBuffer->setTexture("reflectionSamplerLow",
                                     _reflectionTexture->_lodTextureLow() ?
                                       _reflectionTexture->_lodTextureLow() :
                                       _reflectionTexture);
          _uniformBuffer->setTexture("reflectionSamplerHigh",
                                     _reflectionTexture->_lodTextureHigh() ?
                                       _reflectionTexture->_lodTextureHigh() :
                                       _reflectionTexture);
        }
      }

      if (defines["ENVIRONMENTBRDF"]) {
        _uniformBuffer->setTexture("environmentBrdfSampler",
                                   _environmentBRDFTexture);
      }

      if (_refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
        if (defines["LODBASEDMICROSFURACE"]) {
          _uniformBuffer->setTexture("refractionSampler", _refractionTexture);
        }
        else {
          _uniformBuffer->setTexture("refractionSampler",
                                     _refractionTexture->_lodTextureMid() ?
                                       _refractionTexture->_lodTextureMid() :
                                       _refractionTexture);
          _uniformBuffer->setTexture("refractionSamplerLow",
                                     _refractionTexture->_lodTextureLow() ?
                                       _refractionTexture->_lodTextureLow() :
                                       _refractionTexture);
          _uniformBuffer->setTexture("refractionSamplerHigh",
                                     _refractionTexture->_lodTextureHigh() ?
                                       _refractionTexture->_lodTextureHigh() :
                                       _refractionTexture);
        }
      }

      if (_emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
        _uniformBuffer->setTexture("emissiveSampler", _emissiveTexture);
      }

      if (_lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
        _uniformBuffer->setTexture("lightmapSampler", _lightmapTexture);
      }

      if (StandardMaterial::SpecularTextureEnabled()) {
        if (_metallicTexture) {
          _uniformBuffer->setTexture("reflectivitySampler", _metallicTexture);
        }
        else if (_reflectivityTexture) {
          _uniformBuffer->setTexture("reflectivitySampler",
                                     _reflectivityTexture);
        }

        if (_microSurfaceTexture) {
          _uniformBuffer->setTexture("microSurfaceSampler",
                                     _microSurfaceTexture);
        }
      }

      if (_bumpTexture && scene->getEngine()->getCaps().standardDerivatives
          && StandardMaterial::BumpTextureEnabled() && !_disableBumpMap) {
        _uniformBuffer->setTexture("bumpSampler", _bumpTexture);
      }
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Colors
    scene->ambientColor.multiplyToRef(_ambientColor, _globalAmbientColor);

    auto eyePosition = scene->_forcedViewPosition ?
                         *scene->_forcedViewPosition :
                         (scene->_mirroredCameraPosition ?
                            *scene->_mirroredCameraPosition :
                            scene->activeCamera->globalPosition());
    auto invertNormal = (scene->useRightHandedSystem()
                         == (scene->_mirroredCameraPosition != nullptr));
    effect->setFloat4("vEyePosition", eyePosition.x, eyePosition.y,
                      eyePosition.z, invertNormal ? -1.f : 1.f);
    effect->setColor3("vAmbientColor", _globalAmbientColor);
  }

  if (mustRebind || !isFrozen()) {
    // Lights
    if (scene->lightsEnabled() && !_disableLighting) {
      MaterialHelper::BindLights(
        scene, mesh, _activeEffect, defines, _maxSimultaneousLights,
        _lightFalloff != PBRBaseMaterial::LIGHTFALLOFF_STANDARD);
    }

    // View
    if ((scene->fogEnabled() && mesh->applyFog()
         && scene->fogMode() != Scene::FOGMODE_NONE)
        || _reflectionTexture) {
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

  _uniformBuffer->update();

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> PBRBaseMaterial::getAnimatables() const
{
  std::vector<IAnimatablePtr> results;

  if (_albedoTexture && _albedoTexture->animations.size() > 0) {
    results.emplace_back(_albedoTexture);
  }

  if (_ambientTexture && _ambientTexture->animations.size() > 0) {
    results.emplace_back(_ambientTexture);
  }

  if (_opacityTexture && _opacityTexture->animations.size() > 0) {
    results.emplace_back(_opacityTexture);
  }

  if (_reflectionTexture && _reflectionTexture->animations.size() > 0) {
    results.emplace_back(_reflectionTexture);
  }

  if (_emissiveTexture && _emissiveTexture->animations.size() > 0) {
    results.emplace_back(_emissiveTexture);
  }

  if (_metallicTexture && _metallicTexture->animations.size() > 0) {
    results.emplace_back(_metallicTexture);
  }
  else if (_reflectivityTexture
           && _reflectivityTexture->animations.size() > 0) {
    results.emplace_back(_reflectivityTexture);
  }

  if (_bumpTexture && _bumpTexture->animations.size() > 0) {
    results.emplace_back(_bumpTexture);
  }

  if (_lightmapTexture && _lightmapTexture->animations.size() > 0) {
    results.emplace_back(_lightmapTexture);
  }

  if (_refractionTexture && _refractionTexture->animations.size() > 0) {
    results.emplace_back(_refractionTexture);
  }

  return results;
}

BaseTexturePtr PBRBaseMaterial::_getReflectionTexture() const
{
  if (_reflectionTexture) {
    return _reflectionTexture;
  }

  return getScene()->environmentTexture();
}

BaseTexturePtr PBRBaseMaterial::_getRefractionTexture() const
{
  if (_refractionTexture) {
    return _refractionTexture;
  }

  if (_linkRefractionWithTransparency) {
    return getScene()->environmentTexture();
  }

  return nullptr;
}

void PBRBaseMaterial::dispose(bool forceDisposeEffect,
                              bool forceDisposeTextures)
{
  if (forceDisposeTextures) {
    if (_albedoTexture) {
      _albedoTexture->dispose();
    }

    if (_ambientTexture) {
      _ambientTexture->dispose();
    }

    if (_opacityTexture) {
      _opacityTexture->dispose();
    }

    if (_reflectionTexture) {
      _reflectionTexture->dispose();
    }

    if (_environmentBRDFTexture
        && getScene()->environmentBRDFTexture != _environmentBRDFTexture) {
      _environmentBRDFTexture->dispose();
    }

    if (_emissiveTexture) {
      _emissiveTexture->dispose();
    }

    if (_metallicTexture) {
      _metallicTexture->dispose();
    }

    if (_reflectivityTexture) {
      _reflectivityTexture->dispose();
    }

    if (_bumpTexture) {
      _bumpTexture->dispose();
    }

    if (_lightmapTexture) {
      _lightmapTexture->dispose();
    }

    if (_refractionTexture) {
      _refractionTexture->dispose();
    }
  }

  _renderTargets.clear();

  if (_imageProcessingConfiguration && _imageProcessingObserver) {
    _imageProcessingConfiguration->onUpdateParameters.remove(
      _imageProcessingObserver);
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

} // end of namespace BABYLON
