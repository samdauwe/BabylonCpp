#include <babylon/materialslibrary/custom/custom_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/color_grading_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materialslibrary/custom/custom_shader_helper.h>
#include <babylon/materialslibrary/custom/icustom_material_builder.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

CustomMaterial::CustomMaterial(const std::string& iName,
                               ICustomMaterialBuilder* iBuilder, Scene* scene)
    : StandardMaterial{iName, scene}, builder{iBuilder}
{
}

CustomMaterial::~CustomMaterial()
{
}

bool CustomMaterial::isReady(AbstractMesh* mesh, bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady) {
      return true;
    }
  }

  auto scene       = getScene();
  auto engine      = scene->getEngine();
  bool needUVs     = false;
  bool needNormals = false;

  _defines.reset();

  // Lights
  if (scene->lightsEnabled && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights);
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
  if (pointsCloud() || scene->forcePointsCloud) {
    _defines.defines[SMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled && mesh && mesh->applyFog
      && scene->fogMode != Scene::FOGMODE_NONE && fogEnabled) {
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
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines);

    std::string shaderName = "default";

    if (builder) {
      CustomShaderHelper helper;
      shaderName = builder->build(helper, shaderName, _mainPart, _diffusePart,
                                  _vertexPositionPart);
    }

    std::string join = _defines.toString();
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
        = std_util::make_unique<MaterialDefines>(StandardMaterialDefines());
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
