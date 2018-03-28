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

BackgroundMaterial::BackgroundMaterial(const string_t& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , useEquirectangularFOV{false}
    , _primaryColor{Color3::White()}
    , _primaryLevel{1.f}
    , _secondaryColor{Color3::Gray()}
    , _secondaryLevel{1.f}
    , _tertiaryColor{Color3::Black()}
    , _tertiaryLevel{1.f}
    , _reflectionTexture{nullptr}
    , _reflectionBlur{0.f}
    , _diffuseTexture{nullptr}
    , _shadowBlurScale{1}
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
{
  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);

#if 0
  getRenderTargetTextures = [this](vector_t<RenderTargetTexture*>()) {
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
  if (::std::isnan(value)) {
    value = 1.f;
  }
  _fovMultiplier = ::std::max(0.f, ::std::min(2.f, value));
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
    _imageProcessingConfiguration = getScene()->imageProcessingConfiguration();
  }
  else {
    _imageProcessingConfiguration = configuration;
  }

  // Attaches observer.
  _imageProcessingObserver
    = _imageProcessingConfiguration->onUpdateParameters.add(
      [this](ImageProcessingConfiguration* /*conf*/, EventState /*es*/) {
        _markAllSubMeshesAsImageProcessingDirty();
      });
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
  _imageProcessingConfiguration->setColorCurvesEnabled(value);
}

bool BackgroundMaterial::cameraColorGradingEnabled() const
{
  return _imageProcessingConfiguration->colorGradingEnabled();
}

void BackgroundMaterial::setCameraColorGradingEnabled(bool value)
{
  _imageProcessingConfiguration->setColorGradingEnabled(value);
}

bool BackgroundMaterial::cameraToneMappingEnabled() const
{
  return _imageProcessingConfiguration->toneMappingEnabled();
}

void BackgroundMaterial::setCameraToneMappingEnabled(bool value)
{
  _imageProcessingConfiguration->setToneMappingEnabled(value);
}

float BackgroundMaterial::cameraExposure()
{
  return _imageProcessingConfiguration->exposure();
}

void BackgroundMaterial::setCameraExposure(float value)
{
  _imageProcessingConfiguration->setExposure(value);
}

float BackgroundMaterial::cameraContrast() const
{
  return _imageProcessingConfiguration->contrast();
}

void BackgroundMaterial::setCameraContrast(float value)
{
  _imageProcessingConfiguration->setContrast(value);
}

BaseTexture* BackgroundMaterial::cameraColorGradingTexture() const
{
  return _imageProcessingConfiguration->colorGradingTexture;
}

