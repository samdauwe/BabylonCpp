#include <babylon/materialslibrary/terrain/terrain_material.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/terrain/terrain_fragment_fx.h>
#include <babylon/materialslibrary/terrain/terrain_vertex_fx.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

TerrainMaterial::TerrainMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , mixTexture{this, &TerrainMaterial::get_mixTexture,
                 &TerrainMaterial::set_mixTexture}
    , diffuseTexture1{this, &TerrainMaterial::get_diffuseTexture1,
                      &TerrainMaterial::set_diffuseTexture1}
    , diffuseTexture2{this, &TerrainMaterial::get_diffuseTexture2,
                      &TerrainMaterial::set_diffuseTexture2}
    , diffuseTexture3{this, &TerrainMaterial::get_diffuseTexture3,
                      &TerrainMaterial::set_diffuseTexture3}
    , bumpTexture1{this, &TerrainMaterial::get_bumpTexture1,
                   &TerrainMaterial::set_bumpTexture1}
    , bumpTexture2{this, &TerrainMaterial::get_bumpTexture2,
                   &TerrainMaterial::set_bumpTexture2}
    , bumpTexture3{this, &TerrainMaterial::get_bumpTexture3,
                   &TerrainMaterial::set_bumpTexture3}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.f, 0.f, 0.f)}
    , specularPower{64.f}
    , disableLighting{this, &TerrainMaterial::get_disableLighting,
                      &TerrainMaterial::set_disableLighting}
    , maxSimultaneousLights{this, &TerrainMaterial::get_maxSimultaneousLights,
                            &TerrainMaterial::set_maxSimultaneousLights}
    , _mixTexture{nullptr}
    , _diffuseTexture1{nullptr}
    , _diffuseTexture2{nullptr}
    , _diffuseTexture3{nullptr}
    , _bumpTexture1{nullptr}
    , _bumpTexture2{nullptr}
    , _bumpTexture3{nullptr}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _renderId{-1}
{
  // Vertex shader
  Effect::ShadersStore()["terrainVertexShader"] = terrainVertexShader;

  // Fragment shader
  Effect::ShadersStore()["terrainPixelShader"] = terrainPixelShader;
}

TerrainMaterial::~TerrainMaterial()
{
}

BaseTexturePtr& TerrainMaterial::get_mixTexture()
{
  return _mixTexture;
}

