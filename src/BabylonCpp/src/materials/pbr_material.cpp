#include <babylon/materials/pbr_material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/light.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/color_grading_texture.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/materials/textures/refraction_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/morph/morph_target_manager.h>

namespace BABYLON {

Color3 PBRMaterial::_scaledAlbedo       = Color3();
Color3 PBRMaterial::_scaledReflectivity = Color3();
Color3 PBRMaterial::_scaledEmissive     = Color3();
Color3 PBRMaterial::_scaledReflection   = Color3();

PBRMaterial::PBRMaterial(const std::string& _name, Scene* scene)
    : Material{_name, scene}
    , directIntensity{1.f}
    , emissiveIntensity{1.f}
    , environmentIntensity{1.f}
    , specularIntensity{1.f}
    , disableBumpMap{false}
    , cameraExposure{1.f}
    , cameraContrast{1.f}
    , cameraColorGradingTexture{nullptr}
    , cameraColorCurves{nullptr}
    , albedoTexture{nullptr}
    , ambientTexture{nullptr}
    , ambientTextureStrength{1.f}
    , opacityTexture{nullptr}
    , reflectionTexture{nullptr}
    , emissiveTexture{nullptr}
    , reflectivityTexture{nullptr}
    , metallicTexture{nullptr}
    , metallic{1.f}
    , roughness{1.f}
    , microSurfaceTexture{nullptr}
    , bumpTexture{nullptr}
    , lightmapTexture{nullptr}
    , refractionTexture{nullptr}
    , ambientColor{Color3(0.f, 0.f, 0.f)}
    , albedoColor{Color3(1.f, 1.f, 1.f)}
    , reflectivityColor{Color3(1.f, 1.f, 1.f)}
    , reflectionColor{Color3(0.f, 0.f, 0.f)}
    , emissiveColor{Color3(0.f, 0.f, 0.f)}
    , microSurface{0.9f}
    , indexOfRefraction{0.66f}
    , invertRefractionY{false}
    , opacityFresnelParameters{nullptr}
    , emissiveFresnelParameters{nullptr}
    , linkRefractionWithTransparency{false}
    , linkEmissiveWithAlbedo{false}
    , useLightmapAsShadowmap{false}
    , useEmissiveAsIllumination{false}
    , useAlphaFromAlbedoTexture{false}
    , useSpecularOverAlpha{true}
    , useMicroSurfaceFromReflectivityMapAlpha{false}
    , useRoughnessFromMetallicTextureAlpha{true}
    , useRoughnessFromMetallicTextureGreen{true}
    , useMetallnessFromMetallicTextureBlue{false}
    , useAmbientOcclusionFromMetallicTextureRed{false}
    , useAmbientInGrayScale{false}
    , useAutoMicroSurfaceFromReflectivityMap{false}
    , useScalarInLinearSpace{false}
    , usePhysicalLightFalloff{true}
    , useRadianceOverAlpha{true}
    , useParallax{false}
    , useParallaxOcclusion{false}
    , parallaxScaleBias{0.05f}
    , disableLighting{false}
    , maxSimultaneousLights{4}
    , invertNormalMapX{false}
    , invertNormalMapY{false}
    , twoSidedLighting{false}
    , _lightingInfos{Vector4(directIntensity, emissiveIntensity,
                             environmentIntensity, specularIntensity)}
    , _cameraInfos{Vector4(1.f, 1.f, 0.f, 0.f)}
    , _microsurfaceTextureLods{Vector2(0.f, 0.f)}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _globalAmbientColor{Color3(0.f, 0.f, 0.f)}
    , _tempColor{Color3(0.f, 0.f, 0.f)}
    , _renderId{-1}
    , _useLogarithmicDepth{false}
    , _myScene{nullptr}
    , _myShadowGenerator{nullptr}
{
  _renderTargets.resize(16);

  _cachedDefines->BonesPerMesh = 0;

  getRenderTargetTextures = [&]() {
    _renderTargets.clear();

    if (StandardMaterial::ReflectionTextureEnabled() && reflectionTexture
        && reflectionTexture->isRenderTarget) {
      _renderTargets.emplace_back(reflectionTexture);
    }

    if (StandardMaterial::RefractionTextureEnabled() && refractionTexture
        && refractionTexture->isRenderTarget) {
      _renderTargets.emplace_back(refractionTexture);
    }

    return _renderTargets;
  };
}

PBRMaterial::~PBRMaterial()
{
}

const char* PBRMaterial::getClassName() const
{
  return "PBRMaterial";
}

IReflect::Type PBRMaterial::type() const
{
  return IReflect::Type::PBRMATERIAL;
}

bool PBRMaterial::useLogarithmicDepth() const
{
  return _useLogarithmicDepth;
}

void PBRMaterial::setUseLogarithmicDepth(bool value)
{
  _useLogarithmicDepth
    = value && getScene()->getEngine()->getCaps().fragmentDepthSupported;
}

bool PBRMaterial::needAlphaBlending()
{
  if (linkRefractionWithTransparency) {
    return false;
  }
  return (alpha < 1.f) || (opacityTexture != nullptr)
         || _shouldUseAlphaFromAlbedoTexture()
         || (opacityFresnelParameters && opacityFresnelParameters->isEnabled());
}

bool PBRMaterial::needAlphaTesting()
{
  if (linkRefractionWithTransparency) {
    return false;
  }
  return (albedoTexture != nullptr) && albedoTexture->hasAlpha();
}

bool PBRMaterial::_shouldUseAlphaFromAlbedoTexture() const
{
  return (albedoTexture != nullptr) && albedoTexture->hasAlpha()
         && useAlphaFromAlbedoTexture;
}

BaseTexture* PBRMaterial::getAlphaTestTexture()
{
  return albedoTexture;
}

bool PBRMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                              bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[PMD::INSTANCES] != useInstances) {
    return false;
  }

  return false;
}