void BackgroundMaterial::setCameraColorGradingTexture(BaseTexture* value)
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
    = ::std::make_unique<ColorCurves>(*value);
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
                                           bool /*useInstances*/)
{
  if (subMesh->effect() && isFrozen()) {
    if (_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = ::std::make_unique<BackgroundMaterialDefines>();
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
                                          _maxSimultaneousLights, false,
                                          BMD::SPECULARTERM, BMD::SHADOWFLOAT);
  defines._needNormals = true;

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (scene->getEngine()->getCaps().textureLOD) {
        defines.defines[BMD::TEXTURELODSUPPORT] = true;
      }

      if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
        if (!_diffuseTexture->isReadyOrNotBlocking()) {
          return false;
        }

        MaterialHelper::PrepareDefinesForMergedUV(_diffuseTexture, defines,
                                                  BMD::DIFFUSE, "DIFFUSE",
                                                  BMD::MAINUV1, BMD::MAINUV2);
        defines.defines[BMD::DIFFUSEHASALPHA] = _diffuseTexture->hasAlpha();
        defines.defines[BMD::GAMMADIFFUSE]    = _diffuseTexture->gammaSpace;
        defines.defines[BMD::OPACITYFRESNEL]  = _opacityFresnel;
      }
      else {
        defines.defines[BMD::DIFFUSE]         = false;
        defines.defines[BMD::DIFFUSEHASALPHA] = false;
        defines.defines[BMD::GAMMADIFFUSE]    = false;
        defines.defines[BMD::OPACITYFRESNEL]  = false;
      }

      auto reflectionTexture = _reflectionTexture;
      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        if (!reflectionTexture->isReadyOrNotBlocking()) {
          return false;
        }

        defines.defines[BMD::REFLECTION]      = true;
        defines.defines[BMD::GAMMAREFLECTION] = reflectionTexture->gammaSpace;
        defines.defines[BMD::REFLECTIONBLUR]  = _reflectionBlur > 0;
        defines.defines[BMD::REFLECTIONMAP_OPPOSITEZ]
          = getScene()->useRightHandedSystem() ? !reflectionTexture->invertZ :
                                                 reflectionTexture->invertZ;
        defines.defines[BMD::LODINREFLECTIONALPHA]
          = reflectionTexture->lodLevelInAlpha;
        defines.defines[BMD::EQUIRECTANGULAR_RELFECTION_FOV]
          = useEquirectangularFOV;
        defines.defines[BMD::REFLECTIONBGR] = switchToBGR;

        if (reflectionTexture->coordinatesMode
            == TextureConstants::INVCUBIC_MODE) {
          defines.defines[BMD::INVERTCUBICMAP] = true;
        }

        defines.defines[BMD::REFLECTIONMAP_3D] = reflectionTexture->isCube;

        switch (reflectionTexture->coordinatesMode) {
          case TextureConstants::CUBIC_MODE:
          case TextureConstants::INVCUBIC_MODE:
            defines.defines[BMD::REFLECTIONMAP_CUBIC] = true;
            break;
          case TextureConstants::EXPLICIT_MODE:
            defines.defines[BMD::REFLECTIONMAP_EXPLICIT] = true;
            break;
          case TextureConstants::PLANAR_MODE:
            defines.defines[BMD::REFLECTIONMAP_PLANAR] = true;
            break;
          case TextureConstants::PROJECTION_MODE:
            defines.defines[BMD::REFLECTIONMAP_PROJECTION] = true;
            break;
          case TextureConstants::SKYBOX_MODE:
            defines.defines[BMD::REFLECTIONMAP_SKYBOX] = true;
            break;
          case TextureConstants::SPHERICAL_MODE:
            defines.defines[BMD::REFLECTIONMAP_SPHERICAL] = true;
            break;
          case TextureConstants::EQUIRECTANGULAR_MODE:
            defines.defines[BMD::REFLECTIONMAP_EQUIRECTANGULAR] = true;
            break;
          case TextureConstants::FIXED_EQUIRECTANGULAR_MODE:
            defines.defines[BMD::REFLECTIONMAP_EQUIRECTANGULAR_FIXED] = true;
            break;
          case TextureConstants::FIXED_EQUIRECTANGULAR_MIRRORED_MODE:
            defines.defines[BMD::REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED]
              = true;
            break;
        }

        if (reflectionFresnel()) {
          defines.defines[BMD::REFLECTIONFRESNEL] = true;
          defines.defines[BMD::REFLECTIONFALLOFF]
            = reflectionFalloffDistance() > 0;

          _reflectionControls.x = reflectionAmount();
          _reflectionControls.y = reflectionReflectance0();
          _reflectionControls.z = reflectionReflectance90();
          _reflectionControls.w = 1.f / reflectionFalloffDistance();
        }
        else {
          defines.defines[BMD::REFLECTIONFRESNEL] = false;
          defines.defines[BMD::REFLECTIONFALLOFF] = false;
        }
      }
      else {
        defines.defines[BMD::REFLECTION]                          = false;
        defines.defines[BMD::REFLECTIONFALLOFF]                   = false;
        defines.defines[BMD::REFLECTIONBLUR]                      = false;
        defines.defines[BMD::REFLECTIONMAP_3D]                    = false;
        defines.defines[BMD::REFLECTIONMAP_SPHERICAL]             = false;
        defines.defines[BMD::REFLECTIONMAP_PLANAR]                = false;
        defines.defines[BMD::REFLECTIONMAP_CUBIC]                 = false;
        defines.defines[BMD::REFLECTIONMAP_PROJECTION]            = false;
        defines.defines[BMD::REFLECTIONMAP_SKYBOX]                = false;
        defines.defines[BMD::REFLECTIONMAP_EXPLICIT]              = false;
        defines.defines[BMD::REFLECTIONMAP_EQUIRECTANGULAR]       = false;
        defines.defines[BMD::REFLECTIONMAP_EQUIRECTANGULAR_FIXED] = false;
        defines.defines[BMD::REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED]
          = false;
        defines.defines[BMD::INVERTCUBICMAP]          = false;
        defines.defines[BMD::REFLECTIONMAP_OPPOSITEZ] = false;
        defines.defines[BMD::LODINREFLECTIONALPHA]    = false;
        defines.defines[BMD::GAMMAREFLECTION]         = false;
      }
    }

    defines.defines[BMD::PREMULTIPLYALPHA]
      = (alphaMode() == EngineConstants::ALPHA_PREMULTIPLIED
         || alphaMode() == EngineConstants::ALPHA_PREMULTIPLIED_PORTERDUFF);
    defines.defines[BMD::USERGBCOLOR] = _useRGBColor;
    defines.defines[BMD::NOISE]       = _enableNoise;
  }

  if (defines._areImageProcessingDirty) {
    if (!_imageProcessingConfiguration->isReady()) {
      return false;
    }

    _imageProcessingConfiguration->prepareDefines(defines);
  }

    // Misc.
