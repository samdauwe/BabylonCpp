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
#include <babylon/materials/textures/refraction_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/base_sub_mesh.h>
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
    , _useAlphaFromAlbedoTexture{false}
    , _useSpecularOverAlpha{true}
    , _useMicroSurfaceFromReflectivityMapAlpha{false}
    , _useRoughnessFromMetallicTextureAlpha{true}
    , _useRoughnessFromMetallicTextureGreen{false}
    , _useMetallnessFromMetallicTextureBlue{false}
    , _useAmbientOcclusionFromMetallicTextureRed{false}
    , _useAmbientInGrayScale{false}
    , _useAutoMicroSurfaceFromReflectivityMap{false}
    , _usePhysicalLightFalloff{true}
    , _useRadianceOverAlpha{true}
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
    , _preMultiplyAlpha{false}
    , _useAlphaFresnel{false}
    , _environmentBRDFTexture{nullptr}
    , _forceIrradianceInFragment{false}
    , _forceNormalForward{false}
    , _lightingInfos{Vector4(_directIntensity, _emissiveIntensity,
                             _environmentIntensity, _specularIntensity)}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _globalAmbientColor{Color3(0, 0, 0)}
    , _renderId{-1}
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
    _imageProcessingConfiguration = getScene()->imageProcessingConfiguration();
  }
  else {
    _imageProcessingConfiguration = configuration;
  }

  // Attaches observer.
  _imageProcessingObserver
    = _imageProcessingConfiguration->onUpdateParameters.add(
      [this](ImageProcessingConfiguration*, EventState&) {
        _markAllSubMeshesAsImageProcessingDirty();
      });
}

const char* PBRBaseMaterial::getClassName() const
{
  return "PBRBaseMaterial";
}

bool PBRBaseMaterial::useLogarithmicDepth() const
{
  return _useLogarithmicDepth;
}

void PBRBaseMaterial::setUseLogarithmicDepth(bool value)
{
  _useLogarithmicDepth
    = value && getScene()->getEngine()->getCaps().fragmentDepthSupported;
}

bool PBRBaseMaterial::needAlphaBlending()
{
  if (_linkRefractionWithTransparency) {
    return false;
  }
  return (alpha < 1.f) || (_opacityTexture != nullptr)
         || _shouldUseAlphaFromAlbedoTexture();
}

bool PBRBaseMaterial::needAlphaTesting()
{
  if (_forceAlphaTest) {
    return true;
  }

  if (_linkRefractionWithTransparency) {
    return false;
  }
  return _albedoTexture != nullptr && _albedoTexture->hasAlpha();
}

bool PBRBaseMaterial::_shouldUseAlphaFromAlbedoTexture() const
{
  return _albedoTexture != nullptr && _albedoTexture->hasAlpha()
         && _useAlphaFromAlbedoTexture;
}

BaseTexture* PBRBaseMaterial::getAlphaTestTexture()
{
  return _albedoTexture;
}

