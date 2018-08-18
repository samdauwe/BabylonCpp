#include <babylon/materials/pbr/pbr_base_material.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
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
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/base_sub_mesh.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/instanced_mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/tools/texture_tools.h>

namespace BABYLON {

Color3 PBRBaseMaterial::_scaledReflectivity = Color3();

PBRBaseMaterial::PBRBaseMaterial(const string_t& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , _directIntensity{1.f}
    , _emissiveIntensity{1.f}
    , _environmentIntensity{1.f}
    , _specularIntensity{1.f}
    , _disableBumpMap{false}
    , _albedoTexture{nullptr}
    , _ambientTexture{nullptr}
    , _ambientTextureStrength{1.f}
    , _opacityTexture{nullptr}
    , _reflectionTexture{nullptr}
    , _refractionTexture{nullptr}
    , _emissiveTexture{nullptr}
    , _reflectivityTexture{nullptr}
    , _metallicTexture{nullptr}
    , _metallic{0.f}
    , _roughness{0.f}
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
    , _transparencyMode{nullptr}
    , _environmentBRDFTexture{nullptr}
    , _forceIrradianceInFragment{false}
    , _forceNormalForward{false}
    , _enableSpecularAntiAliasing{false}
    , _lightingInfos{Vector4(_directIntensity, _emissiveIntensity,
                             _environmentIntensity, _specularIntensity)}
    , _imageProcessingObserver{nullptr}
    , _globalAmbientColor{Color3(0, 0, 0)}
    , _unlit{false}
{
  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);

  getRenderTargetTextures = [this]() {
    _renderTargets.clear();

    if (StandardMaterial::ReflectionTextureEnabled() && _reflectionTexture
        && _reflectionTexture->isRenderTarget) {
      _renderTargets.emplace_back(
        static_cast<RenderTargetTexture*>(_reflectionTexture));
    }

    if (StandardMaterial::RefractionTextureEnabled() && _refractionTexture
        && _refractionTexture->isRenderTarget) {
      _renderTargets.emplace_back(
        static_cast<RenderTargetTexture*>(_refractionTexture));
    }

    return _renderTargets;
  };

  _environmentBRDFTexture = TextureTools::GetEnvironmentBRDFTexture(scene);
}

PBRBaseMaterial::~PBRBaseMaterial()
{
}

void PBRBaseMaterial::_attachImageProcessingConfiguration(
  ImageProcessingConfiguration* configuration)
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
    _imageProcessingConfiguration
      = getScene()->imageProcessingConfiguration().get();
  }
  else {
    _imageProcessingConfiguration = configuration;
  }

  // Attaches observer.
  if (_imageProcessingConfiguration) {
    _imageProcessingObserver
      = _imageProcessingConfiguration->onUpdateParameters.add(
        [this](ImageProcessingConfiguration*, EventState&) {
          _markAllSubMeshesAsImageProcessingDirty();
        });
  }
}

const string_t PBRBaseMaterial::getClassName() const
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

Nullable<unsigned int> PBRBaseMaterial::transparencyMode() const
{
  return _transparencyMode;
}

void PBRBaseMaterial::setTransparencyMode(const Nullable<unsigned int>& value)
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

bool PBRBaseMaterial::needAlphaBlendingForMesh(AbstractMesh* mesh) const
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
         && (_transparencyMode == nullptr
             || _transparencyMode == PBRMaterial::PBRMATERIAL_ALPHATEST);
}

bool PBRBaseMaterial::_shouldUseAlphaFromAlbedoTexture() const
{
  return _albedoTexture != nullptr && _albedoTexture->hasAlpha()
         && _useAlphaFromAlbedoTexture
         && _transparencyMode != PBRMaterial::PBRMATERIAL_OPAQUE;
}

BaseTexture* PBRBaseMaterial::getAlphaTestTexture()
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
    subMesh->_materialDefines = ::std::make_unique<PBRMaterialDefines>();
  }

  auto scene = getScene();
  auto defines
    = *(static_cast<PBRMaterialDefines*>(subMesh->_materialDefines.get()));
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
    subMesh->setEffect(effect, defines);
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
  if (_metallic != 0.f || _roughness != 0.f || _metallicTexture) {
    return true;
  }

  return false;
}

