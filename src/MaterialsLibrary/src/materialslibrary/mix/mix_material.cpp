#include <babylon/materialslibrary/mix/mix_material.h>

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
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/mix/mix_fragment_fx.h>
#include <babylon/materialslibrary/mix/mix_material_defines.h>
#include <babylon/materialslibrary/mix/mix_vertex_fx.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

MixMaterial::MixMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , mixTexture1{this, &MixMaterial::get_mixTexture1,
                  &MixMaterial::set_mixTexture1}
    , mixTexture2{this, &MixMaterial::get_mixTexture2,
                  &MixMaterial::set_mixTexture2}
    , diffuseTexture1{this, &MixMaterial::get_diffuseTexture1,
                      &MixMaterial::set_diffuseTexture1}
    , diffuseTexture2{this, &MixMaterial::get_diffuseTexture2,
                      &MixMaterial::set_diffuseTexture2}
    , diffuseTexture3{this, &MixMaterial::get_diffuseTexture3,
                      &MixMaterial::set_diffuseTexture3}
    , diffuseTexture4{this, &MixMaterial::get_diffuseTexture4,
                      &MixMaterial::set_diffuseTexture4}
    , diffuseTexture5{this, &MixMaterial::get_diffuseTexture5,
                      &MixMaterial::set_diffuseTexture5}
    , diffuseTexture6{this, &MixMaterial::get_diffuseTexture6,
                      &MixMaterial::set_diffuseTexture6}
    , diffuseTexture7{this, &MixMaterial::get_diffuseTexture7,
                      &MixMaterial::set_diffuseTexture7}
    , diffuseTexture8{this, &MixMaterial::get_diffuseTexture8,
                      &MixMaterial::set_diffuseTexture8}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.f, 0.f, 0.f)}
    , specularPower{64.f}
    , disableLighting{this, &MixMaterial::get_disableLighting,
                      &MixMaterial::set_disableLighting}
    , maxSimultaneousLights{this, &MixMaterial::get_maxSimultaneousLights,
                            &MixMaterial::set_maxSimultaneousLights}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _renderId{-1}
{
  // Vertex shader
  Effect::ShadersStore()["mixVertexShader"] = mixVertexShader;

  // Fragment shader
  Effect::ShadersStore()["mixPixelShader"] = mixPixelShader;
}

MixMaterial::~MixMaterial()
{
}

BaseTexturePtr& MixMaterial::get_mixTexture1()
{
  return _mixTexture1;
}