void PBRMaterial::convertColorToLinearSpaceToRef(Color3& color, Color3& ref)
{
  ConvertColorToLinearSpaceToRef(color, ref, useScalarInLinearSpace);
}

void PBRMaterial::ConvertColorToLinearSpaceToRef(Color3& color, Color3& ref,
                                                 bool useScalarInLinear)
{
  if (!useScalarInLinear) {
    color.toLinearSpaceToRef(ref);
  }
  else {
    ref.r = color.r;
    ref.g = color.g;
    ref.b = color.b;
  }
}

void PBRMaterial::BindLights(Scene* scene, AbstractMesh* mesh, Effect* effect,
                             MaterialDefines& defines,
                             bool useScalarInLinearSpace,
                             unsigned int maxSimultaneousLights,
                             bool usePhysicalLightFalloff)
{
  unsigned int lightIndex = 0;
  for (auto light : mesh->_lightSources) {
    bool useUbo = light->_uniformBuffer->useUbo();

    const std::string lightIndexStr = std::to_string(lightIndex);

    light->_uniformBuffer->bindToEffect(effect, "Light" + lightIndexStr);
    MaterialHelper::BindLightProperties(light, effect, lightIndex);

    // GAMMA CORRECTION.
    ConvertColorToLinearSpaceToRef(light->diffuse, PBRMaterial::_scaledAlbedo,
                                   useScalarInLinearSpace);

    PBRMaterial::_scaledAlbedo.scaleToRef(light->intensity,
                                          PBRMaterial::_scaledAlbedo);
    light->_uniformBuffer->updateColor4(
      useUbo ? "vLightDiffuse" : "vLightDiffuse" + lightIndexStr,
      PBRMaterial::_scaledAlbedo,
      usePhysicalLightFalloff ? light->radius() : light->range, "");

    if (defines[PMD::SPECULARTERM]) {
      ConvertColorToLinearSpaceToRef(light->specular,
                                     PBRMaterial::_scaledReflectivity,
                                     useScalarInLinearSpace);

      PBRMaterial::_scaledReflectivity.scaleToRef(
        light->intensity, PBRMaterial::_scaledReflectivity);
      light->_uniformBuffer->updateColor3(
        useUbo ? "vLightSpecular" : "vLightSpecular" + lightIndexStr,
        PBRMaterial::_scaledReflectivity, "");
    }

    // Shadows
    if (scene->shadowsEnabled()) {
      MaterialHelper::BindLightShadow(light, scene, mesh, lightIndex, effect);
    }

    light->_uniformBuffer->update();

    ++lightIndex;

    if (lightIndex == maxSimultaneousLights) {
      break;
    }
  }
}

