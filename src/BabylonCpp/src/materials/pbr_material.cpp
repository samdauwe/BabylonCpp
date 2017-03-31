#include <babylon/materials/pbr_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/light.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/color_grading_texture.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/materials/textures/refraction_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/spherical_polynomial.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

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
    , overloadedShadowIntensity{1.f}
    , overloadedShadeIntensity{1.f}
    , cameraExposure{1.f}
    , cameraContrast{1.f}
    , cameraColorGradingTexture{nullptr}
    , cameraColorCurves{nullptr}
    , overloadedAmbient{Color3::White()}
    , overloadedAmbientIntensity{0.f}
    , overloadedAlbedo{Color3::White()}
    , overloadedAlbedoIntensity{0.f}
    , overloadedReflectivity{Color3(0.3f, 0.3f, 0.3f)}
    , overloadedReflectivityIntensity{0.f}
    , overloadedEmissive{Color3::White()}
    , overloadedEmissiveIntensity{0.f}
    , overloadedReflection{Color3::White()}
    , overloadedReflectionIntensity{0.f}
    , overloadedMicroSurface{0.f}
    , overloadedMicroSurfaceIntensity{0.f}
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
    , bumpTexture{nullptr}
    , lightmapTexture{nullptr}
    , refractionTexture{nullptr}
    , ambientColor{Color3(0.f, 0.f, 0.f)}
    , albedoColor{Color3(1.f, 1.f, 1.f)}
    , reflectivityColor{Color3(1.f, 1.f, 1.f)}
    , reflectionColor{Color3(0.5f, 0.5f, 0.5f)}
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
    , _overloadedShadowInfos{Vector4(overloadedShadowIntensity,
                                     overloadedShadeIntensity, 0.f, 0.f)}
    , _cameraInfos{Vector4(1.f, 1.f, 0.f, 0.f)}
    , _microsurfaceTextureLods{Vector2(0.f, 0.f)}
    , _overloadedIntensity{Vector4(
        overloadedAmbientIntensity, overloadedAlbedoIntensity,
        overloadedReflectivityIntensity, overloadedEmissiveIntensity)}
    , _overloadedMicroSurface{Vector3(overloadedMicroSurface,
                                      overloadedMicroSurfaceIntensity,
                                      overloadedReflectionIntensity)}
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

    if (reflectionTexture && reflectionTexture->isRenderTarget) {
      _renderTargets.emplace_back(reflectionTexture);
    }

    if (refractionTexture && refractionTexture->isRenderTarget) {
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
         || (opacityFresnelParameters && opacityFresnelParameters->isEnabled);
}

bool PBRMaterial::needAlphaTesting()
{
  if (linkRefractionWithTransparency) {
    return false;
  }
  return (albedoTexture != nullptr) && albedoTexture->hasAlpha;
}

