#include <babylon/materials/standard_material.h>

#include <babylon/animations/animation.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/color_grading_texture.h>
#include <babylon/materials/textures/refraction_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/tools/serialization_helper.h>

namespace BABYLON {

bool StandardMaterial::DiffuseTextureEnabled      = true;
bool StandardMaterial::AmbientTextureEnabled      = true;
bool StandardMaterial::OpacityTextureEnabled      = true;
bool StandardMaterial::ReflectionTextureEnabled   = true;
bool StandardMaterial::EmissiveTextureEnabled     = true;
bool StandardMaterial::SpecularTextureEnabled     = true;
bool StandardMaterial::BumpTextureEnabled         = true;
bool StandardMaterial::FresnelEnabled             = true;
bool StandardMaterial::LightmapTextureEnabled     = true;
bool StandardMaterial::RefractionTextureEnabled   = true;
bool StandardMaterial::ColorGradingTextureEnabled = true;

const Color3 StandardMaterial::_scaledDiffuse  = Color3();
const Color3 StandardMaterial::_scaledSpecular = Color3();

StandardMaterial::StandardMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , diffuseTexture{nullptr}
    , ambientTexture{nullptr}
    , opacityTexture{nullptr}
    , reflectionTexture{nullptr}
    , emissiveTexture{nullptr}
    , specularTexture{nullptr}
    , bumpTexture{nullptr}
    , lightmapTexture{nullptr}
    , refractionTexture{nullptr}
    , ambientColor{Color3(0.f, 0.f, 0.f)}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(1.f, 1.f, 1.f)}
    , emissiveColor{Color3(0.f, 0.f, 0.f)}
    , specularPower{64.f}
    , useAlphaFromDiffuseTexture{false}
    , useEmissiveAsIllumination{false}
    , linkEmissiveWithDiffuse{false}
    , useReflectionFresnelFromSpecular{false}
    , useSpecularOverAlpha{false}
    , useReflectionOverAlpha{false}
    , disableLighting{false}
    , useParallax{false}
    , useParallaxOcclusion{false}
    , parallaxScaleBias{0.05f}
    , roughness{0.f}
    , indexOfRefraction{0.98f}
    , invertRefractionY{true}
    , useLightmapAsShadowmap{false}
    , diffuseFresnelParameters{nullptr}
    , opacityFresnelParameters{nullptr}
    , reflectionFresnelParameters{nullptr}
    , refractionFresnelParameters{nullptr}
    , emissiveFresnelParameters{nullptr}
    , useGlossinessFromSpecularMapAlpha{false}
    , maxSimultaneousLights{4}
    , invertNormalMapX{false}
    , invertNormalMapY{false}
    , twoSidedLighting{false}
    , cameraColorGradingTexture{nullptr}
    , cameraColorCurves{nullptr}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _globalAmbientColor{Color3(0.f, 0.f, 0.f)}
    , _renderId{-1}
    , _cachedDefines{new StandardMaterialDefines()}
    , _useLogarithmicDepth{false}
{
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

StandardMaterial::StandardMaterial(const StandardMaterial& other)
    : Material{other.name, other.getScene()}
{
  // Base material
  other.copyTo(dynamic_cast<Material*>(this));

  // Standard material
  /*if (other.diffuseTexture) {
    diffuseTexture = other.diffuseTexture->clone();
  }
  if (other.ambientTexture) {
    ambientTexture = other.ambientTexture->clone();
  }
  if (other.opacityTexture) {
    opacityTexture = other.opacityTexture->clone();
  }
  if (other.reflectionTexture) {
    reflectionTexture = other.reflectionTexture->clone();
  }
  if (other.emissiveTexture) {
    emissiveTexture = other.emissiveTexture->clone();
  }
  if (other.specularTexture) {
    specularTexture = other.specularTexture->clone();
  }
  if (other.bumpTexture) {
    bumpTexture = other.bumpTexture->clone();
  }
  if (other.lightmapTexture) {
    lightmapTexture = other.lightmapTexture->clone();
  }
  if (other.refractionTexture) {
    refractionTexture = other.refractionTexture->clone();
  }*/

  ambientColor                     = other.ambientColor;
  diffuseColor                     = other.diffuseColor;
  specularColor                    = other.specularColor;
  emissiveColor                    = other.emissiveColor;
  specularPower                    = other.specularPower;
  useAlphaFromDiffuseTexture       = other.useAlphaFromDiffuseTexture;
  useEmissiveAsIllumination        = other.useEmissiveAsIllumination;
  linkEmissiveWithDiffuse          = other.linkEmissiveWithDiffuse;
  useReflectionFresnelFromSpecular = other.useReflectionFresnelFromSpecular;
  useSpecularOverAlpha             = other.useSpecularOverAlpha;
  useReflectionOverAlpha           = other.useReflectionOverAlpha;
  disableLighting                  = other.disableLighting;
  useParallax                      = other.useParallax;
  useParallaxOcclusion             = other.useParallaxOcclusion;
  parallaxScaleBias                = other.parallaxScaleBias;
  roughness                        = other.roughness;
  indexOfRefraction                = other.indexOfRefraction;
  invertRefractionY                = other.invertRefractionY;
  useLightmapAsShadowmap           = other.useLightmapAsShadowmap;

  if (other.diffuseFresnelParameters) {
    diffuseFresnelParameters = other.diffuseFresnelParameters->clone();
  }
  if (other.opacityFresnelParameters) {
    opacityFresnelParameters = other.opacityFresnelParameters->clone();
  }
  if (other.reflectionFresnelParameters) {
    reflectionFresnelParameters = other.reflectionFresnelParameters->clone();
  }
  if (other.refractionFresnelParameters) {
    refractionFresnelParameters = other.refractionFresnelParameters->clone();
  }
  if (other.emissiveFresnelParameters) {
    emissiveFresnelParameters = other.emissiveFresnelParameters->clone();
  }

  useGlossinessFromSpecularMapAlpha = other.useGlossinessFromSpecularMapAlpha;
  maxSimultaneousLights             = other.maxSimultaneousLights;
  invertNormalMapX                  = other.invertNormalMapX;
  invertNormalMapY                  = other.invertNormalMapY;
}

StandardMaterial::~StandardMaterial()
{
}

const char* StandardMaterial::getClassName() const
{
  return "StandardMaterial";
}

IReflect::Type StandardMaterial::type() const
{
  return IReflect::Type::STANDARDMATERIAL;
}

void StandardMaterial::setAmbientColor(const Color3& color)
{
  ambientColor = color;
}

void StandardMaterial::setDiffuseColor(const Color3& color)
{
  diffuseColor = color;
}

void StandardMaterial::setSpecularColor(const Color3& color)
{
  specularColor = color;
}
void StandardMaterial::setEmissiveColor(const Color3& color)
{
  emissiveColor = color;
}

bool StandardMaterial::useLogarithmicDepth() const
{
  return _useLogarithmicDepth;
}

void StandardMaterial::setUseLogarithmicDepth(bool value)
{
  _useLogarithmicDepth
    = value && getScene()->getEngine()->getCaps().fragmentDepthSupported;
}

bool StandardMaterial::needAlphaBlending()
{
  return (alpha < 1.f) || (opacityTexture != nullptr)
         || _shouldUseAlphaFromDiffuseTexture()
         || (opacityFresnelParameters && opacityFresnelParameters->isEnabled());
}

bool StandardMaterial::needAlphaTesting()
{
  return diffuseTexture != nullptr && diffuseTexture->hasAlpha;
}

bool StandardMaterial::_shouldUseAlphaFromDiffuseTexture()
{
  return diffuseTexture != nullptr && diffuseTexture->hasAlpha
         && useAlphaFromDiffuseTexture;
}

BaseTexture* StandardMaterial::getAlphaTestTexture()
{
  return diffuseTexture;
}

bool StandardMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                                   bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[SMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool StandardMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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

  // Lights
  if (scene->lightsEnabled() && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights, SMD::SPECULARTERM,
      SMD::SHADOWS, SMD::SHADOWFULLFLOAT);
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
    if (diffuseTexture && StandardMaterial::DiffuseTextureEnabled) {
      if (!diffuseTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[SMD::DIFFUSE] = true;
      }
    }

    if (ambientTexture && StandardMaterial::AmbientTextureEnabled) {
      if (!ambientTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[SMD::AMBIENT] = true;
      }
    }

    if (opacityTexture && StandardMaterial::OpacityTextureEnabled) {
      if (!opacityTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[SMD::OPACITY] = true;

        if (opacityTexture->getAlphaFromRGB) {
          _defines.defines[SMD::OPACITYRGB] = true;
        }
      }
    }

    if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled) {
      if (!reflectionTexture->isReady()) {
        return false;
      }
      else {
        needNormals                       = true;
        _defines.defines[SMD::REFLECTION] = true;

        if (roughness > 0) {
          _defines.defines[SMD::ROUGHNESS] = true;
        }

        if (useReflectionOverAlpha) {
          _defines.defines[SMD::REFLECTIONOVERALPHA] = true;
        }

        if (reflectionTexture->coordinatesMode == Texture::INVCUBIC_MODE) {
          _defines.defines[SMD::INVERTCUBICMAP] = true;
        }

        _defines.defines[SMD::REFLECTIONMAP_3D] = reflectionTexture->isCube;

        switch (reflectionTexture->coordinatesMode) {
          case Texture::CUBIC_MODE:
          case Texture::INVCUBIC_MODE:
            _defines.defines[SMD::REFLECTIONMAP_CUBIC] = true;
            break;
          case Texture::EXPLICIT_MODE:
            _defines.defines[SMD::REFLECTIONMAP_EXPLICIT] = true;
            break;
          case Texture::PLANAR_MODE:
            _defines.defines[SMD::REFLECTIONMAP_PLANAR] = true;
            break;
          case Texture::PROJECTION_MODE:
            _defines.defines[SMD::REFLECTIONMAP_PROJECTION] = true;
            break;
          case Texture::SKYBOX_MODE:
            _defines.defines[SMD::REFLECTIONMAP_SKYBOX] = true;
            break;
          case Texture::SPHERICAL_MODE:
            _defines.defines[SMD::REFLECTIONMAP_SPHERICAL] = true;
            break;
          case Texture::EQUIRECTANGULAR_MODE:
            _defines.defines[SMD::REFLECTIONMAP_EQUIRECTANGULAR] = true;
            break;
          case Texture::FIXED_EQUIRECTANGULAR_MODE:
            _defines.defines[SMD::REFLECTIONMAP_EQUIRECTANGULAR_FIXED] = true;
            break;
          default:
            break;
        }
      }
    }

    if (emissiveTexture && StandardMaterial::EmissiveTextureEnabled) {
      if (!emissiveTexture->isReady()) {
        return false;
      }
      else {
        needUVs                         = true;
        _defines.defines[SMD::EMISSIVE] = true;
      }
    }

    if (lightmapTexture && StandardMaterial::LightmapTextureEnabled) {
      if (!lightmapTexture->isReady()) {
        return false;
      }
      else {
        needUVs                                       = true;
        _defines.defines[SMD::LIGHTMAP]               = true;
        _defines.defines[SMD::USELIGHTMAPASSHADOWMAP] = useLightmapAsShadowmap;
      }
    }

    if (specularTexture && StandardMaterial::SpecularTextureEnabled) {
      if (!specularTexture->isReady()) {
        return false;
      }
      else {
        needUVs                           = true;
        _defines.defines[SMD::SPECULAR]   = true;
        _defines.defines[SMD::GLOSSINESS] = useGlossinessFromSpecularMapAlpha;
      }
    }

    if (scene->getEngine()->getCaps().standardDerivatives && bumpTexture
        && StandardMaterial::BumpTextureEnabled) {
      if (!bumpTexture->isReady()) {
        return false;
      }
      else {
        needUVs                     = true;
        _defines.defines[SMD::BUMP] = true;

        if (useParallax) {
          _defines.defines[SMD::PARALLAX] = true;
          if (useParallaxOcclusion) {
            _defines.defines[SMD::PARALLAXOCCLUSION] = true;
          }
        }

        if (invertNormalMapX) {
          _defines.defines[SMD::INVERTNORMALMAPX] = true;
        }

        if (invertNormalMapY) {
          _defines.defines[SMD::INVERTNORMALMAPY] = true;
        }

        if (scene->_mirroredCameraPosition) {
          _defines.defines[SMD::INVERTNORMALMAPX]
            = !_defines.defines[SMD::INVERTNORMALMAPX];
          _defines.defines[SMD::INVERTNORMALMAPY]
            = !_defines.defines[SMD::INVERTNORMALMAPY];
        }
      }
    }

    if (refractionTexture && StandardMaterial::RefractionTextureEnabled) {
      if (!refractionTexture->isReady()) {
        return false;
      }
      else {
        needUVs                           = true;
        _defines.defines[SMD::REFRACTION] = true;

        _defines.defines[SMD::REFRACTIONMAP_3D] = refractionTexture->isCube;
      }
    }

    if (cameraColorGradingTexture
        && StandardMaterial::ColorGradingTextureEnabled) {
      if (!cameraColorGradingTexture->isReady()) {
        return false;
      }
      else {
        _defines.defines[SMD::CAMERACOLORGRADING] = true;
      }
    }

    if (!backFaceCulling && twoSidedLighting) {
      _defines.defines[SMD::TWOSIDEDLIGHTING] = true;
    }
  }

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[SMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[SMD::ALPHATEST] = true;
  }

  if (_shouldUseAlphaFromDiffuseTexture()) {
    _defines.defines[SMD::ALPHAFROMDIFFUSE] = true;
  }

  if (useEmissiveAsIllumination) {
    _defines.defines[SMD::EMISSIVEASILLUMINATION] = true;
  }

  if (linkEmissiveWithDiffuse) {
    _defines.defines[SMD::LINKEMISSIVEWITHDIFFUSE] = true;
  }

  if (useLogarithmicDepth()) {
    _defines.defines[SMD::LOGARITHMICDEPTH] = true;
  }

  if (cameraColorCurves) {
    _defines.defines[SMD::CAMERACOLORCURVES] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[SMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[SMD::FOG] = true;
  }

  if (StandardMaterial::FresnelEnabled) {
    // Fresnel
    if ((diffuseFresnelParameters && diffuseFresnelParameters->isEnabled())
        || (opacityFresnelParameters && opacityFresnelParameters->isEnabled())
        || (emissiveFresnelParameters && emissiveFresnelParameters->isEnabled())
        || (refractionFresnelParameters
            && refractionFresnelParameters->isEnabled())
        || (reflectionFresnelParameters
            && reflectionFresnelParameters->isEnabled())) {

      if (diffuseFresnelParameters && diffuseFresnelParameters->isEnabled()) {
        _defines.defines[SMD::DIFFUSEFRESNEL] = true;
      }

      if (opacityFresnelParameters && opacityFresnelParameters->isEnabled()) {
        _defines.defines[SMD::OPACITYFRESNEL] = true;
      }

      if (reflectionFresnelParameters
          && reflectionFresnelParameters->isEnabled()) {
        _defines.defines[SMD::REFLECTIONFRESNEL] = true;

        if (useReflectionFresnelFromSpecular) {
          _defines.defines[SMD::REFLECTIONFRESNELFROMSPECULAR] = true;
        }
      }

      if (refractionFresnelParameters
          && refractionFresnelParameters->isEnabled()) {
        _defines.defines[SMD::REFRACTIONFRESNEL] = true;
      }

      if (emissiveFresnelParameters && emissiveFresnelParameters->isEnabled()) {
        _defines.defines[SMD::EMISSIVEFRESNEL] = true;
      }

      needNormals                    = true;
      _defines.defines[SMD::FRESNEL] = true;
    }
  }

  if (_defines.defines[SMD::SPECULARTERM] && useSpecularOverAlpha) {
    _defines.defines[SMD::SPECULAROVERALPHA] = true;
  }

  // Attribs
  if (mesh) {
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[SMD::NORMAL] = true;
    }
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[SMD::UV1] = true;
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        _defines.defines[SMD::UV2] = true;
      }
    }
    if (mesh->useVertexColors
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[SMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha) {
        _defines.defines[SMD::VERTEXALPHA] = true;
      }
    }
    if (mesh->useBones() && mesh->computeBonesUsingShaders) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers;
      _defines.BonesPerMesh         = mesh->skeleton()->bones.size() + 1;
    }

    // Instances
    if (useInstances) {
      _defines.defines[SMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    std::unique_ptr<EffectFallbacks> fallbacks(new EffectFallbacks());
    if (_defines[SMD::REFLECTION]) {
      fallbacks->addFallback(0, "REFLECTION");
    }

    if (_defines[SMD::SPECULAR]) {
      fallbacks->addFallback(0, "SPECULAR");
    }

    if (_defines[SMD::BUMP]) {
      fallbacks->addFallback(0, "BUMP");
    }

    if (_defines[SMD::PARALLAX]) {
      fallbacks->addFallback(1, "PARALLAX");
    }

    if (_defines[SMD::PARALLAXOCCLUSION]) {
      fallbacks->addFallback(0, "PARALLAXOCCLUSION");
    }

    if (_defines[SMD::SPECULAROVERALPHA]) {
      fallbacks->addFallback(0, "SPECULAROVERALPHA");
    }

    if (_defines[SMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (_defines[SMD::POINTSIZE]) {
      fallbacks->addFallback(0, "POINTSIZE");
    }

    if (_defines[SMD::LOGARITHMICDEPTH]) {
      fallbacks->addFallback(0, "LOGARITHMICDEPTH");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get(),
                                              maxSimultaneousLights);

    if (_defines[SMD::SPECULARTERM]) {
      fallbacks->addFallback(0, "SPECULARTERM");
    }

    if (_defines[SMD::DIFFUSEFRESNEL]) {
      fallbacks->addFallback(1, "DIFFUSEFRESNEL");
    }

    if (_defines[SMD::OPACITYFRESNEL]) {
      fallbacks->addFallback(2, "OPACITYFRESNEL");
    }

    if (_defines[SMD::REFLECTIONFRESNEL]) {
      fallbacks->addFallback(3, "REFLECTIONFRESNEL");
    }

    if (_defines[SMD::EMISSIVEFRESNEL]) {
      fallbacks->addFallback(4, "EMISSIVEFRESNEL");
    }

    if (_defines[SMD::FRESNEL]) {
      fallbacks->addFallback(4, "FRESNEL");
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

    if (_defines[SMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (_defines[SMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (_defines[SMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (_defines[SMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  SMD::INSTANCES);

    // Legacy browser patch
    std::string shaderName = "default";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{"world",
                                      "view",
                                      "viewProjection",
                                      "vEyePosition",
                                      "vLightsType",
                                      "vAmbientColor",
                                      "vDiffuseColor",
                                      "vSpecularColor",
                                      "vEmissiveColor",
                                      "vFogInfos",
                                      "vFogColor",
                                      "pointSize",
                                      "vDiffuseInfos",
                                      "vAmbientInfos",
                                      "vOpacityInfos",
                                      "vReflectionInfos",
                                      "vEmissiveInfos",
                                      "vSpecularInfos",
                                      "vBumpInfos",
                                      "vLightmapInfos",
                                      "vRefractionInfos",
                                      "mBones",
                                      "vClipPlane",
                                      "diffuseMatrix",
                                      "ambientMatrix",
                                      "opacityMatrix",
                                      "reflectionMatrix",
                                      "emissiveMatrix",
                                      "specularMatrix",
                                      "bumpMatrix",
                                      "lightmapMatrix",
                                      "refractionMatrix",
                                      "depthValues",
                                      "diffuseLeftColor",
                                      "diffuseRightColor",
                                      "opacityParts",
                                      "reflectionLeftColor",
                                      "reflectionRightColor",
                                      "emissiveLeftColor",
                                      "emissiveRightColor",
                                      "refractionLeftColor",
                                      "refractionRightColor",
                                      "logarithmicDepthConstant"};
    std::vector<std::string> samplers{
      "diffuseSampler",        "ambientSampler",      "opacitySampler",
      "reflectionCubeSampler", "reflection2DSampler", "emissiveSampler",
      "specularSampler",       "bumpSampler",         "lightmapSampler",
      "refractionCubeSampler", "refraction2DSampler"};
    if (_defines[SMD::CAMERACOLORCURVES]) {
      ColorCurves::PrepareUniforms(uniforms);
    }
    if (_defines[SMD::CAMERACOLORGRADING]) {
      ColorGradingTexture::PrepareUniformsAndSamplers(uniforms, samplers);
    }
    MaterialHelper::PrepareUniformsAndSamplersList(uniforms, samplers, _defines,
                                                   maxSimultaneousLights);

    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", maxSimultaneousLights - 1}};

    _effect = scene->getEngine()->createEffect(
      shaderName, attribs, uniforms, samplers, join, fallbacks.get(),
      onCompiled, onError, indexParameters);
  }
  if (!_effect->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  if (mesh) {
    if (!mesh->_materialDefines) {
      mesh->_materialDefines
        = std::unique_ptr<MaterialDefines>(new StandardMaterialDefines());
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void StandardMaterial::unbind()
{
  if (reflectionTexture && reflectionTexture->isRenderTarget) {
    _effect->setTexture("reflection2DSampler", nullptr);
  }

  if (refractionTexture && refractionTexture->isRenderTarget) {
    _effect->setTexture("refraction2DSampler", nullptr);
  }

  Material::unbind();
}

void StandardMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void StandardMaterial::bind(Matrix* world, Mesh* mesh)
{
  Scene* scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (scene->getCachedMaterial() != this) {
    _effect->setMatrix("viewProjection", scene->getTransformMatrix());

    if (StandardMaterial::FresnelEnabled) {
      // Fresnel
      if (diffuseFresnelParameters && diffuseFresnelParameters->isEnabled()) {
        _effect->setColor4("diffuseLeftColor",
                           diffuseFresnelParameters->leftColor,
                           diffuseFresnelParameters->power);
        _effect->setColor4("diffuseRightColor",
                           diffuseFresnelParameters->rightColor,
                           diffuseFresnelParameters->bias);
      }

      if (opacityFresnelParameters && opacityFresnelParameters->isEnabled()) {
        _effect->setColor4(
          "opacityParts",
          Color3(opacityFresnelParameters->leftColor.toLuminance(),
                 opacityFresnelParameters->rightColor.toLuminance(),
                 opacityFresnelParameters->bias),
          static_cast<float>(opacityFresnelParameters->power));
      }

      if (reflectionFresnelParameters
          && reflectionFresnelParameters->isEnabled()) {
        _effect->setColor4("reflectionLeftColor",
                           reflectionFresnelParameters->leftColor,
                           reflectionFresnelParameters->power);
        _effect->setColor4("reflectionRightColor",
                           reflectionFresnelParameters->rightColor,
                           reflectionFresnelParameters->bias);
      }

      if (refractionFresnelParameters
          && refractionFresnelParameters->isEnabled()) {
        _effect->setColor4("refractionLeftColor",
                           refractionFresnelParameters->leftColor,
                           refractionFresnelParameters->power);
        _effect->setColor4("refractionRightColor",
                           refractionFresnelParameters->rightColor,
                           refractionFresnelParameters->bias);
      }

      if (emissiveFresnelParameters && emissiveFresnelParameters->isEnabled()) {
        _effect->setColor4("emissiveLeftColor",
                           emissiveFresnelParameters->leftColor,
                           emissiveFresnelParameters->power);
        _effect->setColor4("emissiveRightColor",
                           emissiveFresnelParameters->rightColor,
                           emissiveFresnelParameters->bias);
      }
    }

    // Textures
    if (scene->texturesEnabled()) {
      if (diffuseTexture && StandardMaterial::DiffuseTextureEnabled) {
        _effect->setTexture("diffuseSampler", diffuseTexture);

        _effect->setFloat2("vDiffuseInfos",
                           static_cast<float>(diffuseTexture->coordinatesIndex),
                           diffuseTexture->level);
        _effect->setMatrix("diffuseMatrix",
                           *diffuseTexture->getTextureMatrix());
      }

      if (ambientTexture && StandardMaterial::AmbientTextureEnabled) {
        _effect->setTexture("ambientSampler", ambientTexture);

        _effect->setFloat2("vAmbientInfos",
                           static_cast<float>(ambientTexture->coordinatesIndex),
                           ambientTexture->level);
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
        if (reflectionTexture->isCube) {
          _effect->setTexture("reflectionCubeSampler", reflectionTexture);
        }
        else {
          _effect->setTexture("reflection2DSampler", reflectionTexture);
        }

        _effect->setMatrix("reflectionMatrix",
                           *reflectionTexture->getReflectionTextureMatrix());
        _effect->setFloat2("vReflectionInfos", reflectionTexture->level,
                           roughness);
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

      if (specularTexture && StandardMaterial::SpecularTextureEnabled) {
        _effect->setTexture("specularSampler", specularTexture);

        _effect->setFloat2(
          "vSpecularInfos",
          static_cast<float>(specularTexture->coordinatesIndex),
          specularTexture->level);
        _effect->setMatrix("specularMatrix",
                           *specularTexture->getTextureMatrix());
      }

      if (bumpTexture && scene->getEngine()->getCaps().standardDerivatives
          && StandardMaterial::BumpTextureEnabled) {
        _effect->setTexture("bumpSampler", bumpTexture);

        _effect->setFloat2("vBumpInfos",
                           static_cast<float>(bumpTexture->coordinatesIndex),
                           1.f / bumpTexture->level);
        _effect->setMatrix("bumpMatrix", *bumpTexture->getTextureMatrix());
      }

      if (refractionTexture && StandardMaterial::RefractionTextureEnabled) {
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
                           invertRefractionY ? -1 : 1);
      }
      if (cameraColorGradingTexture
          && StandardMaterial::ColorGradingTextureEnabled) {
        ColorGradingTexture::Bind(cameraColorGradingTexture, _effect);
      }
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_effect, scene);

    // Point size
    if (pointsCloud()) {
      _effect->setFloat("pointSize", pointSize);
    }

    // Colors
    scene->ambientColor.multiplyToRef(ambientColor, _globalAmbientColor);

    _effect->setVector3("vEyePosition", scene->_mirroredCameraPosition ?
                                          *scene->_mirroredCameraPosition :
                                          scene->activeCamera->position);
    _effect->setColor3("vAmbientColor", _globalAmbientColor);

    if (_defines[SMD::SPECULARTERM]) {
      _effect->setColor4("vSpecularColor", specularColor, specularPower);
    }
    _effect->setColor3("vEmissiveColor", emissiveColor);
  }

  if (scene->getCachedMaterial() != this || !isFrozen()) {
    // Diffuse
    _effect->setColor4("vDiffuseColor", diffuseColor, alpha * mesh->visibility);

    // Lights
    if (scene->lightsEnabled() && !disableLighting) {
      MaterialHelper::BindLights(scene, mesh, _effect,
                                 _defines.defines[SMD::SPECULARTERM],
                                 maxSimultaneousLights);
    }

    // View
    if ((scene->fogEnabled() && mesh->applyFog
         && scene->fogMode() != scene->FOGMODE_NONE)
        || reflectionTexture || refractionTexture) {
      _effect->setMatrix("view", scene->getViewMatrix());
    }

    // Fog
    MaterialHelper::BindFogParameters(scene, mesh, _effect);

    // Log. depth
    MaterialHelper::BindLogDepth(_defines.defines[SMD::LOGARITHMICDEPTH],
                                 _effect, scene);

    // Color Curves
    if (cameraColorCurves) {
      // ColorCurves::Bind(cameraColorCurves, _effect);
    }
  }

  Material::bind(world, mesh);
}

std::vector<IAnimatable*> StandardMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (diffuseTexture && diffuseTexture->animations.size() > 0) {
    results.emplace_back(diffuseTexture);
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

  if (specularTexture && specularTexture->animations.size() > 0) {
    results.emplace_back(specularTexture);
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

void StandardMaterial::dispose(bool forceDisposeEffect,
                               bool forceDisposeTextures)
{
  if (forceDisposeTextures) {
    if (diffuseTexture) {
      diffuseTexture->dispose();
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

    if (specularTexture) {
      specularTexture->dispose();
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

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* StandardMaterial::clone(const std::string& _name,
                                  bool /*cloneChildren*/) const
{
  auto standardMaterial  = StandardMaterial::New(*this);
  standardMaterial->name = _name;
  return standardMaterial;
}

Json::object StandardMaterial::serialize() const
{
  return Json::object();
}

// Statics
StandardMaterial* StandardMaterial::Parse(const Json::value& source,
                                          Scene* scene,
                                          const std::string& rootUrl)
{
  return SerializationHelper::Parse(
    StandardMaterial::New(Json::GetString(source, "name"), scene), source,
    scene, rootUrl);
}

} // end of namespace BABYLON