bool PBRMaterial::isReady(AbstractMesh* mesh, bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady) {
      return true;
    }
  }

  auto scene   = getScene();
  auto engine  = scene->getEngine();
  auto needUVs = false;

  _defines.reset();

  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, true, maxSimultaneousLights, false,
      PMD::SPECULARTERM, PMD::SHADOWFULLFLOAT);
  }

  if (!checkReadyOnEveryCall) {
    if (_renderId == scene->getRenderId()) {
      if (_checkCache(scene, mesh, useInstances)) {
        return true;
      }
    }
  }

  // Textures
  if (scene->texturesEnabled()) {
    if (scene->getEngine()->getCaps().textureLOD) {
      _defines.defines[PMD::LODBASEDMICROSFURACE] = true;
    }

    if (albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
      if (!albedoTexture->isReady()) {
        return false;
      }
      else {
        needUVs                       = true;
        _defines.defines[PMD::ALBEDO] = true;
      }
    }

    if (ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
      if (!ambientTexture->isReady()) {
        return false;
      }

      needUVs                                   = true;
      _defines.defines[PMD::AMBIENT]            = true;
      _defines.defines[PMD::AMBIENTINGRAYSCALE] = useAmbientInGrayScale;
    }

    if (opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
      if (!opacityTexture->isReady()) {
        return false;
      }

      needUVs                        = true;
      _defines.defines[PMD::OPACITY] = true;

      if (opacityTexture->getAlphaFromRGB) {
        _defines.defines[PMD::OPACITYRGB] = true;
      }
    }

    if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
      if (!reflectionTexture->isReady()) {
        return false;
      }

      _defines.defines[PMD::REFLECTION] = true;

      if (reflectionTexture->coordinatesMode
          == TextureConstants::INVCUBIC_MODE) {
        _defines.defines[PMD::INVERTCUBICMAP] = true;
      }

      _defines.defines[PMD::REFLECTIONMAP_3D] = reflectionTexture->isCube;

      switch (reflectionTexture->coordinatesMode) {
        case TextureConstants::CUBIC_MODE:
        case TextureConstants::INVCUBIC_MODE:
          _defines.defines[PMD::REFLECTIONMAP_CUBIC] = true;
          break;
        case TextureConstants::EXPLICIT_MODE:
          _defines.defines[PMD::REFLECTIONMAP_EXPLICIT] = true;
          break;
        case TextureConstants::PLANAR_MODE:
          _defines.defines[PMD::REFLECTIONMAP_PLANAR] = true;
          break;
        case TextureConstants::PROJECTION_MODE:
          _defines.defines[PMD::REFLECTIONMAP_PROJECTION] = true;
          break;
        case TextureConstants::SKYBOX_MODE:
          _defines.defines[PMD::REFLECTIONMAP_SKYBOX] = true;
          break;
        case TextureConstants::SPHERICAL_MODE:
          _defines.defines[PMD::REFLECTIONMAP_SPHERICAL] = true;
          break;
        case TextureConstants::EQUIRECTANGULAR_MODE:
          _defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR] = true;
          break;
        case TextureConstants::FIXED_EQUIRECTANGULAR_MODE:
          _defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR_FIXED] = true;
          break;
        case TextureConstants::FIXED_EQUIRECTANGULAR_MIRRORED_MODE:
          _defines.defines[PMD::REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED]
            = true;
          break;
        default:
          break;
      }

      auto hdrCubeTexture = dynamic_cast<HDRCubeTexture*>(reflectionTexture);
      if (hdrCubeTexture != nullptr) {
        _defines.defines[PMD::USESPHERICALFROMREFLECTIONMAP] = true;

        if (hdrCubeTexture->isPMREM) {
          _defines.defines[PMD::USEPMREMREFLECTION] = true;
        }
      }
    }

    if (lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
      if (!lightmapTexture->isReady()) {
        return false;
      }

      needUVs                                       = true;
      _defines.defines[PMD::LIGHTMAP]               = true;
      _defines.defines[PMD::USELIGHTMAPASSHADOWMAP] = useLightmapAsShadowmap;
    }

    if (emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
      if (!emissiveTexture->isReady()) {
        return false;
      }

      needUVs                         = true;
      _defines.defines[PMD::EMISSIVE] = true;
    }

    if (StandardMaterial::SpecularTextureEnabled()) {
      if (metallicTexture) {
        if (!metallicTexture->isReady()) {
          return false;
        }

        needUVs                                 = true;
        _defines.defines[PMD::METALLICWORKFLOW] = true;
        _defines.defines[PMD::METALLICMAP]      = true;
        _defines.defines[PMD::ROUGHNESSSTOREINMETALMAPALPHA]
          = useRoughnessFromMetallicTextureAlpha;
        _defines.defines[PMD::ROUGHNESSSTOREINMETALMAPGREEN]
          = !useRoughnessFromMetallicTextureAlpha
            && useRoughnessFromMetallicTextureGreen;
        _defines.defines[PMD::METALLNESSSTOREINMETALMAPBLUE]
          = useMetallnessFromMetallicTextureBlue;
        _defines.defines[PMD::AOSTOREINMETALMAPRED]
          = useAmbientOcclusionFromMetallicTextureRed;
      }
      else if (reflectivityTexture) {
        if (!reflectivityTexture->isReady()) {
          return false;
        }

        needUVs                             = true;
        _defines.defines[PMD::REFLECTIVITY] = true;
        _defines.defines[PMD::MICROSURFACEFROMREFLECTIVITYMAP]
          = useMicroSurfaceFromReflectivityMapAlpha;
        _defines.defines[PMD::MICROSURFACEAUTOMATIC]
          = useAutoMicroSurfaceFromReflectivityMap;
      }

      if (microSurfaceTexture) {
        if (!microSurfaceTexture->isReady()) {
          return false;
        }

        needUVs                                = true;
        _defines.defines[PMD::MICROSURFACEMAP] = true;
      }
    }

    if (scene->getEngine()->getCaps().standardDerivatives && bumpTexture
        && StandardMaterial::BumpTextureEnabled() && !disableBumpMap) {
      if (!bumpTexture->isReady()) {
        return false;
      }

      needUVs                     = true;
      _defines.defines[PMD::BUMP] = true;

      if (useParallax && albedoTexture
          && StandardMaterial::DiffuseTextureEnabled()) {
        _defines.defines[PMD::PARALLAX] = true;
        if (useParallaxOcclusion) {
          _defines.defines[PMD::PARALLAXOCCLUSION] = true;
        }
      }

      if (invertNormalMapX) {
        _defines.defines[PMD::INVERTNORMALMAPX] = true;
      }

      if (invertNormalMapY) {
        _defines.defines[PMD::INVERTNORMALMAPY] = true;
      }

      if (scene->_mirroredCameraPosition) {
        _defines.defines[PMD::INVERTNORMALMAPX]
          = !_defines.defines[PMD::INVERTNORMALMAPX];
        _defines.defines[PMD::INVERTNORMALMAPY]
          = !_defines.defines[PMD::INVERTNORMALMAPY];
      }

      if (scene->useRightHandedSystem()) {
        _defines.USERIGHTHANDEDSYSTEM = true;
      }
    }

    if (refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
      if (!refractionTexture->isReady()) {
        return false;
      }

      needUVs                                 = true;
      _defines.defines[PMD::REFRACTION]       = true;
      _defines.defines[PMD::REFRACTIONMAP_3D] = refractionTexture->isCube;

      if (linkRefractionWithTransparency) {
        _defines.defines[PMD::LINKREFRACTIONTOTRANSPARENCY] = true;
      }

      auto hdrCubeTexture = dynamic_cast<HDRCubeTexture*>(reflectionTexture);
      if (hdrCubeTexture != nullptr) {
        _defines.defines[PMD::REFRACTIONMAPINLINEARSPACE] = true;

        if (hdrCubeTexture->isPMREM) {
          _defines.defines[PMD::USEPMREMREFRACTION] = true;
        }
      }
    }

    if (cameraColorGradingTexture
        && StandardMaterial::ColorGradingTextureEnabled()) {
      if (!cameraColorGradingTexture->isReady()) {
        return false;
      }

      _defines.defines[PMD::CAMERACOLORGRADING] = true;
    }

    if (!backFaceCulling() && twoSidedLighting) {
      _defines.defines[PMD::TWOSIDEDLIGHTING] = true;
    }
  }

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[PMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[PMD::ALPHATEST] = true;
  }

  if (_shouldUseAlphaFromAlbedoTexture()) {
    _defines.defines[PMD::ALPHAFROMALBEDO] = true;
  }

  if (useEmissiveAsIllumination) {
    _defines.defines[PMD::EMISSIVEASILLUMINATION] = true;
  }

  if (linkEmissiveWithAlbedo) {
    _defines.defines[PMD::LINKEMISSIVEWITHALBEDO] = true;
  }

  if (useLogarithmicDepth()) {
    _defines.defines[PMD::LOGARITHMICDEPTH] = true;
  }

  if (!stl_util::almost_equal(cameraContrast, 1.f)) {
    _defines.defines[PMD::CAMERACONTRAST] = true;
  }

  if (!stl_util::almost_equal(cameraExposure, 1.f)) {
    _defines.defines[PMD::CAMERATONEMAP] = true;
  }

  if (cameraColorCurves) {
    _defines.defines[PMD::CAMERACOLORCURVES] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[PMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog()
      && scene->fogMode() != scene->FOGMODE_NONE && fogEnabled()) {
    _defines.defines[PMD::FOG] = true;
  }

  if (StandardMaterial::FresnelEnabled()) {
    // Fresnel
    if ((opacityFresnelParameters && opacityFresnelParameters->isEnabled())
        || (emissiveFresnelParameters
            && emissiveFresnelParameters->isEnabled())) {

      if (opacityFresnelParameters && opacityFresnelParameters->isEnabled()) {
        _defines.defines[PMD::OPACITYFRESNEL] = true;
      }

      if (emissiveFresnelParameters && emissiveFresnelParameters->isEnabled()) {
        _defines.defines[PMD::EMISSIVEFRESNEL] = true;
      }

      _defines.defines[PMD::FRESNEL] = true;
    }
  }

  if (_defines[PMD::SPECULARTERM] && useSpecularOverAlpha) {
    _defines.defines[PMD::SPECULAROVERALPHA] = true;
  }

  if (usePhysicalLightFalloff) {
    _defines.defines[PMD::USEPHYSICALLIGHTFALLOFF] = true;
  }

  if (useRadianceOverAlpha) {
    _defines.defines[PMD::RADIANCEOVERALPHA] = true;
  }

  if ((metallic != 0.f) || (roughness != 0.f)) {
    _defines.defines[PMD::METALLICWORKFLOW] = true;
  }

  // Attribs
  if (mesh) {
    if (!scene->getEngine()->getCaps().standardDerivatives
        && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      mesh->createNormals(true);
      BABYLON_LOGF_WARN(
        "PBRMaterial",
        "PBRMaterial: Normals have been created for the mesh: %s",
        mesh->name.c_str());
    }

    if (mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[PMD::NORMAL] = true;
      if (mesh->isVerticesDataPresent(VertexBuffer::TangentKind)) {
        _defines.defines[PMD::TANGENT] = true;
      }
    }
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[PMD::UV1] = true;
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        _defines.defines[PMD::UV2] = true;
      }
    }
    if (mesh->useVertexColors()
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[PMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha()) {
        _defines.defines[PMD::VERTEXALPHA] = true;
      }
    }
    if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers();
      _defines.BonesPerMesh
        = static_cast<unsigned>(mesh->skeleton()->bones.size() + 1);
    }

    // Instances
    if (useInstances) {
      _defines.defines[PMD::INSTANCES] = true;
    }

    if (mesh->type() == IReflect::Type::MESH) {
      auto _mesh = static_cast<Mesh*>(mesh);
      if (_mesh && _mesh->morphTargetManager()) {
        auto manager = _mesh->morphTargetManager();
        _defines.defines[PMD::MORPHTARGETS_TANGENT]
          = manager->supportsTangents() && _defines[PMD::TANGENT];
        _defines.defines[PMD::MORPHTARGETS_NORMAL]
          = manager->supportsNormals() && _defines[PMD::NORMAL];
        _defines.defines[PMD::MORPHTARGETS] = (manager->numInfluencers() > 0);
        _defines.NUM_MORPH_INFLUENCERS
          = static_cast<unsigned>(manager->numInfluencers());
      }
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (_defines[PMD::REFLECTION]) {
      fallbacks->addFallback(0, "REFLECTION");
    }

    if (_defines[PMD::REFRACTION]) {
      fallbacks->addFallback(0, "REFRACTION");
    }

    if (_defines[PMD::REFLECTIVITY]) {
      fallbacks->addFallback(0, "REFLECTIVITY");
    }

    if (_defines[PMD::BUMP]) {
      fallbacks->addFallback(0, "BUMP");
    }

    if (_defines[PMD::PARALLAX]) {
      fallbacks->addFallback(1, "PARALLAX");
    }

    if (_defines[PMD::PARALLAXOCCLUSION]) {
      fallbacks->addFallback(0, "PARALLAXOCCLUSION");
    }

    if (_defines[PMD::SPECULAROVERALPHA]) {
      fallbacks->addFallback(0, "SPECULAROVERALPHA");
    }

    if (_defines[PMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (_defines[PMD::POINTSIZE]) {
      fallbacks->addFallback(0, "POINTSIZE");
    }

    if (_defines[PMD::LOGARITHMICDEPTH]) {
      fallbacks->addFallback(0, "LOGARITHMICDEPTH");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, *fallbacks.get(),
                                              maxSimultaneousLights);

    if (_defines[PMD::SPECULARTERM]) {
      fallbacks->addFallback(0, "SPECULARTERM");
    }

    if (_defines[PMD::OPACITYFRESNEL]) {
      fallbacks->addFallback(1, "OPACITYFRESNEL");
    }

    if (_defines[PMD::EMISSIVEFRESNEL]) {
      fallbacks->addFallback(2, "EMISSIVEFRESNEL");
    }

    if (_defines[PMD::FRESNEL]) {
      fallbacks->addFallback(3, "FRESNEL");
    }

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{
      std::string(VertexBuffer::PositionKindChars)};

    if (_defines[PMD::NORMAL]) {
      attribs.emplace_back(std::string(VertexBuffer::NormalKindChars));
    }

    if (_defines[PMD::TANGENT]) {
      attribs.emplace_back(VertexBuffer::TangentKindChars);
    }

    if (_defines[PMD::UV1]) {
      attribs.emplace_back(std::string(VertexBuffer::UVKindChars));
    }

    if (_defines[PMD::UV2]) {
      attribs.emplace_back(std::string(VertexBuffer::UV2KindChars));
    }

    if (_defines[PMD::VERTEXCOLOR]) {
      attribs.emplace_back(std::string(VertexBuffer::ColorKindChars));
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, _defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  PMD::INSTANCES);
    MaterialHelper::PrepareAttributesForMorphTargets(attribs, mesh, _defines,
                                                     PMD::NORMAL);

    std::string join = _defines.toString();

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
                                      "depthValues",
                                      "opacityParts",
                                      "emissiveLeftColor",
                                      "emissiveRightColor",
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
                                      "vMicrosurfaceTextureLods",
                                      "vCameraInfos"};

    std::vector<std::string> samplers{
      "albedoSampler",         "ambientSampler",        "opacitySampler",
      "reflectionCubeSampler", "reflection2DSampler",   "emissiveSampler",
      "reflectivitySampler",   "microSurfaceSampler",   "bumpSampler",
      "lightmapSampler",       "refractionCubeSampler", "refraction2DSampler"};
    std::vector<std::string> uniformBuffers{"Material", "Scene"};

    if (_defines[PMD::CAMERACOLORCURVES]) {
      ColorCurves::PrepareUniforms(uniforms);
    }
    // if (_defines[PMD::CAMERACOLORGRADING]) {
    //  ColorGradingTexture::PrepareUniformsAndSamplers(uniforms, samplers);
    // }

    const auto _onCompiled = [this](Effect* effect) {
      if (onCompiled) {
        onCompiled(effect);
      }
      bindSceneUniformBuffer(effect, getScene()->getSceneUniformBuffer());
    };

    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", maxSimultaneousLights},
      {"maxSimultaneousMorphTargets", _defines.NUM_MORPH_INFLUENCERS},
    };

    EffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.uniformBuffersNames   = std::move(uniformBuffers);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &_defines;
    options.defines               = std::move(join);
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = std::move(_onCompiled);
    options.onError               = onError;
    options.indexParameters       = std::move(indexParameters);
    options.maxSimultaneousLights = maxSimultaneousLights;

    MaterialHelper::PrepareUniformsAndSamplersList(options);

    _effect = scene->getEngine()->createEffect("pbr", options, engine);

    buildUniformLayout();
  }
  if (!_effect->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void PBRMaterial::buildUniformLayout()
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
  _uniformBuffer->addUniform("refractionMatrix", 16);
  _uniformBuffer->addUniform("reflectionMatrix", 16);

  _uniformBuffer->addUniform("vReflectionColor", 3);
  _uniformBuffer->addUniform("vAlbedoColor", 4);
  _uniformBuffer->addUniform("vLightingIntensity", 4);

  _uniformBuffer->addUniform("vMicrosurfaceTextureLods", 2);
  _uniformBuffer->addUniform("vReflectivityColor", 4);
  _uniformBuffer->addUniform("vEmissiveColor", 3);
  _uniformBuffer->addUniform("opacityParts", 4);
  _uniformBuffer->addUniform("emissiveLeftColor", 4);
  _uniformBuffer->addUniform("emissiveRightColor", 4);

  _uniformBuffer->addUniform("pointSize", 1);
  _uniformBuffer->create();
}

