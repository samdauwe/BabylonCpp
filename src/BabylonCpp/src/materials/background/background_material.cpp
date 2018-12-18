#include <babylon/materials/background/background_material.h>

#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/background/background_material_defines.h>
#include <babylon/materials/color_curves.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

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
    , useEquirectangularFOV{false}
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
    , switchToBGR{false}
    , _imageProcessingObserver{nullptr}
    , _reflectionControls{Vector4::Zero()}
    , _white{Color3::White()}
    , _primaryShadowColor{Color3::Black()}
    , _primaryHighlightColor{Color3::Black()}
{
  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);

#if 0
  getRenderTargetTextures = [this](std::vector<RenderTargetTexture*>()) {
    _renderTargets.clear();

    if (_diffuseTexture && _diffuseTexture->isRenderTarget) {
      _renderTargets.emplace_back(_diffuseTexture);
    }

    if (_reflectionTexture && _reflectionTexture->isRenderTarget) {
      _renderTargets.emplace_back(_reflectionTexture);
    }

    return _renderTargets;
  };
#endif
}

BackgroundMaterial::~BackgroundMaterial()
{
}

void BackgroundMaterial::setReflectionStandardFresnelWeight(float value)
{
  auto reflectionWeight = value;

  if (reflectionWeight < 0.5f) {
    reflectionWeight = reflectionWeight * 2.f;
    setReflectionReflectance0(BackgroundMaterial::StandardReflectance0()
                              * reflectionWeight);
    setReflectionReflectance90(BackgroundMaterial::StandardReflectance90()
                               * reflectionWeight);
  }
  else {
    reflectionWeight = reflectionWeight * 2.f - 1.f;
    setReflectionReflectance0(
      BackgroundMaterial::StandardReflectance0()
      + (1.f - BackgroundMaterial::StandardReflectance0()) * reflectionWeight);
    setReflectionReflectance90(
      BackgroundMaterial::StandardReflectance90()
      + (1.f - BackgroundMaterial::StandardReflectance90()) * reflectionWeight);
  }
}

float BackgroundMaterial::fovMultiplier() const
{
  return _fovMultiplier;
}

void BackgroundMaterial::setFovMultiplier(float value)
{
  if (std::isnan(value)) {
    value = 1.f;
  }
  _fovMultiplier = std::max(0.f, std::min(2.f, value));
}

void BackgroundMaterial::_attachImageProcessingConfiguration(
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
        [this](ImageProcessingConfiguration* /*conf*/, EventState /*es*/) {
          _computePrimaryColorFromPerceptualColor();
          _markAllSubMeshesAsImageProcessingDirty();
        });
  }
}

ImageProcessingConfiguration*
BackgroundMaterial::imageProcessingConfiguration() const
{
  return _imageProcessingConfiguration;
}

void BackgroundMaterial::setImageProcessingConfiguration(
  ImageProcessingConfiguration* value)
{
  _attachImageProcessingConfiguration(value);

  // Ensure the effect will be rebuilt.
  _markAllSubMeshesAsTexturesDirty();
}

bool BackgroundMaterial::cameraColorCurvesEnabled() const
{
  return _imageProcessingConfiguration->colorCurvesEnabled();
}

void BackgroundMaterial::setCameraColorCurvesEnabled(bool value)
{
  _imageProcessingConfiguration->colorCurvesEnabled = value;
}

bool BackgroundMaterial::cameraColorGradingEnabled() const
{
  return _imageProcessingConfiguration->colorGradingEnabled();
}

void BackgroundMaterial::setCameraColorGradingEnabled(bool value)
{
  _imageProcessingConfiguration->colorGradingEnabled = value;
}

bool BackgroundMaterial::cameraToneMappingEnabled() const
{
  return _imageProcessingConfiguration->toneMappingEnabled();
}

void BackgroundMaterial::setCameraToneMappingEnabled(bool value)
{
  _imageProcessingConfiguration->toneMappingEnabled = value;
}