Effect* PBRBaseMaterial::_prepareEffect(
  AbstractMesh* mesh, PBRMaterialDefines& defines,
  const ::std::function<void(Effect* effect)> onCompiled,
  ::std::function<void(Effect* effect, const string_t& errors)> onError,
  const Nullable<bool>& useInstances, const Nullable<bool>& useClipPlane)
{
  _prepareDefines(mesh, defines, useInstances, useClipPlane);
  if (!defines.isDirty()) {
    return nullptr;
  }

  defines.markAsProcessed();

  auto scene  = getScene();
  auto engine = scene->getEngine();

  // Fallbacks
  auto fallbacks    = ::std::make_unique<EffectFallbacks>();
  auto fallbackRank = 0u;
  if (defines[PMD::USESPHERICALINVERTEX]) {
    fallbacks->addFallback(fallbackRank++, "USESPHERICALINVERTEX");
  }

  if (defines[PMD::FOG]) {
    fallbacks->addFallback(fallbackRank, "FOG");
  }
  if (defines[PMD::SPECULARAA]) {
    fallbacks->addFallback(fallbackRank, "SPECULARAA");
  }
  if (defines[PMD::POINTSIZE]) {
    fallbacks->addFallback(fallbackRank, "POINTSIZE");
  }
  if (defines[PMD::LOGARITHMICDEPTH]) {
    fallbacks->addFallback(fallbackRank, "LOGARITHMICDEPTH");
  }
  if (defines[PMD::PARALLAX]) {
    fallbacks->addFallback(fallbackRank, "PARALLAX");
  }
  if (defines[PMD::PARALLAXOCCLUSION]) {
    fallbacks->addFallback(fallbackRank++, "PARALLAXOCCLUSION");
  }

  if (defines[PMD::ENVIRONMENTBRDF]) {
    fallbacks->addFallback(fallbackRank++, "ENVIRONMENTBRDF");
  }

  if (defines[PMD::TANGENT]) {
    fallbacks->addFallback(fallbackRank++, "TANGENT");
  }

  if (defines[PMD::BUMP]) {
    fallbacks->addFallback(fallbackRank++, "BUMP");
  }

  fallbackRank = MaterialHelper::HandleFallbacksForShadows(
    defines, *fallbacks, _maxSimultaneousLights, fallbackRank++);

  if (defines[PMD::SPECULARTERM]) {
    fallbacks->addFallback(fallbackRank++, "SPECULARTERM");
  }

  if (defines[PMD::USESPHERICALFROMREFLECTIONMAP]) {
    fallbacks->addFallback(fallbackRank++, "USESPHERICALFROMREFLECTIONMAP");
  }

  if (defines[PMD::LIGHTMAP]) {
    fallbacks->addFallback(fallbackRank++, "LIGHTMAP");
  }

  if (defines[PMD::NORMAL]) {
    fallbacks->addFallback(fallbackRank++, "NORMAL");
  }

  if (defines[PMD::AMBIENT]) {
    fallbacks->addFallback(fallbackRank++, "AMBIENT");
  }

  if (defines[PMD::EMISSIVE]) {
    fallbacks->addFallback(fallbackRank++, "EMISSIVE");
  }

  if (defines[PMD::VERTEXCOLOR]) {
    fallbacks->addFallback(fallbackRank++, "VERTEXCOLOR");
  }

  if (defines.NUM_BONE_INFLUENCERS > 0) {
    fallbacks->addCPUSkinningFallback(fallbackRank++, mesh);
  }

  if (defines[PMD::MORPHTARGETS]) {
    fallbacks->addFallback(fallbackRank++, "MORPHTARGETS");
  }

  // Attributes
  vector_t<string_t> attribs{VertexBuffer::PositionKindChars};

  if (defines[PMD::NORMAL]) {
    attribs.emplace_back(VertexBuffer::NormalKindChars);
  }

  if (defines[PMD::TANGENT]) {
    attribs.emplace_back(VertexBuffer::TangentKindChars);
  }

  if (defines[PMD::UV1]) {
    attribs.emplace_back(VertexBuffer::UVKindChars);
  }

  if (defines[PMD::UV2]) {
    attribs.emplace_back(VertexBuffer::UV2KindChars);
  }

  if (defines[PMD::VERTEXCOLOR]) {
    attribs.emplace_back(VertexBuffer::ColorKindChars);
  }

  MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines, *fallbacks);
  MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                PMD::INSTANCES);
  MaterialHelper::PrepareAttributesForMorphTargets(attribs, mesh, defines,
                                                   PMD::NORMAL);

  vector_t<string_t> uniforms{"world",
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

  vector_t<string_t> samplers{
    "albedoSampler",         "reflectivitySampler", "ambientSampler",
    "emissiveSampler",       "bumpSampler",         "lightmapSampler",
    "opacitySampler",        "refractionSampler",   "refractionSamplerLow",
    "refractionSamplerHigh", "reflectionSampler",   "reflectionSamplerLow",
    "reflectionSamplerHigh", "microSurfaceSampler", "environmentBrdfSampler"};
  vector_t<string_t> uniformBuffers{"Material", "Scene"};

  // if (ImageProcessingConfiguration)
  {
    ImageProcessingConfiguration::PrepareUniforms(uniforms, defines);
    ImageProcessingConfiguration::PrepareSamplers(samplers, defines);
  }

  unordered_map_t<string_t, unsigned int> indexParameters{
    {"maxSimultaneousLights", _maxSimultaneousLights},
    {"maxSimultaneousMorphTargets", defines.NUM_MORPH_INFLUENCERS}};

  auto join = defines.toString();

  EffectCreationOptions options;
  options.attributes            = ::std::move(attribs);
  options.uniformsNames         = ::std::move(uniforms);
  options.uniformBuffersNames   = ::std::move(uniformBuffers);
  options.samplers              = ::std::move(samplers);
  options.materialDefines       = &defines;
  options.defines               = ::std::move(join);
  options.fallbacks             = ::std::move(fallbacks);
  options.onCompiled            = onCompiled;
  options.onError               = onError;
  options.indexParameters       = ::std::move(indexParameters);
  options.maxSimultaneousLights = _maxSimultaneousLights;

  MaterialHelper::PrepareUniformsAndSamplersList(options);

  return engine->createEffect("pbr", options, engine);
}