#if 0
  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, false, pointsCloud(), fogEnabled(),
    _shouldTurnAlphaTestOn(mesh), defines,
    BMD::LOGARITHMICDEPTH, BMD::POINTSIZE, BMD::FOG, BMD::NONUNIFORMSCALING);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, BMD::CLIPPLANE, BMD::ALPHATEST,
    BMD::DEPTHPREPASS, BMD::INSTANCES);
#endif

  // Attribs
  if (MaterialHelper::PrepareDefinesForAttributes(
        mesh, defines, false, true, false, BMD::NORMAL, BMD::UV1, BMD::UV2,
        BMD::VERTEXCOLOR, BMD::VERTEXALPHA, BMD::MORPHTARGETS_NORMAL,
        BMD::MORPHTARGETS)) {
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
    auto fallbacks = ::std::make_unique<EffectFallbacks>();
    if (defines[BMD::FOG]) {
      fallbacks->addFallback(0, "FOG");
    }

    if (defines[BMD::POINTSIZE]) {
      fallbacks->addFallback(1, "POINTSIZE");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    vector_t<string_t> attribs{VertexBuffer::PositionKindChars};

    if (defines[BMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines[BMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines[BMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  BMD::INSTANCES);

    vector_t<string_t> uniforms{"world",
                                "view",
                                "viewProjection",
                                "vEyePosition",
                                "vLightsType",
                                "vFogInfos",
                                "vFogColor",
                                "pointSize",
                                "vClipPlane",
                                "mBones",
                                "vPrimaryColor",
                                "vSecondaryColor",
                                "vTertiaryColor",
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
    vector_t<string_t> samplers{"diffuseSampler", "reflectionSampler",
                                "reflectionSamplerLow",
                                "reflectionSamplerHigh"};
    vector_t<string_t> uniformBuffers{"Material", "Scene"};

    ImageProcessingConfiguration::PrepareUniforms(uniforms, defines);
    ImageProcessingConfiguration::PrepareSamplers(samplers, defines);

    unordered_map_t<string_t, unsigned int> indexParameters{
      {"maxSimultaneousLights", _maxSimultaneousLights}};

    ::std::function<void(Effect * effect)> onCompiled = [&](Effect* effect) {
      if (onCompiled) {
        onCompiled(effect);
      }

      bindSceneUniformBuffer(effect, scene->getSceneUniformBuffer());
    };

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

void BackgroundMaterial::buildUniformLayout()
{
  // Order is important !
  _uniformBuffer->addUniform("vPrimaryColor", 4);
  _uniformBuffer->addUniform("vSecondaryColor", 4);
  _uniformBuffer->addUniform("vTertiaryColor", 4);
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

void BackgroundMaterial::bindForSubMesh(Matrix* world, Mesh* mesh,
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
  bindOnlyWorldMatrix(*world);

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  auto mustRebind = _mustRebind(scene, effect, mesh->visibility());
  if (mustRebind) {
    _uniformBuffer->bindToEffect(effect, "Material");

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

      _uniformBuffer->updateColor4("vPrimaryColor", _primaryColor,
                                   _primaryLevel, "");
      _uniformBuffer->updateColor4("vSecondaryColor", _secondaryColor,
                                   _secondaryLevel, "");
      _uniformBuffer->updateColor4("vTertiaryColor", _tertiaryColor,
                                   _tertiaryLevel, "");
    }

    _uniformBuffer->updateFloat("fFovMultiplier", _fovMultiplier);

    // Textures
    if (scene->texturesEnabled()) {
      if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
        _uniformBuffer->setTexture("diffuseSampler", _diffuseTexture);
      }

      if (reflectionTexture && StandardMaterial::ReflectionTextureEnabled()) {
        if (defines[BMD::REFLECTIONBLUR] && defines[BMD::TEXTURELODSUPPORT]) {
          _uniformBuffer->setTexture("reflectionSampler", reflectionTexture);
        }
        else if (!defines[BMD::REFLECTIONBLUR]) {
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

        if (defines[BMD::REFLECTIONFRESNEL]) {
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
    bindView(effect);

    // Fog
    MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

    // image processing
    _imageProcessingConfiguration->bind(_activeEffect);
  }

  _uniformBuffer->update();

  Material::_afterBind(mesh);
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

BackgroundMaterial* BackgroundMaterial::clone(const string_t& /*name*/,
                                              bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object BackgroundMaterial::serialize() const
{
  return Json::object();
}

const char* BackgroundMaterial::getClassName() const
{
  return "BackgroundMaterial";
}

unique_ptr_t<BackgroundMaterial>
BackgroundMaterial::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                          const string_t& /*url*/)
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

float BackgroundMaterial::primaryLevel() const
{
  return _primaryLevel;
}

void BackgroundMaterial::setPrimaryLevel(float value)
{
  _primaryLevel = value;
}

const Color3& BackgroundMaterial::secondaryColor() const
{
  return _secondaryColor;
}

void BackgroundMaterial::setSecondaryColor(const Color3& value)
{
  _secondaryColor = value;
}

float BackgroundMaterial::secondaryLevel() const
{
  return _secondaryLevel;
}

void BackgroundMaterial::setSecondaryLevel(float value)
{
  _secondaryLevel = value;
}

const Color3& BackgroundMaterial::tertiaryColor() const
{
  return _tertiaryColor;
}

void BackgroundMaterial::setTertiaryColor(const Color3& value)
{
  _tertiaryColor = value;
}

float BackgroundMaterial::tertiaryLevel() const
{
  return _tertiaryLevel;
}

void BackgroundMaterial::setTertiaryLevel(float value)
{
  _tertiaryLevel = value;
}

BaseTexture* BackgroundMaterial::reflectionTexture() const
{
  return _reflectionTexture;
}

void BackgroundMaterial::setReflectionTexture(RenderTargetTexture* value)
{
  _reflectionTexture = value;
}

BaseTexture* BackgroundMaterial::diffuseTexture() const
{
  return _diffuseTexture;
}

void BackgroundMaterial::setDiffuseTexture(RenderTargetTexture* value)
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