void PBRMaterial::unbind()
{
  if (reflectionTexture && reflectionTexture->isRenderTarget) {
    _effect->setTexture("reflection2DSampler", nullptr);
  }

  if (refractionTexture && refractionTexture->isRenderTarget) {
    _effect->setTexture("refraction2DSampler", nullptr);
  }

  Material::unbind();
}

void PBRMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void PBRMaterial::bind(Matrix* world, Mesh* mesh)
{
  _myScene    = getScene();
  auto effect = _effect;

  // Matrices
  bindOnlyWorldMatrix(*world);

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (_myScene->getCachedMaterial() != this) {
    _uniformBuffer->bindToEffect(effect, "Material");

    bindViewProjection(effect);

    if (!_uniformBuffer->useUbo() || !isFrozen() || !_uniformBuffer->isSync()) {

      // Fresnel
      if (StandardMaterial::FresnelEnabled()) {
        if (opacityFresnelParameters && opacityFresnelParameters->isEnabled()) {
          _uniformBuffer->updateColor4(
            "opacityParts",
            Color3(opacityFresnelParameters->leftColor.toLuminance(),
                   opacityFresnelParameters->rightColor.toLuminance(),
                   opacityFresnelParameters->bias),
            opacityFresnelParameters->power, "");
        }

        if (emissiveFresnelParameters
            && emissiveFresnelParameters->isEnabled()) {
          _uniformBuffer->updateColor4("emissiveLeftColor",
                                       emissiveFresnelParameters->leftColor,
                                       emissiveFresnelParameters->power, "");
          _uniformBuffer->updateColor4("emissiveRightColor",
                                       emissiveFresnelParameters->rightColor,
                                       emissiveFresnelParameters->bias, "");
        }
      }

      // Texture uniforms
      if (_myScene->texturesEnabled()) {
        if (albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
          _uniformBuffer->updateFloat2("vAlbedoInfos",
                                       albedoTexture->coordinatesIndex,
                                       albedoTexture->level, "");
          _uniformBuffer->updateMatrix("albedoMatrix",
                                       *albedoTexture->getTextureMatrix());
        }

        if (ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
          _uniformBuffer->updateFloat3(
            "vAmbientInfos", ambientTexture->coordinatesIndex,
            ambientTexture->level, ambientTextureStrength, "");
          _uniformBuffer->updateMatrix("ambientMatrix",
                                       *ambientTexture->getTextureMatrix());
        }

        if (opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
          _uniformBuffer->updateFloat2("vOpacityInfos",
                                       opacityTexture->coordinatesIndex,
                                       opacityTexture->level, "");
          _uniformBuffer->updateMatrix("opacityMatrix",
                                       *opacityTexture->getTextureMatrix());
        }

        if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
          _microsurfaceTextureLods.x = std::round(
            static_cast<float>(std::log(reflectionTexture->getSize().width))
            * Math::LOG2E);
          _uniformBuffer->updateMatrix(
            "reflectionMatrix",
            *reflectionTexture->getReflectionTextureMatrix());
          _uniformBuffer->updateFloat2("vReflectionInfos",
                                       reflectionTexture->level, 0.f, "");

          if (_defines[PMD::USESPHERICALFROMREFLECTIONMAP]) {
            auto hdrCubeTexture
              = dynamic_cast<HDRCubeTexture*>(reflectionTexture);
            _effect->setFloat3("vSphericalX",
                               hdrCubeTexture->sphericalPolynomial->x.x,
                               hdrCubeTexture->sphericalPolynomial->x.y,
                               hdrCubeTexture->sphericalPolynomial->x.z);
            _effect->setFloat3("vSphericalY",
                               hdrCubeTexture->sphericalPolynomial->y.x,
                               hdrCubeTexture->sphericalPolynomial->y.y,
                               hdrCubeTexture->sphericalPolynomial->y.z);
            _effect->setFloat3("vSphericalZ",
                               hdrCubeTexture->sphericalPolynomial->z.x,
                               hdrCubeTexture->sphericalPolynomial->z.y,
                               hdrCubeTexture->sphericalPolynomial->z.z);
            _effect->setFloat3("vSphericalXX",
                               hdrCubeTexture->sphericalPolynomial->xx.x,
                               hdrCubeTexture->sphericalPolynomial->xx.y,
                               hdrCubeTexture->sphericalPolynomial->xx.z);
            _effect->setFloat3("vSphericalYY",
                               hdrCubeTexture->sphericalPolynomial->yy.x,
                               hdrCubeTexture->sphericalPolynomial->yy.y,
                               hdrCubeTexture->sphericalPolynomial->yy.z);
            _effect->setFloat3("vSphericalZZ",
                               hdrCubeTexture->sphericalPolynomial->zz.x,
                               hdrCubeTexture->sphericalPolynomial->zz.y,
                               hdrCubeTexture->sphericalPolynomial->zz.z);
            _effect->setFloat3("vSphericalXY",
                               hdrCubeTexture->sphericalPolynomial->xy.x,
                               hdrCubeTexture->sphericalPolynomial->xy.y,
                               hdrCubeTexture->sphericalPolynomial->xy.z);
            _effect->setFloat3("vSphericalYZ",
                               hdrCubeTexture->sphericalPolynomial->yz.x,
                               hdrCubeTexture->sphericalPolynomial->yz.y,
                               hdrCubeTexture->sphericalPolynomial->yz.z);
            _effect->setFloat3("vSphericalZX",
                               hdrCubeTexture->sphericalPolynomial->zx.x,
                               hdrCubeTexture->sphericalPolynomial->zx.y,
                               hdrCubeTexture->sphericalPolynomial->zx.z);
          }
        }

        if (emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
          _uniformBuffer->updateFloat2("vEmissiveInfos",
                                       emissiveTexture->coordinatesIndex,
                                       emissiveTexture->level, "");
          _uniformBuffer->updateMatrix("emissiveMatrix",
                                       *emissiveTexture->getTextureMatrix());
        }

        if (lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
          _uniformBuffer->updateFloat2("vLightmapInfos",
                                       lightmapTexture->coordinatesIndex,
                                       lightmapTexture->level, "");
          _uniformBuffer->updateMatrix("lightmapMatrix",
                                       *lightmapTexture->getTextureMatrix());
        }

        if (StandardMaterial::SpecularTextureEnabled()) {
          if (metallicTexture) {
            _uniformBuffer->updateFloat3(
              "vReflectivityInfos", metallicTexture->coordinatesIndex,
              metallicTexture->level, ambientTextureStrength, "");
            _uniformBuffer->updateMatrix("reflectivityMatrix",
                                         *metallicTexture->getTextureMatrix());
          }
          else if (reflectivityTexture) {
            _uniformBuffer->updateFloat3("vReflectivityInfos",
                                         reflectivityTexture->coordinatesIndex,
                                         reflectivityTexture->level, 1.f, "");
            _uniformBuffer->updateMatrix(
              "reflectivityMatrix", *reflectivityTexture->getTextureMatrix());
          }

          if (microSurfaceTexture) {
            _uniformBuffer->updateFloat2("vMicroSurfaceSamplerInfos",
                                         microSurfaceTexture->coordinatesIndex,
                                         microSurfaceTexture->level, "");
            _uniformBuffer->updateMatrix(
              "microSurfaceSamplerMatrix",
              *microSurfaceTexture->getTextureMatrix());
          }
        }

        if (bumpTexture && _myScene->getEngine()->getCaps().standardDerivatives
            && StandardMaterial::BumpTextureEnabled() && !disableBumpMap) {
          _uniformBuffer->updateFloat3(
            "vBumpInfos", bumpTexture->coordinatesIndex,
            1.f / bumpTexture->level, parallaxScaleBias, "");
          _uniformBuffer->updateMatrix("bumpMatrix",
                                       *bumpTexture->getTextureMatrix());
        }

        if (refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
          _microsurfaceTextureLods.y = std::round(
            static_cast<float>(std::log(refractionTexture->getSize().width))
            * Math::LOG2E);

          float depth = 1.f;
          if (!refractionTexture->isCube) {
            _uniformBuffer->updateMatrix(
              "refractionMatrix",
              *refractionTexture->getReflectionTextureMatrix());

            auto _refractionTexture
              = static_cast<RefractionTexture*>(refractionTexture);
            if (_refractionTexture) {
              depth = _refractionTexture->depth;
            }
          }
          _uniformBuffer->updateFloat4(
            "vRefractionInfos", refractionTexture->level, indexOfRefraction,
            depth, invertRefractionY ? -1.f : 1.f, "");
        }

        if ((reflectionTexture || refractionTexture)) {
          _uniformBuffer->updateFloat2("vMicrosurfaceTextureLods",
                                       _microsurfaceTextureLods.x,
                                       _microsurfaceTextureLods.y, "");
        }
      }

      // Point size
      if (pointsCloud()) {
        _uniformBuffer->updateFloat("pointSize", pointSize);
      }

      // Colors
      if (_defines[PMD::METALLICWORKFLOW]) {
        PBRMaterial::_scaledReflectivity.r = metallic;
        PBRMaterial::_scaledReflectivity.g = roughness;
        _uniformBuffer->updateColor4("vReflectivityColor",
                                     PBRMaterial::_scaledReflectivity, 0, "");
      }
      else {
        // GAMMA CORRECTION.
        convertColorToLinearSpaceToRef(reflectivityColor,
                                       PBRMaterial::_scaledReflectivity);
        _uniformBuffer->updateColor4("vReflectivityColor",
                                     PBRMaterial::_scaledReflectivity,
                                     microSurface, "");
      }

      // GAMMA CORRECTION.
      convertColorToLinearSpaceToRef(emissiveColor,
                                     PBRMaterial::_scaledEmissive);
      _uniformBuffer->updateColor3("vEmissiveColor",
                                   PBRMaterial::_scaledEmissive, "");

      // GAMMA CORRECTION.
      convertColorToLinearSpaceToRef(reflectionColor,
                                     PBRMaterial::_scaledReflection);
      _uniformBuffer->updateColor3("vReflectionColor",
                                   PBRMaterial::_scaledReflection, "");

      // GAMMA CORRECTION.
      convertColorToLinearSpaceToRef(albedoColor, PBRMaterial::_scaledAlbedo);
      _uniformBuffer->updateColor4("vAlbedoColor", PBRMaterial::_scaledAlbedo,
                                   alpha * mesh->visibility, "");

      // Misc
      _lightingInfos.x = directIntensity;
      _lightingInfos.y = emissiveIntensity;
      _lightingInfos.z = environmentIntensity;
      _lightingInfos.w = specularIntensity;

      _uniformBuffer->updateVector4("vLightingIntensity", _lightingInfos);
    }

    // Textures
    if (_myScene->texturesEnabled()) {
      if (albedoTexture && StandardMaterial::DiffuseTextureEnabled()) {
        _uniformBuffer->setTexture("albedoSampler", albedoTexture);
      }

      if (ambientTexture && StandardMaterial::AmbientTextureEnabled()) {
        _uniformBuffer->setTexture("ambientSampler", ambientTexture);
      }

      if (opacityTexture && StandardMaterial::OpacityTextureEnabled()) {
        _uniformBuffer->setTexture("opacitySampler", opacityTexture);
      }

      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        if (reflectionTexture->isCube) {
          _uniformBuffer->setTexture("reflectionCubeSampler",
                                     reflectionTexture);
        }
        else {
          _uniformBuffer->setTexture("reflection2DSampler", reflectionTexture);
        }
      }

      if (emissiveTexture && StandardMaterial::EmissiveTextureEnabled()) {
        _uniformBuffer->setTexture("emissiveSampler", emissiveTexture);
      }

      if (lightmapTexture && StandardMaterial::LightmapTextureEnabled()) {
        _uniformBuffer->setTexture("lightmapSampler", lightmapTexture);
      }

      if (StandardMaterial::SpecularTextureEnabled()) {
        if (metallicTexture) {
          _uniformBuffer->setTexture("reflectivitySampler", metallicTexture);
        }
        else if (reflectivityTexture) {
          _uniformBuffer->setTexture("reflectivitySampler",
                                     reflectivityTexture);
        }

        if (microSurfaceTexture) {
          _uniformBuffer->setTexture("microSurfaceSampler",
                                     microSurfaceTexture);
        }
      }

      if (bumpTexture && _myScene->getEngine()->getCaps().standardDerivatives
          && StandardMaterial::BumpTextureEnabled() && !disableBumpMap) {
        _uniformBuffer->setTexture("bumpSampler", bumpTexture);
      }

      if (refractionTexture && StandardMaterial::RefractionTextureEnabled()) {
        if (refractionTexture->isCube) {
          _uniformBuffer->setTexture("refractionCubeSampler",
                                     refractionTexture);
        }
        else {
          _uniformBuffer->setTexture("refraction2DSampler", refractionTexture);
        }
      }

      // if (cameraColorGradingTexture
      //     && StandardMaterial::ColorGradingTextureEnabled()) {
      //   ColorGradingTexture::Bind(cameraColorGradingTexture, _effect);
      // }
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_effect, _myScene);

    // Colors
    _myScene->ambientColor.multiplyToRef(ambientColor, _globalAmbientColor);

    effect->setVector3("vEyePosition", _myScene->_mirroredCameraPosition ?
                                         *_myScene->_mirroredCameraPosition :
                                         _myScene->activeCamera->position);
    effect->setColor3("vAmbientColor", _globalAmbientColor);
  }

  if (_myScene->getCachedMaterial() != this || !isFrozen()) {

    // Lights
    if (_myScene->lightsEnabled() && !disableLighting) {
      PBRMaterial::BindLights(_myScene, mesh, _effect, _defines,
                              useScalarInLinearSpace, maxSimultaneousLights,
                              usePhysicalLightFalloff);
    }

    // View
    if ((_myScene->fogEnabled() && mesh->applyFog()
         && (_myScene->fogMode() != Scene::FOGMODE_NONE))
        || reflectionTexture) {
      bindView(effect);
    }

    // Fog
    MaterialHelper::BindFogParameters(_myScene, mesh, _effect);

    // Morph targets
    if (_defines.NUM_MORPH_INFLUENCERS) {
      MaterialHelper::BindMorphTargetParameters(mesh, _effect);
    }

    _cameraInfos.x = cameraExposure;
    _cameraInfos.y = cameraContrast;
    effect->setVector4("vCameraInfos", _cameraInfos);

    if (cameraColorCurves) {
      ColorCurves::Bind(*cameraColorCurves, _effect);
    }

    // Log. depth
    MaterialHelper::BindLogDepth(_defines, _effect, _myScene,
                                 PMD::LOGARITHMICDEPTH);
  }

  _uniformBuffer->update();

  _afterBind(mesh);
}