float BackgroundMaterial::cameraExposure()
{
  return _imageProcessingConfiguration->exposure();
}

void BackgroundMaterial::setCameraExposure(float value)
{
  _imageProcessingConfiguration->exposure = value;
}

float BackgroundMaterial::cameraContrast() const
{
  return _imageProcessingConfiguration->contrast();
}

void BackgroundMaterial::setCameraContrast(float value)
{
  _imageProcessingConfiguration->contrast = value;
}

BaseTexturePtr& BackgroundMaterial::cameraColorGradingTexture()
{
  return _imageProcessingConfiguration->colorGradingTexture;
}

void BackgroundMaterial::setCameraColorGradingTexture(
  const BaseTexturePtr& value)
{
  _imageProcessingConfiguration->colorGradingTexture = value;
}

ColorCurves* BackgroundMaterial::cameraColorCurves()
{
  return _imageProcessingConfiguration->colorCurves.get();
}

void BackgroundMaterial::setCameraColorCurves(ColorCurves* value)
{
  _imageProcessingConfiguration->colorCurves
    = std::make_unique<ColorCurves>(*value);
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
  return ((alpha() < 0.f)
          || (_diffuseTexture != nullptr && _diffuseTexture->hasAlpha()));
}

bool BackgroundMaterial::isReadyForSubMesh(AbstractMesh* mesh,
                                           BaseSubMesh* subMesh,
                                           bool useInstances)
{
  if (subMesh->effect() && isFrozen()) {
    if (_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<BackgroundMaterialDefines>();
  }

  auto scene   = getScene();
  auto defines = *(
    static_cast<BackgroundMaterialDefines*>(subMesh->_materialDefines.get()));
  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (defines._renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Lights
  MaterialHelper::PrepareDefinesForLights(scene, mesh, defines, false,
                                          _maxSimultaneousLights);
  defines._needNormals = true;

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (scene->getEngine()->getCaps().textureLOD) {
        defines.boolDef["TEXTURELODSUPPORT"] = true;
      }

      if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
        if (!_diffuseTexture->isReadyOrNotBlocking()) {
          return false;
        }

        MaterialHelper::PrepareDefinesForMergedUV(_diffuseTexture, defines,
                                                  "DIFFUSE");
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

      auto reflectionTexture = _reflectionTexture;
      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        if (!reflectionTexture->isReadyOrNotBlocking()) {
          return false;
        }

        defines.boolDef["REFLECTION"]      = true;
        defines.boolDef["GAMMAREFLECTION"] = reflectionTexture->gammaSpace;
        defines.boolDef["RGBDREFLECTION"]  = reflectionTexture->isRGBD();
        defines.boolDef["REFLECTIONBLUR"]  = _reflectionBlur > 0;
        defines.boolDef["REFLECTIONMAP_OPPOSITEZ"]
          = getScene()->useRightHandedSystem() ? !reflectionTexture->invertZ :
                                                 reflectionTexture->invertZ;
        defines.boolDef["LODINREFLECTIONALPHA"]
          = reflectionTexture->lodLevelInAlpha;
        defines.boolDef["EQUIRECTANGULAR_RELFECTION_FOV"]
          = useEquirectangularFOV;
        defines.boolDef["REFLECTIONBGR"] = switchToBGR;

        if (reflectionTexture->coordinatesMode
            == TextureConstants::INVCUBIC_MODE) {
          defines.boolDef["INVERTCUBICMAP"] = true;
        }

        defines.boolDef["REFLECTIONMAP_3D"] = reflectionTexture->isCube;

        switch (reflectionTexture->coordinatesMode) {
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
            defines.boolDef["REFLECTIONMAP_SKYBOX_TRANSFORMED"]
              = !reflectionTexture->getReflectionTextureMatrix()->isIdentity();
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
          default:
            defines.boolDef["REFLECTIONMAP_CUBIC"] = true;
            break;
        }

        if (reflectionFresnel()) {
          defines.boolDef["REFLECTIONFRESNEL"] = true;
          defines.boolDef["REFLECTIONFALLOFF"]
            = reflectionFalloffDistance() > 0;

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
        defines.boolDef["REFLECTIONMAP_SKYBOX_TRANSFORMED"]            = false;
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
      = (alphaMode() == EngineConstants::ALPHA_PREMULTIPLIED
         || alphaMode() == EngineConstants::ALPHA_PREMULTIPLIED_PORTERDUFF);
    defines.boolDef["USERGBCOLOR"] = _useRGBColor;
    defines.boolDef["NOISE"]       = _enableNoise;
  }

  if (defines._areLightsDirty) {
    defines.boolDef["USEHIGHLIGHTANDSHADOWCOLORS"]
      = !_useRGBColor
        && (_primaryColorShadowLevel != 0.f
            || _primaryColorHighlightLevel != 0.f);
  }

  if (defines._areImageProcessingDirty && _imageProcessingConfiguration) {
    if (!_imageProcessingConfiguration->isReady()) {
      return false;
    }

    _imageProcessingConfiguration->prepareDefines(defines);
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(),
                                        fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(scene, engine, defines,
                                                    useInstances);

  // Attribs
  if (MaterialHelper::PrepareDefinesForAttributes(mesh, defines, false, true,
                                                  false)) {
    if (mesh) {
      if (!scene->getEngine()->getCaps().standardDerivatives
          && !mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
        mesh->createNormals(true);
        BABYLON_LOGF_WARN(
          "BackgroundMaterial",
          "BackgroundMaterial: Normals have been created for the mesh: %s",
          mesh->name.c_str());
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

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines["UV2"]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
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
    std::vector<std::string> samplers{"diffuseSampler", "reflectionSampler",
                                      "reflectionSamplerLow",
                                      "reflectionSamplerHigh"};
    std::vector<std::string> uniformBuffers{"Material", "Scene"};

    ImageProcessingConfiguration::PrepareUniforms(uniforms, defines);
    ImageProcessingConfiguration::PrepareSamplers(samplers, defines);

    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", _maxSimultaneousLights}};

    std::function<void(Effect * effect)> onCompiled = [&](Effect* effect) {
      if (onCompiled) {
        onCompiled(effect);
      }

      bindSceneUniformBuffer(effect, scene->getSceneUniformBuffer());
    };

    auto join = defines.toString();

    EffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.uniformBuffersNames   = std::move(uniformBuffers);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.indexParameters       = std::move(indexParameters);
    options.maxSimultaneousLights = _maxSimultaneousLights;

    MaterialHelper::PrepareUniformsAndSamplersList(options);

    subMesh->setEffect(
      scene->getEngine()->createEffect("background", options, engine), defines);

    buildUniformLayout();
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  defines._renderId   = scene->getRenderId();
  _wasPreviouslyReady = true;

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
    _primaryColor.scaleToRef(1.f / _imageProcessingConfiguration->exposure(),
                             _primaryColor);
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
  _primaryHighlightColor.scaleToRef(_primaryColorHighlightLevel,
                                    _primaryHighlightColor);
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

void BackgroundMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _activeEffect->setMatrix("world", world);
}

void BackgroundMaterial::bindForSubMesh(Matrix& world, Mesh* mesh,
                                        SubMesh* subMesh)
{
  auto scene = getScene();

  auto definesTmp
    = static_cast<BackgroundMaterialDefines*>(subMesh->_materialDefines.get());
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
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  auto mustRebind = _mustRebind(scene, effect, mesh->visibility());
  if (mustRebind) {
    _uniformBuffer->bindToEffect(effect.get(), "Material");

    bindViewProjection(effect);

    auto reflectionTexture = _reflectionTexture;
    if (!_uniformBuffer->useUbo() || !isFrozen() || !_uniformBuffer->isSync()) {

      // Texture uniforms
      if (scene->texturesEnabled()) {
        if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
          _uniformBuffer->updateFloat2("vDiffuseInfos",
                                       _diffuseTexture->coordinatesIndex,
                                       _diffuseTexture->level, "");
          MaterialHelper::BindTextureMatrix(*_diffuseTexture, *_uniformBuffer,
                                            "diffuse");
        }

        if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
          _uniformBuffer->updateMatrix(
            "reflectionMatrix",
            *reflectionTexture->getReflectionTextureMatrix());
          _uniformBuffer->updateFloat2(
            "vReflectionInfos", reflectionTexture->level, _reflectionBlur, "");

          _uniformBuffer->updateFloat3(
            "vReflectionMicrosurfaceInfos", reflectionTexture->getSize().width,
            reflectionTexture->lodGenerationScale,
            reflectionTexture->lodGenerationOffset, "");
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
        _uniformBuffer->updateColor4("vPrimaryColor", _primaryHighlightColor,
                                     1.f, "");
        _uniformBuffer->updateColor4("vPrimaryColorShadow", _primaryShadowColor,
                                     1.f, "");
      }
      else {
        _uniformBuffer->updateColor4("vPrimaryColor", _primaryColor, 1.f, "");
      }
    }

    _uniformBuffer->updateFloat("fFovMultiplier", _fovMultiplier);

    // Textures
    if (scene->texturesEnabled()) {
      if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
        _uniformBuffer->setTexture("diffuseSampler", _diffuseTexture);
      }

      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        if (defines["REFLECTIONBLUR"] && defines["TEXTURELODSUPPORT"]) {
          _uniformBuffer->setTexture("reflectionSampler", reflectionTexture);
        }
        else if (!defines["REFLECTIONBLUR"]) {
          _uniformBuffer->setTexture("reflectionSampler", reflectionTexture);
        }
        else {
          _uniformBuffer->setTexture("reflectionSampler",
                                     reflectionTexture->_lodTextureMid() ?
                                       reflectionTexture->_lodTextureMid() :
                                       reflectionTexture);
          _uniformBuffer->setTexture("reflectionSamplerLow",
                                     reflectionTexture->_lodTextureLow() ?
                                       reflectionTexture->_lodTextureLow() :
                                       reflectionTexture);
          _uniformBuffer->setTexture("reflectionSamplerHigh",
                                     reflectionTexture->_lodTextureHigh() ?
                                       reflectionTexture->_lodTextureHigh() :
                                       reflectionTexture);
        }

        if (defines["REFLECTIONFRESNEL"]) {
          _uniformBuffer->updateFloat3("vBackgroundCenter", sceneCenter().x,
                                       sceneCenter().y, sceneCenter().z, "");
          _uniformBuffer->updateFloat4(
            "vReflectionControl", _reflectionControls.x, _reflectionControls.y,
            _reflectionControls.z, _reflectionControls.w, "");
        }
      }
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    MaterialHelper::BindEyePosition(effect, scene);
  }

  if (mustRebind || !isFrozen()) {
    if (scene->lightsEnabled()) {
      MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                                 _maxSimultaneousLights, false);
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

  _uniformBuffer->update();

  Material::_afterBind(mesh /*, _activeEffect*/);
}

void BackgroundMaterial::dispose(bool forceDisposeEffect,
                                 bool forceDisposeTextures)
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
    _imageProcessingConfiguration->onUpdateParameters.remove(
      _imageProcessingObserver);
  }

  PushMaterial::dispose(forceDisposeEffect);
}

MaterialPtr BackgroundMaterial::clone(const std::string& /*name*/,
                                      bool /*cloneChildren*/) const
{
  return nullptr;
}

json BackgroundMaterial::serialize() const
{
  return nullptr;
}

const std::string BackgroundMaterial::getClassName() const
{
  return "BackgroundMaterial";
}

std::unique_ptr<BackgroundMaterial>
BackgroundMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                          const std::string& /*url*/)
{
  return nullptr;
}

const Color3& BackgroundMaterial::primaryColor() const
{
  return _primaryColor;
}

void BackgroundMaterial::setPrimaryColor(const Color3& value)
{
  _primaryColor = value;
}

std::optional<Color3>& BackgroundMaterial::_perceptualColor()
{
  return __perceptualColor;
}

void BackgroundMaterial::setPerceptualColor(const std::optional<Color3>& value)
{
  __perceptualColor = value;
  _computePrimaryColorFromPerceptualColor();
  _markAllSubMeshesAsLightsDirty();
}

float BackgroundMaterial::primaryColorShadowLevel() const
{
  return _primaryColorShadowLevel;
}

void BackgroundMaterial::setPrimaryColorShadowLevel(float value)
{
  _primaryColorShadowLevel = value;
  _computePrimaryColors();
  _markAllSubMeshesAsLightsDirty();
}

float BackgroundMaterial::primaryColorHighlightLevel() const
{
  return _primaryColorHighlightLevel;
}

void BackgroundMaterial::setPrimaryColorHighlightLevel(float value)
{
  _primaryColorHighlightLevel = value;
  _computePrimaryColors();
  _markAllSubMeshesAsLightsDirty();
}

BaseTexturePtr BackgroundMaterial::reflectionTexture()
{
  return std::static_pointer_cast<BaseTexture>(_reflectionTexture);
}

void BackgroundMaterial::setReflectionTexture(
  const RenderTargetTexturePtr& value)
{
  _reflectionTexture = value;
}

BaseTexturePtr BackgroundMaterial::diffuseTexture()
{
  return std::static_pointer_cast<BaseTexture>(_diffuseTexture);
}

void BackgroundMaterial::setDiffuseTexture(const RenderTargetTexturePtr& value)
{
  _diffuseTexture = value;
}

float BackgroundMaterial::shadowLevel() const
{
  return _shadowLevel;
}

void BackgroundMaterial::setShadowLevel(float value)
{
  _shadowLevel = value;
}

const Vector3& BackgroundMaterial::sceneCenter() const
{
  return _sceneCenter;
}

void BackgroundMaterial::setSceneCenter(const Vector3& value)
{
  _sceneCenter = value;
}

bool BackgroundMaterial::opacityFresnel() const
{
  return _opacityFresnel;
}

void BackgroundMaterial::setOpacityFresnel(bool value)
{
  _opacityFresnel = value;
}

bool BackgroundMaterial::reflectionFresnel() const
{
  return _reflectionFresnel;
}

void BackgroundMaterial::setReflectionFresnel(bool value)
{
  _reflectionFresnel = value;
}

float BackgroundMaterial::reflectionFalloffDistance() const
{
  return _reflectionFalloffDistance;
}

void BackgroundMaterial::setReflectionFalloffDistance(float value)
{
  _reflectionFalloffDistance = value;
}

float BackgroundMaterial::reflectionAmount() const
{
  return _reflectionAmount;
}

void BackgroundMaterial::setReflectionAmount(float value)
{
  _reflectionAmount = value;
}

float BackgroundMaterial::reflectionReflectance0() const
{
  return _reflectionReflectance0;
}

void BackgroundMaterial::setReflectionReflectance0(float value)
{
  _reflectionReflectance0 = value;
}

float BackgroundMaterial::reflectionReflectance90() const
{
  return _reflectionReflectance90;
}

void BackgroundMaterial::setReflectionReflectance90(float value)
{
  _reflectionReflectance90 = value;
}

bool BackgroundMaterial::useRGBColor() const
{
  return _useRGBColor;
}

void BackgroundMaterial::setUseRGBColor(bool value)
{
  _useRGBColor = value;
}

bool BackgroundMaterial::enableNoise() const
{
  return _enableNoise;
}

void BackgroundMaterial::setEnableNoise(bool value)
{
  _enableNoise = value;
}

} // end of namespace BABYLON