void PBRBaseMaterial::_prepareDefines(AbstractMesh* mesh,
                                      PBRMaterialDefines& defines,
                                      const Nullable<bool>& useInstances,
                                      const Nullable<bool>& useClipPlane)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  // Lights
  MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, true, _maxSimultaneousLights, _disableLighting,
    PMD::SPECULARTERM, PMD::SHADOWFLOAT);
  defines._needNormals = true;

  // Textures
  defines.defines[PMD::METALLICWORKFLOW] = isMetallicWorkflow();
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (scene->getEngine()->getCaps().textureLOD) {
        defines.defines[PMD::LODBASEDMICROSFURACE] = true;
      }

      if (_albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_albedoTexture, defines,
                                                  PMD::ALBEDO, "ALBEDO",
                                                  PMD::MAINUV1, PMD::MAINUV2);
      }
      else {
        defines.defines[PMD::ALBEDO] = false;
      }

      if (_ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_ambientTexture, defines,
                                                  PMD::AMBIENT, "AMBIENT",
                                                  PMD::MAINUV1, PMD::MAINUV2);
        defines.defines[PMD::AMBIENTINGRAYSCALE] = _useAmbientInGrayScale;
      }
      else {
        defines.defines[PMD::AMBIENT] = false;
      }

      if (_opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_opacityTexture, defines,
                                                  PMD::OPACITY, "OPACITY",
                                                  PMD::MAINUV1, PMD::MAINUV2);
        defines.defines[PMD::OPACITYRGB] = _opacityTexture->getAlphaFromRGB;
      }
      else {
        defines.defines[PMD::OPACITY] = false;
      }

      auto reflectionTexture = _getReflectionTexture();
      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        defines.defines[PMD::REFLECTION]      = true;
        defines.defines[PMD::GAMMAREFLECTION] = reflectionTexture->gammaSpace;
        defines.defines[PMD::RGBDREFLECTION]  = reflectionTexture->isRGBD;
        defines.defines[PMD::REFLECTIONMAP_OPPOSITEZ]
          = getScene()->useRightHandedSystem() ? !reflectionTexture->invertZ :
                                                 reflectionTexture->invertZ;
        defines.defines[PMD::LODINREFLECTIONALPHA]
          = reflectionTexture->lodLevelInAlpha;

        if (reflectionTexture->coordinatesMode()
            == TextureConstants::INVCUBIC_MODE) {
          defines.defines[PMD::INVERTCUBICMAP] = true;
        }

        defines.defines[PMD::REFLECTIONMAP_3D] = reflectionTexture->isCube;

        switch (reflectionTexture->coordinatesMode()) {
          case TextureConstants::EXPLICIT_MODE:
            defines.defines[PMD::REFLECTIONMAP_EXPLICIT] = true;
            break;
          case TextureConstants::PLANAR_MODE:
            defines.defines[PMD::REFLECTIONMAP_PLANAR] = true;
            break;
          case TextureConstants::PROJECTION_MODE:
            defines.defines[PMD::REFLECTIONMAP_PROJECTION] = true;
            break;
          case TextureConstants::SKYBOX_MODE:
            defines.defines[PMD::REFLECTIONMAP_SKYBOX] = true;
            break;
          case TextureConstants::SPHERICAL_MODE:
            defines.defines[PMD::REFLECTIONMAP_SPHERICAL] = true;
            break;
          case TextureConstants::EQUIRECTANGULAR_MODE:
            defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR] = true;
            break;
          case TextureConstants::FIXED_EQUIRECTANGULAR_MODE:
            defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR_FIXED] = true;
            break;
          case TextureConstants::FIXED_EQUIRECTANGULAR_MIRRORED_MODE:
            defines.defines[PMD::REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED]
              = true;
            break;
          case TextureConstants::CUBIC_MODE:
          case TextureConstants::INVCUBIC_MODE:
            defines.defines[PMD::REFLECTIONMAP_CUBIC] = true;
            defines.defines[PMD::USE_LOCAL_REFLECTIONMAP_CUBIC]
              = reflectionTexture->boundingBoxSize() ? true : false;
            break;
        }

        if (reflectionTexture->coordinatesMode()
            != TextureConstants::SKYBOX_MODE) {
          if (reflectionTexture->sphericalPolynomial()) {
            defines.defines[PMD::USESPHERICALFROMREFLECTIONMAP] = true;
            if (_forceIrradianceInFragment
                || scene->getEngine()->getCaps().maxVaryingVectors <= 8) {
              defines.defines[PMD::USESPHERICALINVERTEX] = false;
            }
            else {
              defines.defines[PMD::USESPHERICALINVERTEX] = true;
            }
          }
        }
      }
      else {
        defines.defines[PMD::REFLECTION]                          = false;
        defines.defines[PMD::REFLECTIONMAP_3D]                    = false;
        defines.defines[PMD::REFLECTIONMAP_SPHERICAL]             = false;
        defines.defines[PMD::REFLECTIONMAP_PLANAR]                = false;
        defines.defines[PMD::REFLECTIONMAP_CUBIC]                 = false;
        defines.defines[PMD::USE_LOCAL_REFLECTIONMAP_CUBIC]       = false;
        defines.defines[PMD::REFLECTIONMAP_PROJECTION]            = false;
        defines.defines[PMD::REFLECTIONMAP_SKYBOX]                = false;
        defines.defines[PMD::REFLECTIONMAP_EXPLICIT]              = false;
        defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR]       = false;
        defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR_FIXED] = false;
        defines.defines[PMD::REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED]
          = false;
        defines.defines[PMD::INVERTCUBICMAP]                = false;
        defines.defines[PMD::USESPHERICALFROMREFLECTIONMAP] = false;
        defines.defines[PMD::USESPHERICALINVERTEX]          = false;
        defines.defines[PMD::REFLECTIONMAP_OPPOSITEZ]       = false;
        defines.defines[PMD::LODINREFLECTIONALPHA]          = false;
        defines.defines[PMD::GAMMAREFLECTION]               = false;
        defines.defines[PMD::RGBDREFLECTION]                = false;
      }

      if (_lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_lightmapTexture, defines,
                                                  PMD::LIGHTMAP, "LIGHTMAP",
                                                  PMD::MAINUV1, PMD::MAINUV2);
        defines.defines[PMD::USELIGHTMAPASSHADOWMAP] = _useLightmapAsShadowmap;
        defines.defines[PMD::GAMMALIGHTMAP] = _lightmapTexture->gammaSpace;
      }
      else {
        defines.defines[PMD::LIGHTMAP] = false;
      }

      if (_emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
        MaterialHelper::PrepareDefinesForMergedUV(_emissiveTexture, defines,
                                                  PMD::EMISSIVE, "EMISSIVE",
                                                  PMD::MAINUV1, PMD::MAINUV2);
      }
      else {
        defines.defines[PMD::EMISSIVE] = false;
      }

      if (StandardMaterial::SpecularTextureEnabled()) {
        if (_metallicTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(
            _metallicTexture, defines, PMD::REFLECTIVITY, "REFLECTIVITY",
            PMD::MAINUV1, PMD::MAINUV2);
          defines.defines[PMD::ROUGHNESSSTOREINMETALMAPALPHA]
            = _useRoughnessFromMetallicTextureAlpha;
          defines.defines[PMD::ROUGHNESSSTOREINMETALMAPGREEN]
            = !_useRoughnessFromMetallicTextureAlpha
              && _useRoughnessFromMetallicTextureGreen;
          defines.defines[PMD::METALLNESSSTOREINMETALMAPBLUE]
            = _useMetallnessFromMetallicTextureBlue;
          defines.defines[PMD::AOSTOREINMETALMAPRED]
            = _useAmbientOcclusionFromMetallicTextureRed;
        }
        else if (_reflectivityTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(
            _reflectivityTexture, defines, PMD::REFLECTIVITY, "REFLECTIVITY",
            PMD::MAINUV1, PMD::MAINUV2);
          defines.defines[PMD::MICROSURFACEFROMREFLECTIVITYMAP]
            = _useMicroSurfaceFromReflectivityMapAlpha;
          defines.defines[PMD::MICROSURFACEAUTOMATIC]
            = _useAutoMicroSurfaceFromReflectivityMap;
        }
        else {
          defines.defines[PMD::REFLECTIVITY] = false;
        }

        if (_microSurfaceTexture) {
          MaterialHelper::PrepareDefinesForMergedUV(
            _microSurfaceTexture, defines, PMD::MICROSURFACEMAP,
            "MICROSURFACEMAP", PMD::MAINUV1, PMD::MAINUV2);
        }
        else {
          defines.defines[PMD::MICROSURFACEMAP] = false;
        }
      }
      else {
        defines.defines[PMD::REFLECTIVITY]    = false;
        defines.defines[PMD::MICROSURFACEMAP] = false;
      }

      if (scene->getEngine()->getCaps().standardDerivatives && _bumpTexture
          && StandardMaterial::BumpTextureEnabled() && !_disableBumpMap) {
        MaterialHelper::PrepareDefinesForMergedUV(
          _bumpTexture, defines, PMD::BUMP, "BUMP", PMD::MAINUV1, PMD::MAINUV2);

        if (_useParallax && _albedoTexture
            && StandardMaterial::DiffuseTextureEnabled()) {
          defines.defines[PMD::PARALLAX]          = true;
          defines.defines[PMD::PARALLAXOCCLUSION] = !!_useParallaxOcclusion;
        }
        else {
          defines.defines[PMD::PARALLAX] = false;
        }
        defines.defines[PMD::OBJECTSPACE_NORMALMAP] = _useObjectSpaceNormalMap;
      }
      else {
        defines.defines[PMD::BUMP] = false;
      }

      auto refractionTexture = _getRefractionTexture();
      if (refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
        defines.defines[PMD::REFRACTION]       = true;
        defines.defines[PMD::REFRACTIONMAP_3D] = refractionTexture->isCube;
        defines.defines[PMD::GAMMAREFRACTION]  = refractionTexture->gammaSpace;
        defines.defines[PMD::RGBDREFRACTION]   = refractionTexture->isRGBD;
        defines.defines[PMD::REFRACTIONMAP_OPPOSITEZ]
          = refractionTexture->invertZ;
        defines.defines[PMD::LODINREFRACTIONALPHA]
          = refractionTexture->lodLevelInAlpha;

        if (_linkRefractionWithTransparency) {
          defines.defines[PMD::LINKREFRACTIONTOTRANSPARENCY] = true;
        }
      }
      else {
        defines.defines[PMD::REFRACTION] = false;
      }

      if (_environmentBRDFTexture
          && StandardMaterial::ReflectionTextureEnabled()) {
        defines.defines[PMD::ENVIRONMENTBRDF] = true;
      }
      else {
        defines.defines[PMD::ENVIRONMENTBRDF] = false;
      }

      if (_shouldUseAlphaFromAlbedoTexture()) {
        defines.defines[PMD::ALPHAFROMALBEDO] = true;
      }
      else {
        defines.defines[PMD::ALPHAFROMALBEDO] = false;
      }
    }

    defines.defines[PMD::SPECULAROVERALPHA] = _useSpecularOverAlpha;

    if (_lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_STANDARD) {
      defines.defines[PMD::USEPHYSICALLIGHTFALLOFF] = false;
      defines.defines[PMD::USEGLTFLIGHTFALLOFF]     = false;
    }
    else if (_lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_GLTF) {
      defines.defines[PMD::USEPHYSICALLIGHTFALLOFF] = false;
      defines.defines[PMD::USEGLTFLIGHTFALLOFF]     = true;
    }
    else {
      defines.defines[PMD::USEPHYSICALLIGHTFALLOFF] = true;
      defines.defines[PMD::USEGLTFLIGHTFALLOFF]     = false;
    }

    defines.defines[PMD::RADIANCEOVERALPHA] = _useRadianceOverAlpha;

    if (!backFaceCulling() && _twoSidedLighting) {
      defines.defines[PMD::TWOSIDEDLIGHTING] = true;
    }
    else {
      defines.defines[PMD::TWOSIDEDLIGHTING] = false;
    }

    defines.ALPHATESTVALUE
      = "${_alphaCutOff}${_alphaCutOff % 1 === 0 ? \".\" : \"\"}";
    defines.defines[PMD::PREMULTIPLYALPHA]
      = (alphaMode() == EngineConstants::ALPHA_PREMULTIPLIED
         || alphaMode() == EngineConstants::ALPHA_PREMULTIPLIED_PORTERDUFF);
    defines.defines[PMD::ALPHABLEND] = needAlphaBlendingForMesh(mesh);
    defines.defines[PMD::ALPHAFRESNEL]
      = _useAlphaFresnel || _useLinearAlphaFresnel;
    defines.defines[PMD::LINEARALPHAFRESNEL] = _useLinearAlphaFresnel;

    defines.defines[PMD::SPECULARAA]
      = scene->getEngine()->getCaps().standardDerivatives
        && _enableSpecularAntiAliasing;
  }

  if (defines._areImageProcessingDirty && _imageProcessingConfiguration) {
    _imageProcessingConfiguration->prepareDefines(defines);
  }

  defines.defines[PMD::FORCENORMALFORWARD] = _forceNormalForward;

  defines.defines[PMD::RADIANCEOCCLUSION] = _useRadianceOcclusion;

  defines.defines[PMD::HORIZONOCCLUSION] = _useHorizonOcclusion;

  // Misc.
  if (defines._areMiscDirty) {
    MaterialHelper::PrepareDefinesForMisc(
      mesh, scene, _useLogarithmicDepth, pointsCloud(), fogEnabled(),
      _shouldTurnAlphaTestOn(mesh) || _forceAlphaTest, defines,
      PMD::LOGARITHMICDEPTH, PMD::POINTSIZE, PMD::FOG, PMD::NONUNIFORMSCALING,
      PMD::ALPHATEST);
    defines.defines[PMD::UNLIT]
      = _unlit
        || ((pointsCloud() || wireframe())
            && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind));
  }

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances ? true : false, PMD::CLIPPLANE,
    PMD::ALPHATEST, PMD::INSTANCES, useClipPlane);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, true, true, true,
    _transparencyMode != PBRMaterial::PBRMATERIAL_OPAQUE, PMD::NORMAL, PMD::UV1,
    PMD::UV2, PMD::VERTEXCOLOR, PMD::VERTEXALPHA, PMD::MORPHTARGETS_TANGENT,
    PMD::MORPHTARGETS_NORMAL, PMD::MORPHTARGETS);
}

