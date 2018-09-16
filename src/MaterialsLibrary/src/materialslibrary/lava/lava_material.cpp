#include <babylon/materialslibrary/lava/lava_material.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/time.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materialslibrary/lava/lava_fragment_fx.h>
#include <babylon/materialslibrary/lava/lava_vertex_fx.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

LavaMaterial::LavaMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , diffuseTexture{this, &LavaMaterial::get_diffuseTexture,
                     &LavaMaterial::set_diffuseTexture}
    , noiseTexture{nullptr}
    , fogColor{nullopt_t}
    , speed{1.f}
    , movingSpeed{1.f}
    , lowFrequencySpeed{1.f}
    , fogDensity{0.15f}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , disableLighting{this, &LavaMaterial::get_disableLighting,
                      &LavaMaterial::set_disableLighting}
    , unlit{this, &LavaMaterial::get_unlit, &LavaMaterial::set_unlit}
    , maxSimultaneousLights{this, &LavaMaterial::get_maxSimultaneousLights,
                            &LavaMaterial::set_maxSimultaneousLights}
    , _diffuseTexture{nullptr}
    , _lastTime{0.f}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
{
  // Vertex shader
  Effect::ShadersStore()["lavaVertexShader"] = lavaVertexShader;

  // Fragment shader
  Effect::ShadersStore()["lavaPixelShader"] = lavaPixelShader;
}

LavaMaterial::~LavaMaterial()
{
}

BaseTexturePtr& LavaMaterial::get_diffuseTexture()
{
  return _diffuseTexture;
}

void LavaMaterial::set_diffuseTexture(const BaseTexturePtr& value)
{
  if (_diffuseTexture != value) {
    _diffuseTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool LavaMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void LavaMaterial::set_disableLighting(bool value)
{
  if (_disableLighting != value) {
    _disableLighting = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool LavaMaterial::get_unlit() const
{
  return _unlit;
}

void LavaMaterial::set_unlit(bool value)
{
  if (_unlit != value) {
    _unlit = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

unsigned int LavaMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void LavaMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights != value) {
    _maxSimultaneousLights = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool LavaMaterial::needAlphaBlending() const
{
  return (alpha < 1.f);
}

bool LavaMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr LavaMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool LavaMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                     bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<LavaMaterialDefines>();
  }

  auto defines
    = *(static_cast<LavaMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
        if (!_diffuseTexture->isReady()) {
          return false;
        }
        else {
          defines._needUVs           = true;
          defines.boolDef["DIFFUSE"] = true;
        }
      }
    }
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(),
                                        fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  // Lights
  defines._needNormals = true;

  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, _disableLighting);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances ? true : false);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(mesh, defines, true, true);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines["FOG"]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks);

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines["UV2"]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (defines["VERTEXCOLOR"]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    // Legacy browser patch
    const std::string shaderName{"lava"};
    auto join = defines.toString();

    const std::vector<std::string> uniforms{
      "world",          "view",
      "viewProjection", "vEyePosition",
      "vLightsType",    "vDiffuseColor",
      "vFogInfos",      "vFogColor",
      "pointSize",      "vDiffuseInfos",
      "mBones",         "vClipPlane",
      "vClipPlane2",    "vClipPlane3",
      "vClipPlane4",    "diffuseMatrix",
      "time",           "speed",
      "movingSpeed",    "fogColor",
      "fogDensity",     "lowFrequencySpeed"};

    const std::vector<std::string> samplers{"diffuseSampler", "noiseTexture"};
    const std::vector<std::string> uniformBuffers{};

    EffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.uniformBuffersNames   = std::move(uniformBuffers);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.maxSimultaneousLights = maxSimultaneousLights();
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.indexParameters
      = {{"maxSimultaneousLights", maxSimultaneousLights()}};

    MaterialHelper::PrepareUniformsAndSamplersList(options);
    subMesh->setEffect(
      scene->getEngine()->createEffect(shaderName, options, engine), defines);
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void LavaMaterial::bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines
    = static_cast<LavaMaterialDefines*>(subMesh->_materialDefines.get());
  if (!defines) {
    return;
  }

  auto effect = subMesh->effect();

  if (!effect) {
    return;
  }
  _activeEffect = effect;

  defines->boolDef["UNLIT"] = _unlit;

  // Matrices
  bindOnlyWorldMatrix(*world);
  _activeEffect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  if (_mustRebind(scene, effect)) {
    // Textures
    if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
      _activeEffect->setTexture("diffuseSampler", _diffuseTexture);

      _activeEffect->setFloat2(
        "vDiffuseInfos", static_cast<float>(_diffuseTexture->coordinatesIndex),
        _diffuseTexture->level);
      _activeEffect->setMatrix("diffuseMatrix",
                               *_diffuseTexture->getTextureMatrix());
    }

    if (noiseTexture) {
      _activeEffect->setTexture("noiseTexture", noiseTexture);
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    MaterialHelper::BindEyePosition(effect, scene);
  }

  _activeEffect->setColor4("vDiffuseColor", _scaledDiffuse,
                           alpha * mesh->visibility);

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, *defines);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE()) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _lastTime += scene->getEngine()->getDeltaTime();
  _activeEffect->setFloat("time", _lastTime * speed / 1000.f);

  if (!fogColor.has_value()) {
    fogColor = Color3::Black();
  }
  _activeEffect->setColor3("fogColor", *fogColor);
  _activeEffect->setFloat("fogDensity", fogDensity);

  _activeEffect->setFloat("lowFrequencySpeed", lowFrequencySpeed);
  _activeEffect->setFloat("movingSpeed", movingSpeed);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> LavaMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_diffuseTexture && _diffuseTexture->animations.size() > 0) {
    results.emplace_back(_diffuseTexture);
  }

  if (noiseTexture && noiseTexture->animations.size() > 0) {
    results.emplace_back(noiseTexture);
  }

  return results;
}

std::vector<BaseTexturePtr> LavaMaterial::getActiveTextures() const
{
  auto activeTextures = PushMaterial::getActiveTextures();
  if (_diffuseTexture) {
    activeTextures.emplace_back(_diffuseTexture);
  }

  return activeTextures;
}

bool LavaMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  if (_diffuseTexture == texture) {
    return true;
  }

  return false;
}

void LavaMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (_diffuseTexture) {
    _diffuseTexture->dispose();
  }

  if (noiseTexture) {
    noiseTexture->dispose();
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr LavaMaterial::clone(const std::string& /*name*/,
                                bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object LavaMaterial::serialize() const
{
  return Json::object();
}

const string_t LavaMaterial::getClassName() const
{
  return "LavaMaterial";
}

LavaMaterial* LavaMaterial::Parse(const Json::value& /*source*/,
                                  Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