void TerrainMaterial::set_mixTexture(const BaseTexturePtr& value)
{
  if (_mixTexture != value) {
    _mixTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TerrainMaterial::get_diffuseTexture1()
{
  return _diffuseTexture1;
}

void TerrainMaterial::set_diffuseTexture1(const TexturePtr& value)
{
  if (_diffuseTexture1 != value) {
    _diffuseTexture1 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TerrainMaterial::get_diffuseTexture2()
{
  return _diffuseTexture2;
}

void TerrainMaterial::set_diffuseTexture2(const TexturePtr& value)
{
  if (_diffuseTexture2 != value) {
    _diffuseTexture2 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TerrainMaterial::get_diffuseTexture3()
{
  return _diffuseTexture3;
}

void TerrainMaterial::set_diffuseTexture3(const TexturePtr& value)
{
  if (_diffuseTexture3 != value) {
    _diffuseTexture3 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TerrainMaterial::get_bumpTexture1()
{
  return _bumpTexture1;
}

void TerrainMaterial::set_bumpTexture1(const TexturePtr& value)
{
  if (_bumpTexture1 != value) {
    _bumpTexture1 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TerrainMaterial::get_bumpTexture2()
{
  return _bumpTexture2;
}

void TerrainMaterial::set_bumpTexture2(const TexturePtr& value)
{
  if (_bumpTexture2 != value) {
    _bumpTexture2 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TerrainMaterial::get_bumpTexture3()
{
  return _bumpTexture3;
}

void TerrainMaterial::set_bumpTexture3(const TexturePtr& value)
{
  if (_bumpTexture3 != value) {
    _bumpTexture3 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool TerrainMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void TerrainMaterial::set_disableLighting(bool value)
{
  if (_disableLighting != value) {
    _disableLighting = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

unsigned int TerrainMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void TerrainMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights != value) {
    _maxSimultaneousLights = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool TerrainMaterial::needAlphaBlending() const
{
  return (alpha < 1.f);
}

bool TerrainMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr TerrainMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool TerrainMaterial::isReadyForSubMesh(AbstractMesh* mesh,
                                        BaseSubMesh* subMesh, bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<TerrainMaterialDefines>();
  }

  auto defines
    = *(static_cast<TerrainMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Textures
  if (scene->texturesEnabled()) {
    if (_mixTexture && StandardMaterial::DiffuseTextureEnabled()) {
      if (!_mixTexture->isReady()) {
        return false;
      }
      else {
        defines._needUVs           = true;
        defines.boolDef["DIFFUSE"] = true;
      }
    }
    if ((_bumpTexture1 || _bumpTexture2 || _bumpTexture3)
        && StandardMaterial::BumpTextureEnabled()) {
      defines._needUVs        = true;
      defines._needNormals    = true;
      defines.boolDef["BUMP"] = true;
    }
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(),
                                        fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  // Lights
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

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              maxSimultaneousLights());

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
    const std::string shaderName{"terrain"};
    auto join = defines.toString();

    const std::vector<std::string> uniforms{
      "world",         "view",          "viewProjection", "vEyePosition",
      "vLightsType",   "vDiffuseColor", "vSpecularColor", "vFogInfos",
      "vFogColor",     "pointSize",     "vTextureInfos",  "mBones",
      "vClipPlane",    "vClipPlane2",   "vClipPlane3",    "vClipPlane4",
      "textureMatrix", "diffuse1Infos", "diffuse2Infos",  "diffuse3Infos"};

    const std::vector<std::string> samplers{
      "textureSampler", "diffuse1Sampler", "diffuse2Sampler", "diffuse3Sampler",
      "bump1Sampler",   "bump2Sampler",    "bump3Sampler"};
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

void TerrainMaterial::bindForSubMesh(Matrix* world, Mesh* mesh,
                                     SubMesh* subMesh)
{
  auto scene = getScene();

  auto _defines
    = static_cast<TerrainMaterialDefines*>(subMesh->_materialDefines.get());
  if (!_defines) {
    return;
  }
  auto defines = *_defines;

  auto effect = subMesh->effect();
  if (!effect) {
    return;
  }
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(*world);
  _activeEffect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  if (_mustRebind(scene, effect)) {
    // Textures
    if (_mixTexture) {
      _activeEffect->setTexture("textureSampler", _mixTexture);
      _activeEffect->setFloat2(
        "vTextureInfos", static_cast<float>(_mixTexture->coordinatesIndex),
        _mixTexture->level);
      _activeEffect->setMatrix("textureMatrix",
                               *_mixTexture->getTextureMatrix());

      if (StandardMaterial::DiffuseTextureEnabled()) {
        if (_diffuseTexture1) {
          _activeEffect->setTexture("diffuse1Sampler", _diffuseTexture1);
          _activeEffect->setFloat2("diffuse1Infos", _diffuseTexture1->uScale,
                                   _diffuseTexture1->vScale);
        }
        if (_diffuseTexture2) {
          _activeEffect->setTexture("diffuse2Sampler", _diffuseTexture2);
          _activeEffect->setFloat2("diffuse2Infos", _diffuseTexture2->uScale,
                                   _diffuseTexture2->vScale);
        }
        if (_diffuseTexture3) {
          _activeEffect->setTexture("diffuse3Sampler", _diffuseTexture3);
          _activeEffect->setFloat2("diffuse3Infos", _diffuseTexture3->uScale,
                                   _diffuseTexture3->vScale);
        }
      }

      if (StandardMaterial::BumpTextureEnabled()
          && scene->getEngine()->getCaps().standardDerivatives) {
        if (_bumpTexture1) {
          _activeEffect->setTexture("bump1Sampler", _bumpTexture1);
        }
        if (_bumpTexture2) {
          _activeEffect->setTexture("bump2Sampler", _bumpTexture2);
        }
        if (_bumpTexture3) {
          _activeEffect->setTexture("bump3Sampler", _bumpTexture3);
        }
      }
    }
    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    MaterialHelper::BindEyePosition(effect, scene);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor,
                           alpha * mesh->visibility);

  if (defines["SPECULARTERM"]) {
    _activeEffect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                               maxSimultaneousLights());
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE()) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> TerrainMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_mixTexture && _mixTexture->animations.size() > 0) {
    results.emplace_back(_mixTexture);
  }

  return results;
}

std::vector<BaseTexturePtr> TerrainMaterial::getActiveTextures() const
{
  auto activeTextures = PushMaterial::getActiveTextures();

  if (_mixTexture) {
    activeTextures.emplace_back(_mixTexture);
  }

  if (_diffuseTexture1) {
    activeTextures.emplace_back(_diffuseTexture1);
  }

  if (_diffuseTexture2) {
    activeTextures.emplace_back(_diffuseTexture2);
  }

  if (_diffuseTexture3) {
    activeTextures.emplace_back(_diffuseTexture3);
  }

  if (_bumpTexture1) {
    activeTextures.emplace_back(_bumpTexture1);
  }

  if (_bumpTexture2) {
    activeTextures.emplace_back(_bumpTexture2);
  }

  if (_bumpTexture3) {
    activeTextures.emplace_back(_bumpTexture3);
  }

  return activeTextures;
}

bool TerrainMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  if (_mixTexture == texture) {
    return true;
  }

  if (_diffuseTexture1 == texture) {
    return true;
  }

  if (_diffuseTexture2 == texture) {
    return true;
  }

  if (_diffuseTexture3 == texture) {
    return true;
  }

  if (_bumpTexture1 == texture) {
    return true;
  }

  if (_bumpTexture2 == texture) {
    return true;
  }

  if (_bumpTexture3 == texture) {
    return true;
  }

  return false;
}

void TerrainMaterial::dispose(bool forceDisposeEffect,
                              bool forceDisposeTextures)
{
  if (_mixTexture) {
    _mixTexture->dispose();
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr TerrainMaterial::clone(const std::string& /*name*/,
                                   bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object TerrainMaterial::serialize() const
{
  return Json::object();
}

const std::string TerrainMaterial::getClassName() const
{
  return "TerrainMaterial";
}

TerrainMaterial* TerrainMaterial::Parse(const Json::value& /*source*/,
                                        Scene* /*scene*/,
                                        const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