void PBRBaseMaterial::forceCompilation(
  AbstractMesh* mesh, ::std::function<void(Material* material)>& onCompiled,
  bool clipPlane)
{
  PBRMaterialDefines defines;
  auto effect
    = _prepareEffect(mesh, defines, nullptr, nullptr, nullptr, clipPlane);
  if (effect->isReady()) {
    if (onCompiled) {
      onCompiled(this);
    }
  }
  else {
    effect->onCompileObservable.add(
      [&](Effect* /*effect*/, EventState& /*es*/) {
        if (onCompiled) {
          onCompiled(this);
        }
      });
  }
}

void PBRBaseMaterial::buildUniformLayout()
{
  // Order is important !
  _uniformBuffer->addUniform("vAlbedoInfos", 2);
  _uniformBuffer->addUniform("vAmbientInfos", 3);
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

void PBRBaseMaterial::bindForSubMesh(Matrix* world, Mesh* mesh,
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
  bindOnlyWorldMatrix(*world);

  // Normal Matrix
  if (defines.defines[PMD::OBJECTSPACE_NORMALMAP]) {
    world->toNormalMatrix(_normalMatrix);
    bindOnlyNormalMatrix(_normalMatrix);
  }

  const auto mustRebind = _mustRebind(scene, effect, mesh->visibility());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, effect);

  if (mustRebind) {
    _uniformBuffer->bindToEffect(effect, "Material");

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
          _uniformBuffer->updateFloat3(
            "vAmbientInfos",
            static_cast<float>(_ambientTexture->coordinatesIndex),
            static_cast<float>(_ambientTexture->level), _ambientTextureStrength,
            "");
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
                = static_cast<CubeTexture*>(reflectionTexture)) {
              _uniformBuffer->updateVector3("vReflectionPosition",
                                            cubeTexture->boundingBoxPosition);
              _uniformBuffer->updateVector3("vReflectionSize",
                                            *cubeTexture->boundingBoxSize());
            }
          }

          auto _polynomials = reflectionTexture->sphericalPolynomial();
          if (defines.USESPHERICALFROMREFLECTIONMAP && _polynomials) {
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
              = static_cast<RefractionTexture*>(_refractionTexture);
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
      if (defines.defines[PMD::METALLICWORKFLOW]) {
        PBRMaterial::_scaledReflectivity.r
          = (_metallic == 0.f || _metallic == 0.f) ? 1 : _metallic;
        PBRMaterial::_scaledReflectivity.g
          = (_roughness == 0.f || _roughness == 0.f) ? 1 : _roughness;
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
        if (defines.defines[PMD::LODBASEDMICROSFURACE]) {
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

      if (defines.defines[PMD::ENVIRONMENTBRDF]) {
        _uniformBuffer->setTexture("environmentBrdfSampler",
                                   _environmentBRDFTexture);
      }

      if (_refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
        if (defines.LODBASEDMICROSFURACE) {
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
         && scene->fogMode() != Scene::FOGMODE_NONE())
        || _reflectionTexture) {
      bindView(effect);
    }

    // Fog
    MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

    // Morph targets
    if (defines.NUM_MORPH_INFLUENCERS) {
      MaterialHelper::BindMorphTargetParameters(mesh, _activeEffect);
    }

    // image processing
    _imageProcessingConfiguration->bind(_activeEffect);

    // Log. depth
    MaterialHelper::BindLogDepth(defines, _activeEffect, scene,
                                 PMD::LOGARITHMICDEPTH);
  }

  _uniformBuffer->update();

  _afterBind(mesh, _activeEffect);
}

vector_t<IAnimatable*> PBRBaseMaterial::getAnimatables() const
{
  vector_t<IAnimatable*> results;

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

BaseTexture* PBRBaseMaterial::_getReflectionTexture() const
{
  if (_reflectionTexture) {
    return _reflectionTexture;
  }

  return getScene()->environmentTexture();
}

BaseTexture* PBRBaseMaterial::_getRefractionTexture() const
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
        && getScene()->_environmentBRDFTexture != _environmentBRDFTexture) {
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