void MixMaterial::set_mixTexture1(const BaseTexturePtr& value)
{
  if (_mixTexture1 != value) {
    _mixTexture1 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

BaseTexturePtr& MixMaterial::get_mixTexture2()
{
  return _mixTexture2;
}

void MixMaterial::set_mixTexture2(const BaseTexturePtr& value)
{
  if (_mixTexture2 != value) {
    _mixTexture2 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& MixMaterial::get_diffuseTexture1()
{
  return _diffuseTexture1;
}

void MixMaterial::set_diffuseTexture1(const TexturePtr& value)
{
  if (_diffuseTexture1 != value) {
    _diffuseTexture1 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& MixMaterial::get_diffuseTexture2()
{
  return _diffuseTexture2;
}

void MixMaterial::set_diffuseTexture2(const TexturePtr& value)
{
  if (_diffuseTexture2 != value) {
    _diffuseTexture2 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& MixMaterial::get_diffuseTexture3()
{
  return _diffuseTexture3;
}

void MixMaterial::set_diffuseTexture3(const TexturePtr& value)
{
  if (_diffuseTexture3 != value) {
    _diffuseTexture3 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& MixMaterial::get_diffuseTexture4()
{
  return _diffuseTexture4;
}

void MixMaterial::set_diffuseTexture4(const TexturePtr& value)
{
  if (_diffuseTexture4 != value) {
    _diffuseTexture4 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& MixMaterial::get_diffuseTexture5()
{
  return _diffuseTexture5;
}

void MixMaterial::set_diffuseTexture5(const TexturePtr& value)
{
  if (_diffuseTexture5 != value) {
    _diffuseTexture5 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& MixMaterial::get_diffuseTexture6()
{
  return _diffuseTexture6;
}

void MixMaterial::set_diffuseTexture6(const TexturePtr& value)
{
  if (_diffuseTexture6 != value) {
    _diffuseTexture6 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& MixMaterial::get_diffuseTexture7()
{
  return _diffuseTexture7;
}

void MixMaterial::set_diffuseTexture7(const TexturePtr& value)
{
  if (_diffuseTexture7 != value) {
    _diffuseTexture7 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& MixMaterial::get_diffuseTexture8()
{
  return _diffuseTexture8;
}

void MixMaterial::set_diffuseTexture8(const TexturePtr& value)
{
  if (_diffuseTexture8 != value) {
    _diffuseTexture8 = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool MixMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void MixMaterial::set_disableLighting(bool value)
{
  if (_disableLighting != value) {
    _disableLighting = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

unsigned int MixMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void MixMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights != value) {
    _maxSimultaneousLights = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool MixMaterial::needAlphaBlending() const
{
  return (alpha < 1.f);
}

bool MixMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr MixMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool MixMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                    bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<MixMaterialDefines>();
  }

  auto defines
    = *(static_cast<MixMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Textures
  if (scene->texturesEnabled) {
    if (StandardMaterial::DiffuseTextureEnabled()) {
      if (_mixTexture1) {
        if (!_mixTexture1->isReady()) {
          return false;
        }
        else {
          defines._needUVs           = true;
          defines.boolDef["DIFFUSE"] = true;
        }
      }
      if (_mixTexture2) {
        if (!_mixTexture2->isReady()) {
          return false;
        }
        else {
          defines.boolDef["MIXMAP2"] = true;
        }
      }
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
    const std::string shaderName{"mix"};
    auto join = defines.toString();

    const std::vector<std::string> uniforms{
      "world",         "view",          "viewProjection", "vEyePosition",
      "vLightsType",   "vDiffuseColor", "vSpecularColor", "vFogInfos",
      "vFogColor",     "pointSize",     "vTextureInfos",  "mBones",
      "vClipPlane",    "vClipPlane2",   "vClipPlane3",    "vClipPlane4",
      "textureMatrix", "diffuse1Infos", "diffuse2Infos",  "diffuse3Infos",
      "diffuse4Infos", "diffuse5Infos", "diffuse6Infos",  "diffuse7Infos",
      "diffuse8Infos"};

    const std::vector<std::string> samplers{
      "mixMap1Sampler",  "mixMap2Sampler",  "diffuse1Sampler",
      "diffuse2Sampler", "diffuse3Sampler", "diffuse4Sampler",
      "diffuse5Sampler", "diffuse6Sampler", "diffuse7Sampler",
      "diffuse8Sampler"};
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

void MixMaterial::bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto _defines
    = static_cast<MixMaterialDefines*>(subMesh->_materialDefines.get());
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
    if (_mixTexture1) {
      _activeEffect->setTexture("mixMap1Sampler", _mixTexture1);
      _activeEffect->setFloat2("vTextureInfos", _mixTexture1->coordinatesIndex,
                               _mixTexture1->level);
      _activeEffect->setMatrix("textureMatrix",
                               *_mixTexture1->getTextureMatrix());

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
        if (_diffuseTexture4) {
          _activeEffect->setTexture("diffuse4Sampler", _diffuseTexture4);
          _activeEffect->setFloat2("diffuse4Infos", _diffuseTexture4->uScale,
                                   _diffuseTexture4->vScale);
        }
      }
    }

    if (_mixTexture2) {
      _activeEffect->setTexture("mixMap2Sampler", _mixTexture2);

      if (StandardMaterial::DiffuseTextureEnabled()) {
        if (_diffuseTexture5) {
          _activeEffect->setTexture("diffuse5Sampler", _diffuseTexture5);
          _activeEffect->setFloat2("diffuse5Infos", _diffuseTexture5->uScale,
                                   _diffuseTexture5->vScale);
        }
        if (_diffuseTexture6) {
          _activeEffect->setTexture("diffuse6Sampler", _diffuseTexture6);
          _activeEffect->setFloat2("diffuse6Infos", _diffuseTexture6->uScale,
                                   _diffuseTexture6->vScale);
        }
        if (_diffuseTexture7) {
          _activeEffect->setTexture("diffuse7Sampler", _diffuseTexture7);
          _activeEffect->setFloat2("diffuse7Infos", _diffuseTexture7->uScale,
                                   _diffuseTexture7->vScale);
        }
        if (_diffuseTexture8) {
          _activeEffect->setTexture("diffuse8Sampler", _diffuseTexture8);
          _activeEffect->setFloat2("diffuse8Infos", _diffuseTexture8->uScale,
                                   _diffuseTexture8->vScale);
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
    MaterialHelper::BindLights(scene, mesh, _activeEffect, defines);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> MixMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_mixTexture1 && _mixTexture1->animations.size() > 0) {
    results.emplace_back(_mixTexture1);
  }

  if (_mixTexture2 && _mixTexture2->animations.size() > 0) {
    results.emplace_back(_mixTexture2);
  }

  return results;
}

std::vector<BaseTexturePtr> MixMaterial::getActiveTextures() const
{
  auto activeTextures = PushMaterial::getActiveTextures();

  // Mix map 1
  if (_mixTexture1) {
    activeTextures.emplace_back(_mixTexture1);
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

  if (_diffuseTexture4) {
    activeTextures.emplace_back(_diffuseTexture4);
  }

  // Mix map 2
  if (_mixTexture2) {
    activeTextures.emplace_back(_mixTexture2);
  }

  if (_diffuseTexture5) {
    activeTextures.emplace_back(_diffuseTexture5);
  }

  if (_diffuseTexture6) {
    activeTextures.emplace_back(_diffuseTexture6);
  }

  if (_diffuseTexture7) {
    activeTextures.emplace_back(_diffuseTexture7);
  }

  if (_diffuseTexture8) {
    activeTextures.emplace_back(_diffuseTexture8);
  }

  return activeTextures;
}

bool MixMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  // Mix map 1
  if (_mixTexture1 == texture) {
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

  if (_diffuseTexture4 == texture) {
    return true;
  }

  // Mix map 2
  if (_mixTexture2 == texture) {
    return true;
  }

  if (_diffuseTexture5 == texture) {
    return true;
  }

  if (_diffuseTexture6 == texture) {
    return true;
  }

  if (_diffuseTexture7 == texture) {
    return true;
  }

  if (_diffuseTexture8 == texture) {
    return true;
  }

  return false;
}

void MixMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (_mixTexture1) {
    _mixTexture1->dispose();
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr MixMaterial::clone(const std::string& /*name*/,
                               bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object MixMaterial::serialize() const
{
  return Json::object();
}

const std::string MixMaterial::getClassName() const
{
  return "MixMaterial";
}

MixMaterial* MixMaterial::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