std::vector<IAnimatable*> PBRMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (albedoTexture && albedoTexture->animations.size() > 0) {
    results.emplace_back(albedoTexture);
  }

  if (ambientTexture && ambientTexture->animations.size() > 0) {
    results.emplace_back(ambientTexture);
  }

  if (opacityTexture && opacityTexture->animations.size() > 0) {
    results.emplace_back(opacityTexture);
  }

  if (reflectionTexture && reflectionTexture->animations.size() > 0) {
    results.emplace_back(reflectionTexture);
  }

  if (emissiveTexture && emissiveTexture->animations.size() > 0) {
    results.emplace_back(emissiveTexture);
  }

  if (metallicTexture && metallicTexture->animations.size() > 0) {
    results.emplace_back(metallicTexture);
  }
  else if (reflectivityTexture && reflectivityTexture->animations.size() > 0) {
    results.emplace_back(reflectivityTexture);
  }

  if (bumpTexture && bumpTexture->animations.size() > 0) {
    results.emplace_back(bumpTexture);
  }

  if (lightmapTexture && lightmapTexture->animations.size() > 0) {
    results.emplace_back(lightmapTexture);
  }

  if (refractionTexture && refractionTexture->animations.size() > 0) {
    results.emplace_back(refractionTexture);
  }

  if (cameraColorGradingTexture
      && cameraColorGradingTexture->animations.size() > 0) {
    results.emplace_back(cameraColorGradingTexture);
  }

  return results;
}

void PBRMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (forceDisposeTextures) {
    if (albedoTexture) {
      albedoTexture->dispose();
    }

    if (ambientTexture) {
      ambientTexture->dispose();
    }

    if (opacityTexture) {
      opacityTexture->dispose();
    }

    if (reflectionTexture) {
      reflectionTexture->dispose();
    }

    if (emissiveTexture) {
      emissiveTexture->dispose();
    }

    if (metallicTexture) {
      metallicTexture->dispose();
    }

    if (reflectivityTexture) {
      reflectivityTexture->dispose();
    }

    if (bumpTexture) {
      bumpTexture->dispose();
    }

    if (lightmapTexture) {
      lightmapTexture->dispose();
    }

    if (refractionTexture) {
      refractionTexture->dispose();
    }

    if (cameraColorGradingTexture) {
      cameraColorGradingTexture->dispose();
    }
  }

  _renderTargets.clear();

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* PBRMaterial::clone(const std::string& /*name*/,
                             bool /*cloneChildren*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