bool PBRBaseMaterial::isReadyForSubMesh(AbstractMesh* mesh,
                                        BaseSubMesh* subMesh, bool useInstances)
{
  if (isFrozen()) {
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

  // Lights
  MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, true, _maxSimultaneousLights, _disableLighting,
    PMD::SPECULARTERM, PMD::SHADOWFLOAT);
  defines._needNormals = true;

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (scene->getEngine()->getCaps().textureLOD) {
        defines.defines[PMD::LODBASEDMICROSFURACE] = true;
      }

      if (_albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
        if (!_albedoTexture->isReadyOrNotBlocking()) {
          return false;
        }

        MaterialHelper::PrepareDefinesForMergedUV(_albedoTexture, defines,
                                                  PMD::ALBEDO, "ALBEDO",
                                                  PMD::MAINUV1, PMD::MAINUV2);
      }
      else {
        defines.defines[PMD::ALBEDO] = false;
      }

      if (_ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
        if (!_ambientTexture->isReadyOrNotBlocking()) {
          return false;
        }

        MaterialHelper::PrepareDefinesForMergedUV(_ambientTexture, defines,
                                                  PMD::AMBIENT, "AMBIENT",
                                                  PMD::MAINUV1, PMD::MAINUV2);
        defines.defines[PMD::AMBIENTINGRAYSCALE] = _useAmbientInGrayScale;
      }
      else {
        defines.defines[PMD::AMBIENT] = false;
      }

      if (_opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
        if (!_opacityTexture->isReadyOrNotBlocking()) {
          return false;
        }

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
        if (!reflectionTexture->isReadyOrNotBlocking()) {
          return false;
        }

        defines.defines[PMD::REFLECTION]      = true;
        defines.defines[PMD::GAMMAREFLECTION] = reflectionTexture->gammaSpace;
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
          case TextureConstants::CUBIC_MODE:
          case TextureConstants::INVCUBIC_MODE:
            defines.defines[PMD::REFLECTIONMAP_CUBIC] = true;
            break;
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
        }

        if (reflectionTexture->coordinatesMode()
            != TextureConstants::SKYBOX_MODE) {
          if (reflectionTexture->sphericalPolynomial()) {
            defines.defines[PMD::USESPHERICALFROMREFLECTIONMAP] = true;
            if (_forceIrradianceInFragment
                || scene->getEngine()->getCaps().maxVaryingVectors <= 8) {
              defines.defines[PMD::USESPHERICALINFRAGMENT] = true;
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
        defines.defines[PMD::REFLECTIONMAP_PROJECTION]            = false;
        defines.defines[PMD::REFLECTIONMAP_SKYBOX]                = false;
        defines.defines[PMD::REFLECTIONMAP_EXPLICIT]              = false;
        defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR]       = false;
        defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR_FIXED] = false;
        defines.defines[PMD::REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED]
          = false;
        defines.defines[PMD::INVERTCUBICMAP]                = false;
        defines.defines[PMD::USESPHERICALFROMREFLECTIONMAP] = false;
        defines.defines[PMD::USESPHERICALINFRAGMENT]        = false;
        defines.defines[PMD::REFLECTIONMAP_OPPOSITEZ]       = false;
        defines.defines[PMD::LODINREFLECTIONALPHA]          = false;
        defines.defines[PMD::GAMMAREFLECTION]               = false;
      }

      if (_lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
        if (!_lightmapTexture->isReadyOrNotBlocking()) {
          return false;
        }

        MaterialHelper::PrepareDefinesForMergedUV(_lightmapTexture, defines,
                                                  PMD::LIGHTMAP, "LIGHTMAP",
                                                  PMD::MAINUV1, PMD::MAINUV2);
        defines.defines[PMD::USELIGHTMAPASSHADOWMAP] = _useLightmapAsShadowmap;
      }
      else {
        defines.defines[PMD::LIGHTMAP] = false;
      }

      if (_emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
        if (!_emissiveTexture->isReadyOrNotBlocking()) {
          return false;
        }

        MaterialHelper::PrepareDefinesForMergedUV(_emissiveTexture, defines,
                                                  PMD::EMISSIVE, "EMISSIVE",
                                                  PMD::MAINUV1, PMD::MAINUV2);
      }
      else {
        defines.defines[PMD::EMISSIVE] = false;
      }

      if (StandardMaterial::SpecularTextureEnabled()) {
        if (_metallicTexture) {
          if (!_metallicTexture->isReadyOrNotBlocking()) {
            return false;
          }

          MaterialHelper::PrepareDefinesForMergedUV(
            _metallicTexture, defines, PMD::REFLECTIVITY, "REFLECTIVITY",
            PMD::MAINUV1, PMD::MAINUV2);
          defines.defines[PMD::METALLICWORKFLOW] = true;
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
          if (!_reflectivityTexture->isReadyOrNotBlocking()) {
            return false;
          }

          defines.defines[PMD::METALLICWORKFLOW] = false;
          MaterialHelper::PrepareDefinesForMergedUV(
            _reflectivityTexture, defines, PMD::REFLECTIVITY, "REFLECTIVITY",
            PMD::MAINUV1, PMD::MAINUV2);
          defines.defines[PMD::MICROSURFACEFROMREFLECTIVITYMAP]
            = _useMicroSurfaceFromReflectivityMapAlpha;
          defines.defines[PMD::MICROSURFACEAUTOMATIC]
            = _useAutoMicroSurfaceFromReflectivityMap;
        }
        else {
          defines.defines[PMD::METALLICWORKFLOW] = false;
          defines.defines[PMD::REFLECTIVITY]     = false;
        }

        if (_microSurfaceTexture) {
          if (!_microSurfaceTexture->isReadyOrNotBlocking()) {
            return false;
          }

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
        // Bump texure can not be none blocking.
        if (!_bumpTexture->isReady()) {
          return false;
        }

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
      }
      else {
        defines.defines[PMD::BUMP] = false;
      }

      auto refractionTexture = _getRefractionTexture();
      if (refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
        if (!refractionTexture->isReadyOrNotBlocking()) {
          return false;
        }

        defines.defines[PMD::REFRACTION]       = true;
        defines.defines[PMD::REFRACTIONMAP_3D] = refractionTexture->isCube;
        defines.defines[PMD::GAMMAREFRACTION]  = refractionTexture->gammaSpace;
        defines.defines[PMD::REFRACTIONMAP_OPPOSITEZ]
          = reflectionTexture->invertZ;
        defines.defines[PMD::LODINREFRACTIONALPHA]
          = reflectionTexture->lodLevelInAlpha;

        if (_linkRefractionWithTransparency) {
          defines.defines[PMD::LINKREFRACTIONTOTRANSPARENCY] = true;
        }
      }
      else {
        defines.defines[PMD::REFRACTION] = false;
      }

      if (_environmentBRDFTexture
          && StandardMaterial::ReflectionTextureEnabled()) {
        // This is blocking.
        if (!_environmentBRDFTexture->isReady()) {
          return false;
        }
        defines.defines[PMD::ENVIRONMENTBRDF] = true;
      }

      if (_shouldUseAlphaFromAlbedoTexture()) {
        defines.defines[PMD::ALPHAFROMALBEDO] = true;
      }
    }

    if (_useSpecularOverAlpha) {
      defines.defines[PMD::SPECULAROVERALPHA] = true;
    }

    if (_usePhysicalLightFalloff) {
      defines.defines[PMD::USEPHYSICALLIGHTFALLOFF] = true;
    }

    if (_useRadianceOverAlpha) {
      defines.defines[PMD::RADIANCEOVERALPHA] = true;
    }

    if ((_metallic != 0.f) || (_roughness != 0.f)) {
      defines.defines[PMD::METALLICWORKFLOW] = true;
    }

    if (!backFaceCulling() && _twoSidedLighting) {
      defines.defines[PMD::TWOSIDEDLIGHTING] = true;
    }

    defines.ALPHATESTVALUE                 = _alphaCutOff;
    defines.defines[PMD::PREMULTIPLYALPHA] = _preMultiplyAlpha;
    defines.defines[PMD::ALPHABLEND]       = needAlphaBlending();
    defines.defines[PMD::ALPHAFRESNEL]     = _useAlphaFresnel;
  }

  if (defines._areImageProcessingDirty) {
    if (!_imageProcessingConfiguration->isReady()) {
      return false;
    }

    _imageProcessingConfiguration->prepareDefines(defines);
  }

  defines.defines[PMD::FORCENORMALFORWARD] = _forceNormalForward;

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, _useLogarithmicDepth, pointsCloud(), fogEnabled(), defines,
    PMD::LOGARITHMICDEPTH, PMD::POINTSIZE, PMD::FOG);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, PMD::CLIPPLANE, PMD::ALPHATEST,
    PMD::INSTANCES, _forceAlphaTest);

  // Attribs
  if (MaterialHelper::PrepareDefinesForAttributes(
        mesh, defines, true, true, true, PMD::NORMAL, PMD::UV1, PMD::UV2,
        PMD::VERTEXCOLOR, PMD::VERTEXALPHA, PMD::MORPHTARGETS_NORMAL,
        PMD::MORPHTARGETS)) {
    if (mesh) {
      if (!scene->getEngine()->getCaps().standardDerivatives
          && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
        mesh->createNormals(true);
        BABYLON_LOGF_WARN(
          "PBRBaseMaterial",
          "PBRMaterial: Normals have been created for the mesh: %s",
          mesh->name.c_str())
      }
    }
  }

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = ::std::make_unique<EffectFallbacks>();
    if (defines[PMD::ENVIRONMENTBRDF]) {
      fallbacks->addFallback(0, "ENVIRONMENTBRDF");
    }

    if (defines[PMD::REFLECTION]) {
      fallbacks->addFallback(0, "REFLECTION");
    }

    if (defines[PMD::REFRACTION]) {
      fallbacks->addFallback(0, "REFRACTION");
    }

    if (defines[PMD::REFLECTIVITY]) {
      fallbacks->addFallback(0, "REFLECTIVITY");
    }

    if (defines[PMD::BUMP]) {
      fallbacks->addFallback(0, "BUMP");
    }

    if (defines[PMD::PARALLAX]) {
      fallbacks->addFallback(1, "PARALLAX");
    }

    if (defines[PMD::PARALLAXOCCLUSION]) {
      fallbacks->addFallback(0, "PARALLAXOCCLUSION");
    }

    if (defines[PMD::SPECULAROVERALPHA]) {
      fallbacks->addFallback(0, "SPECULAROVERALPHA");
    }

    if (defines[PMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (defines[PMD::POINTSIZE]) {
      fallbacks->addFallback(0, "POINTSIZE");
    }

    if (defines[PMD::LOGARITHMICDEPTH]) {
      fallbacks->addFallback(0, "LOGARITHMICDEPTH");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines[PMD::SPECULARTERM]) {
      fallbacks->addFallback(0, "SPECULARTERM");
    }

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
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

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
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

    ImageProcessingConfiguration::PrepareUniforms(uniforms, defines);
    ImageProcessingConfiguration::PrepareSamplers(samplers, defines);

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

    subMesh->setEffect(scene->getEngine()->createEffect("pbr", options, engine),
                       defines);

    buildUniformLayout();
  }

  if (!subMesh->effect()->isReady()) {
    return false;
  }

  defines._renderId   = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
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

void PBRBaseMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _activeEffect->setMatrix("world", world);
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

  auto effect   = subMesh->effect();
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(*world);

  const auto mustRebind = _mustRebind(scene, effect, mesh->visibility);

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

          if (defines.USESPHERICALFROMREFLECTIONMAP) {
            auto polynomials = *reflectionTexture->sphericalPolynomial();
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
                                   alpha * mesh->visibility, "");

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

    auto eyePosition = scene->_mirroredCameraPosition ?
                         *scene->_mirroredCameraPosition :
                         scene->activeCamera->globalPosition();
    auto invertNormal = (scene->useRightHandedSystem()
                         == (scene->_mirroredCameraPosition != nullptr));
    effect->setFloat4("vEyePosition", eyePosition.x, eyePosition.y,
                      eyePosition.z, invertNormal ? -1.f : 1.f);
    effect->setColor3("vAmbientColor", _globalAmbientColor);
  }

  if (mustRebind || !isFrozen()) {
    // Lights
    if (scene->lightsEnabled() && !_disableLighting) {
      MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                                 _maxSimultaneousLights,
                                 _usePhysicalLightFalloff);
    }

    // View
    if ((scene->fogEnabled() && mesh->applyFog()
         && scene->fogMode() != Scene::FOGMODE_NONE)
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

  _afterBind(mesh, effect);

  scene = nullptr;
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

    if (_environmentBRDFTexture) {
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