bool PBRMaterial::_shouldUseAlphaFromAlbedoTexture() const
{
  return (albedoTexture != nullptr) && albedoTexture->hasAlpha
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

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
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
  unsigned int lightIndex    = 0;
  bool depthValuesAlreadySet = false;
  for (auto& light : scene->lights) {
    if (!light->isEnabled()) {
      continue;
    }

    if (!light->canAffectMesh(mesh)) {
      continue;
    }

    MaterialHelper::BindLightProperties(light.get(), effect, lightIndex);

    const std::string lightIndexStr = std::to_string(lightIndex);

    // GAMMA CORRECTION.
    ConvertColorToLinearSpaceToRef(light->diffuse, PBRMaterial::_scaledAlbedo,
                                   useScalarInLinearSpace);

    PBRMaterial::_scaledAlbedo.scaleToRef(light->intensity,
                                          PBRMaterial::_scaledAlbedo);
    effect->setColor4("vLightDiffuse" + lightIndexStr,
                      PBRMaterial::_scaledAlbedo,
                      usePhysicalLightFalloff ? light->radius : light->range);

    if (defines[PMD::SPECULARTERM]) {
      ConvertColorToLinearSpaceToRef(light->specular,
                                     PBRMaterial::_scaledReflectivity,
                                     useScalarInLinearSpace);

      PBRMaterial::_scaledReflectivity.scaleToRef(
        light->intensity, PBRMaterial::_scaledReflectivity);
      effect->setColor3("vLightSpecular" + lightIndexStr,
                        PBRMaterial::_scaledReflectivity);
    }

    // Shadows
    if (scene->shadowsEnabled) {
      depthValuesAlreadySet = MaterialHelper::BindLightShadow(
        light.get(), scene, mesh, lightIndex, effect, depthValuesAlreadySet);
    }

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

  Scene* scene     = getScene();
  Engine* engine   = scene->getEngine();
  bool needNormals = false;
  bool needUVs     = false;

  _defines.reset();

  if (scene->lightsEnabled && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
                    scene, mesh, _defines, maxSimultaneousLights,
                    PMD::SPECULARTERM, PMD::SHADOWS, PMD::SHADOWFULLFLOAT)
                  || needNormals;
  }

  if (!checkReadyOnEveryCall) {
    if (_renderId == scene->getRenderId()) {
      if (_checkCache(scene, mesh, useInstances)) {
        return true;
      }
    }
  }

  // Textures
  if (scene->texturesEnabled) {
    if (scene->getEngine()->getCaps().textureLOD) {
      _defines.defines[PMD::LODBASEDMICROSFURACE] = true;
    }

    if (albedoTexture && StandardMaterial::DiffuseTextureEnabled) {
      if (!albedoTexture->isReady()) {
        return false;
      }
      else {
        needUVs                       = true;
        _defines.defines[PMD::ALBEDO] = true;
      }
    }

    if (ambientTexture && StandardMaterial::AmbientTextureEnabled) {
      if (!ambientTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[PMD::AMBIENT] = true;
      }
    }

    if (opacityTexture && StandardMaterial::OpacityTextureEnabled) {
      if (!opacityTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[PMD::OPACITY] = true;

        if (opacityTexture->getAlphaFromRGB) {
          _defines.defines[PMD::OPACITYRGB] = true;
        }
      }
    }

    if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled) {
      if (!reflectionTexture->isReady()) {
        return false;
      }
      else {
        needNormals                       = true;
        _defines.defines[PMD::REFLECTION] = true;

        if (reflectionTexture->coordinatesMode == Texture::INVCUBIC_MODE) {
          _defines.defines[PMD::INVERTCUBICMAP] = true;
        }

        _defines.defines[PMD::REFLECTIONMAP_3D] = reflectionTexture->isCube;

        switch (reflectionTexture->coordinatesMode) {
          case Texture::CUBIC_MODE:
          case Texture::INVCUBIC_MODE:
            _defines.defines[PMD::REFLECTIONMAP_CUBIC] = true;
            break;
          case Texture::EXPLICIT_MODE:
            _defines.defines[PMD::REFLECTIONMAP_EXPLICIT] = true;
            break;
          case Texture::PLANAR_MODE:
            _defines.defines[PMD::REFLECTIONMAP_PLANAR] = true;
            break;
          case Texture::PROJECTION_MODE:
            _defines.defines[PMD::REFLECTIONMAP_PROJECTION] = true;
            break;
          case Texture::SKYBOX_MODE:
            _defines.defines[PMD::REFLECTIONMAP_SKYBOX] = true;
            break;
          case Texture::SPHERICAL_MODE:
            _defines.defines[PMD::REFLECTIONMAP_SPHERICAL] = true;
            break;
          case Texture::EQUIRECTANGULAR_MODE:
            _defines.defines[PMD::REFLECTIONMAP_EQUIRECTANGULAR] = true;
            break;
          default:
            break;
        }

        HDRCubeTexture* hdrCubeTexture
          = dynamic_cast<HDRCubeTexture*>(reflectionTexture);
        if (hdrCubeTexture != nullptr) {
          _defines.defines[PMD::USESPHERICALFROMREFLECTIONMAP] = true;
          needNormals                                          = true;

          if (hdrCubeTexture->isPMREM) {
            _defines.defines[PMD::USEPMREMREFLECTION] = true;
          }
        }
      }
    }

    if (lightmapTexture && StandardMaterial::LightmapTextureEnabled) {
      if (!lightmapTexture->isReady()) {
        return false;
      }
      else {
        needUVs                                       = true;
        _defines.defines[PMD::LIGHTMAP]               = true;
        _defines.defines[PMD::USELIGHTMAPASSHADOWMAP] = useLightmapAsShadowmap;
      }
    }

    if (emissiveTexture && StandardMaterial::EmissiveTextureEnabled) {
      if (!emissiveTexture->isReady()) {
        return false;
      }
      else {
        needUVs                         = true;
        _defines.defines[PMD::EMISSIVE] = true;
      }
    }

    if (StandardMaterial::SpecularTextureEnabled) {
      if (metallicTexture) {
        if (!metallicTexture->isReady()) {
          return false;
        }
        else {
          needUVs                                     = true;
          _defines.defines[PMD::METALLICWORKFLOW]     = true;
          _defines.defines[PMD::METALLICROUGHNESSMAP] = true;
          _defines.defines[PMD::METALLICROUGHNESSGSTOREINALPHA]
            = useRoughnessFromMetallicTextureAlpha;
          _defines.defines[PMD::METALLICROUGHNESSGSTOREINGREEN]
            = !useRoughnessFromMetallicTextureAlpha
              && useRoughnessFromMetallicTextureGreen;
        }
      }
      else if (reflectivityTexture) {
        if (!reflectivityTexture->isReady()) {
          return false;
        }
        else {
          needUVs                             = true;
          _defines.defines[PMD::REFLECTIVITY] = true;
          _defines.defines[PMD::MICROSURFACEFROMREFLECTIVITYMAP]
            = useMicroSurfaceFromReflectivityMapAlpha;
          _defines.defines[PMD::MICROSURFACEAUTOMATIC]
            = useAutoMicroSurfaceFromReflectivityMap;
        }
      }
    }

    if (scene->getEngine()->getCaps().standardDerivatives && bumpTexture
        && StandardMaterial::BumpTextureEnabled && !disableBumpMap) {
      if (!bumpTexture->isReady()) {
        return false;
      }
      else {
        needUVs                     = true;
        _defines.defines[PMD::BUMP] = true;

        if (useParallax && albedoTexture
            && StandardMaterial::DiffuseTextureEnabled) {
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
      }
    }

    if (refractionTexture && StandardMaterial::RefractionTextureEnabled) {
      if (!refractionTexture->isReady()) {
        return false;
      }
      else {
        needUVs                                 = true;
        _defines.defines[PMD::REFRACTION]       = true;
        _defines.defines[PMD::REFRACTIONMAP_3D] = refractionTexture->isCube;

        if (linkRefractionWithTransparency) {
          _defines.defines[PMD::LINKREFRACTIONTOTRANSPARENCY] = true;
        }
        HDRCubeTexture* hdrCubeTexture
          = dynamic_cast<HDRCubeTexture*>(reflectionTexture);
        if (hdrCubeTexture != nullptr) {
          _defines.defines[PMD::REFRACTIONMAPINLINEARSPACE] = true;

          if (hdrCubeTexture->isPMREM) {
            _defines.defines[PMD::USEPMREMREFRACTION] = true;
          }
        }
      }
    }

    if (cameraColorGradingTexture
        && StandardMaterial::ColorGradingTextureEnabled) {
      if (!cameraColorGradingTexture->isReady()) {
        return false;
      }
      else {
        _defines.defines[PMD::CAMERACOLORGRADING] = true;
      }
    }

    if (!backFaceCulling && twoSidedLighting) {
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

  if (!std_util::almost_equal(cameraContrast, 1.f)) {
    _defines.defines[PMD::CAMERACONTRAST] = true;
  }

  if (!std_util::almost_equal(cameraExposure, 1.f)) {
    _defines.defines[PMD::CAMERATONEMAP] = true;
  }

  if (cameraColorCurves) {
    _defines.defines[PMD::CAMERACOLORCURVES] = true;
  }

  if ((!std_util::almost_equal(overloadedShadeIntensity, 1.f))
      || (!std_util::almost_equal(overloadedShadowIntensity, 1.f))) {
    _defines.defines[PMD::OVERLOADEDSHADOWVALUES] = true;
  }

  if (overloadedMicroSurfaceIntensity > 0 || overloadedEmissiveIntensity > 0
      || overloadedReflectivityIntensity > 0 || overloadedAlbedoIntensity > 0
      || overloadedAmbientIntensity > 0 || overloadedReflectionIntensity > 0) {
    _defines.defines[PMD::OVERLOADEDVALUES] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud) {
    _defines.defines[PMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled && mesh && mesh->applyFog
      && scene->fogMode != scene->FOGMODE_NONE && fogEnabled) {
    _defines.defines[PMD::FOG] = true;
  }

  if (StandardMaterial::FresnelEnabled) {
    // Fresnel
    if ((opacityFresnelParameters && opacityFresnelParameters->isEnabled)
        || (emissiveFresnelParameters
            && emissiveFresnelParameters->isEnabled)) {

      if (opacityFresnelParameters && opacityFresnelParameters->isEnabled) {
        _defines.defines[PMD::OPACITYFRESNEL] = true;
      }

      if (emissiveFresnelParameters && emissiveFresnelParameters->isEnabled) {
        _defines.defines[PMD::EMISSIVEFRESNEL] = true;
      }

      needNormals                    = true;
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
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
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
    if (mesh->useVertexColors
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[PMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha) {
        _defines.defines[PMD::VERTEXALPHA] = true;
      }
    }
    if (mesh->useBones() && mesh->computeBonesUsingShaders) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers;
      _defines.BonesPerMesh         = (mesh->skeleton()->bones.size() + 1);
    }

    // Instances
    if (useInstances) {
      _defines.defines[PMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    std::unique_ptr<EffectFallbacks> fallbacks
      = std_util::make_unique<EffectFallbacks>();
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

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get(),
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
    std::vector<std::string> attribs
      = {std::string(VertexBuffer::PositionKindChars)};

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

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  PMD::INSTANCES);

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
                                      "bumpMatrix",
                                      "lightmapMatrix",
                                      "refractionMatrix",
                                      "depthValues",
                                      "opacityParts",
                                      "emissiveLeftColor",
                                      "emissiveRightColor",
                                      "vLightingIntensity",
                                      "vOverloadedShadowIntensity",
                                      "vOverloadedIntensity",
                                      "vOverloadedAlbedo",
                                      "vOverloadedReflection",
                                      "vOverloadedReflectivity",
                                      "vOverloadedEmissive",
                                      "vOverloadedMicroSurface",
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

    std::vector<std::string> samplers
      = {"albedoSampler",         "ambientSampler",      "opacitySampler",
         "reflectionCubeSampler", "reflection2DSampler", "emissiveSampler",
         "reflectivitySampler",   "bumpSampler",         "lightmapSampler",
         "refractionCubeSampler", "refraction2DSampler"};

    ColorCurves::PrepareUniforms(uniforms);
    ColorGradingTexture::PrepareUniformsAndSamplers(uniforms, samplers);
    MaterialHelper::PrepareUniformsAndSamplersList(uniforms, samplers, _defines,
                                                   maxSimultaneousLights);

    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", maxSimultaneousLights}};

    _effect = scene->getEngine()->createEffect(
      "pbr", attribs, uniforms, samplers, join, fallbacks.get(), onCompiled,
      onError, indexParameters);
  }
  if (!_effect->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  if (mesh) {
    if (!mesh->_materialDefines) {
      mesh->_materialDefines
        = std::unique_ptr<MaterialDefines>(new PBRMaterialDefines());
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
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
  _myScene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (_myScene->getCachedMaterial() != this) {
    _effect->setMatrix("viewProjection", _myScene->getTransformMatrix());

    if (StandardMaterial::FresnelEnabled) {
      if (opacityFresnelParameters && opacityFresnelParameters->isEnabled) {
        _effect->setColor4(
          "opacityParts",
          Color3(opacityFresnelParameters->leftColor.toLuminance(),
                 opacityFresnelParameters->rightColor.toLuminance(),
                 opacityFresnelParameters->bias),
          opacityFresnelParameters->power);
      }

      if (emissiveFresnelParameters && emissiveFresnelParameters->isEnabled) {
        _effect->setColor4("emissiveLeftColor",
                           emissiveFresnelParameters->leftColor,
                           emissiveFresnelParameters->power);
        _effect->setColor4("emissiveRightColor",
                           emissiveFresnelParameters->rightColor,
                           emissiveFresnelParameters->bias);
      }
    }

    // Textures
    if (_myScene->texturesEnabled) {
      if (albedoTexture && StandardMaterial::DiffuseTextureEnabled) {
        _effect->setTexture("albedoSampler", albedoTexture);

        _effect->setFloat2("vAlbedoInfos",
                           static_cast<float>(albedoTexture->coordinatesIndex),
                           albedoTexture->level);
        _effect->setMatrix("albedoMatrix", *albedoTexture->getTextureMatrix());
      }

      if (ambientTexture && StandardMaterial::AmbientTextureEnabled) {
        _effect->setTexture("ambientSampler", ambientTexture);

        _effect->setFloat3("vAmbientInfos",
                           static_cast<float>(ambientTexture->coordinatesIndex),
                           ambientTexture->level, ambientTextureStrength);
        _effect->setMatrix("ambientMatrix",
                           *ambientTexture->getTextureMatrix());
      }

      if (opacityTexture && StandardMaterial::OpacityTextureEnabled) {
        _effect->setTexture("opacitySampler", opacityTexture);

        _effect->setFloat2("vOpacityInfos",
                           static_cast<float>(opacityTexture->coordinatesIndex),
                           opacityTexture->level);
        _effect->setMatrix("opacityMatrix",
                           *opacityTexture->getTextureMatrix());
      }

      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled) {
        _microsurfaceTextureLods.x = std::round(
          std::log(static_cast<float>(reflectionTexture->getSize().width))
          * Math::LOG2E);

        if (reflectionTexture->isCube) {
          _effect->setTexture("reflectionCubeSampler", reflectionTexture);
        }
        else {
          _effect->setTexture("reflection2DSampler", reflectionTexture);
        }

        _effect->setMatrix("reflectionMatrix",
                           *reflectionTexture->getReflectionTextureMatrix());
        _effect->setFloat2("vReflectionInfos", reflectionTexture->level, 0.f);

        if (_defines[PMD::USESPHERICALFROMREFLECTIONMAP]) {
          HDRCubeTexture* hdrCubeTexture
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

      if (emissiveTexture && StandardMaterial::EmissiveTextureEnabled) {
        _effect->setTexture("emissiveSampler", emissiveTexture);

        _effect->setFloat2(
          "vEmissiveInfos",
          static_cast<float>(emissiveTexture->coordinatesIndex),
          emissiveTexture->level);
        _effect->setMatrix("emissiveMatrix",
                           *emissiveTexture->getTextureMatrix());
      }

      if (lightmapTexture && StandardMaterial::LightmapTextureEnabled) {
        _effect->setTexture("lightmapSampler", lightmapTexture);

        _effect->setFloat2(
          "vLightmapInfos",
          static_cast<float>(lightmapTexture->coordinatesIndex),
          lightmapTexture->level);
        _effect->setMatrix("lightmapMatrix",
                           *lightmapTexture->getTextureMatrix());
      }

      if (StandardMaterial::SpecularTextureEnabled) {
        if (metallicTexture) {
          _effect->setTexture("reflectivitySampler", metallicTexture);

          _effect->setFloat2(
            "vReflectivityInfos",
            static_cast<float>(metallicTexture->coordinatesIndex),
            metallicTexture->level);
          _effect->setMatrix("reflectivityMatrix",
                             *metallicTexture->getTextureMatrix());
        }
        else if (reflectivityTexture) {
          _effect->setTexture("reflectivitySampler", reflectivityTexture);

          _effect->setFloat2(
            "vReflectivityInfos",
            static_cast<float>(reflectivityTexture->coordinatesIndex),
            reflectivityTexture->level);
          _effect->setMatrix("reflectivityMatrix",
                             *reflectivityTexture->getTextureMatrix());
        }
      }

      if (bumpTexture && _myScene->getEngine()->getCaps().standardDerivatives
          && StandardMaterial::BumpTextureEnabled && !disableBumpMap) {
        _effect->setTexture("bumpSampler", bumpTexture);

        _effect->setFloat3("vBumpInfos",
                           static_cast<float>(bumpTexture->coordinatesIndex),
                           1.f / bumpTexture->level, parallaxScaleBias);
        _effect->setMatrix("bumpMatrix", *bumpTexture->getTextureMatrix());
      }

      if (refractionTexture && StandardMaterial::RefractionTextureEnabled) {
        _microsurfaceTextureLods.y = std::round(
          std::log(static_cast<float>(refractionTexture->getSize().width))
          * Math::LOG2E);

        float depth = 1.f;
        if (refractionTexture->isCube) {
          _effect->setTexture("refractionCubeSampler", refractionTexture);
        }
        else {
          _effect->setTexture("refraction2DSampler", refractionTexture);
          _effect->setMatrix("refractionMatrix",
                             *refractionTexture->getReflectionTextureMatrix());

          RefractionTexture* _refractionTexture
            = dynamic_cast<RefractionTexture*>(refractionTexture);
          if (_refractionTexture) {
            depth = _refractionTexture->depth;
          }
        }
        _effect->setFloat4("vRefractionInfos", refractionTexture->level,
                           indexOfRefraction, depth,
                           invertRefractionY ? -1.f : 1.f);
      }

      if ((reflectionTexture || refractionTexture)) {
        _effect->setFloat2("vMicrosurfaceTextureLods",
                           _microsurfaceTextureLods.x,
                           _microsurfaceTextureLods.y);
      }

      if (cameraColorGradingTexture
          && StandardMaterial::ColorGradingTextureEnabled) {
        ColorGradingTexture::Bind(cameraColorGradingTexture, _effect);
      }
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_effect, _myScene);

    // Point size
    if (pointsCloud()) {
      _effect->setFloat("pointSize", pointSize);
    }

    // Colors
    _myScene->ambientColor.multiplyToRef(ambientColor, _globalAmbientColor);

    if (_defines[PMD::METALLICWORKFLOW]) {
      PBRMaterial::_scaledReflectivity.r = metallic;
      PBRMaterial::_scaledReflectivity.g = roughness;
      _effect->setColor4("vReflectivityColor", PBRMaterial::_scaledReflectivity,
                         0);
    }
    else {
      // GAMMA CORRECTION.
      convertColorToLinearSpaceToRef(reflectivityColor,
                                     PBRMaterial::_scaledReflectivity);
      _effect->setColor4("vReflectivityColor", PBRMaterial::_scaledReflectivity,
                         microSurface);
    }

    _effect->setVector3("vEyePosition", _myScene->_mirroredCameraPosition ?
                                          *_myScene->_mirroredCameraPosition :
                                          _myScene->activeCamera->position);
    _effect->setColor3("vAmbientColor", _globalAmbientColor);

    // GAMMA CORRECTION.
    convertColorToLinearSpaceToRef(emissiveColor, PBRMaterial::_scaledEmissive);
    _effect->setColor3("vEmissiveColor", PBRMaterial::_scaledEmissive);

    // GAMMA CORRECTION.
    convertColorToLinearSpaceToRef(reflectionColor,
                                   PBRMaterial::_scaledReflection);
    _effect->setColor3("vReflectionColor", PBRMaterial::_scaledReflection);
  }

  if (_myScene->getCachedMaterial() != this || !isFrozen()) {
    // GAMMA CORRECTION.
    convertColorToLinearSpaceToRef(albedoColor, PBRMaterial::_scaledAlbedo);
    _effect->setColor4("vAlbedoColor", PBRMaterial::_scaledAlbedo,
                       alpha * mesh->visibility);

    // Lights
    if (_myScene->lightsEnabled && !disableLighting) {
      PBRMaterial::BindLights(_myScene, mesh, _effect, _defines,
                              useScalarInLinearSpace, maxSimultaneousLights,
                              usePhysicalLightFalloff);
    }

    // View
    if ((_myScene->fogEnabled && mesh->applyFog
         && _myScene->fogMode != Scene::FOGMODE_NONE)
        || reflectionTexture) {
      _effect->setMatrix("view", _myScene->getViewMatrix());
    }

    // Fog
    MaterialHelper::BindFogParameters(_myScene, mesh, _effect);

    _lightingInfos.x = directIntensity;
    _lightingInfos.y = emissiveIntensity;
    _lightingInfos.z = environmentIntensity;
    _lightingInfos.w = specularIntensity;

    _effect->setVector4("vLightingIntensity", _lightingInfos);

    _overloadedShadowInfos.x = overloadedShadowIntensity;
    _overloadedShadowInfos.y = overloadedShadeIntensity;
    _effect->setVector4("vOverloadedShadowIntensity", _overloadedShadowInfos);

    _cameraInfos.x = cameraExposure;
    _cameraInfos.y = cameraContrast;
    _effect->setVector4("vCameraInfos", _cameraInfos);

    if (cameraColorCurves) {
      ColorCurves::Bind(*cameraColorCurves, _effect);
    }

    _overloadedIntensity.x = overloadedAmbientIntensity;
    _overloadedIntensity.y = overloadedAlbedoIntensity;
    _overloadedIntensity.z = overloadedReflectivityIntensity;
    _overloadedIntensity.w = overloadedEmissiveIntensity;
    _effect->setVector4("vOverloadedIntensity", _overloadedIntensity);

    _effect->setColor3("vOverloadedAmbient", overloadedAmbient);
    convertColorToLinearSpaceToRef(overloadedAlbedo, _tempColor);
    _effect->setColor3("vOverloadedAlbedo", _tempColor);
    convertColorToLinearSpaceToRef(overloadedReflectivity, _tempColor);
    _effect->setColor3("vOverloadedReflectivity", _tempColor);
    convertColorToLinearSpaceToRef(overloadedEmissive, _tempColor);
    _effect->setColor3("vOverloadedEmissive", _tempColor);
    convertColorToLinearSpaceToRef(overloadedReflection, _tempColor);
    _effect->setColor3("vOverloadedReflection", _tempColor);

    _overloadedMicroSurface.x = overloadedMicroSurface;
    _overloadedMicroSurface.y = overloadedMicroSurfaceIntensity;
    _overloadedMicroSurface.z = overloadedReflectionIntensity;
    _effect->setVector3("vOverloadedMicroSurface", _overloadedMicroSurface);

    // Log. depth
    MaterialHelper::BindLogDepth(_defines.defines[PMD::LOGARITHMICDEPTH],
                                 _effect, _myScene);
  }
  Material::bind(world, mesh);

  _myScene = nullptr;
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
