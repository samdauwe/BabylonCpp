#include <babylon/materialslibrary/fur/fur_material.h>

#include <nlohmann/json.hpp>

#include <babylon/cameras/camera.h>
#include <babylon/core/time.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/materialslibrary/fur/fur_fragment_fx.h>
#include <babylon/materialslibrary/fur/fur_vertex_fx.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

FurMaterial::FurMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , diffuseTexture{this, &FurMaterial::get_diffuseTexture, &FurMaterial::set_diffuseTexture}
    , heightTexture{this, &FurMaterial::get_heightTexture, &FurMaterial::set_heightTexture}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , furLength{1.f}
    , furAngle{0.f}
    , furColor{Color3(0.44f, 0.21f, 0.02f)}
    , furOffset{0.f}
    , furSpacing{12.f}
    , furGravity{Vector3(0.f, 0.f, 0.f)}
    , furSpeed{100.f}
    , furDensity{20.f}
    , furOcclusion{0.f}
    , furTexture{nullptr}
    , disableLighting{this, &FurMaterial::get_disableLighting, &FurMaterial::set_disableLighting}
    , maxSimultaneousLights{this, &FurMaterial::get_maxSimultaneousLights,
                            &FurMaterial::set_maxSimultaneousLights}
    , highLevelFur{true}
    , furTime{this, &FurMaterial::get_furTime, &FurMaterial::set_furTime}
    , _diffuseTexture{nullptr}
    , _heightTexture{nullptr}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _furTime{0.f}
{
  // Vertex shader
  Effect::ShadersStore()["furVertexShader"] = furVertexShader;

  // Fragment shader
  Effect::ShadersStore()["furPixelShader"] = furPixelShader;
}

FurMaterial::~FurMaterial() = default;

BaseTexturePtr& FurMaterial::get_diffuseTexture()
{
  return _diffuseTexture;
}

void FurMaterial::set_diffuseTexture(const BaseTexturePtr& value)
{
  if (_diffuseTexture != value) {
    _diffuseTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

BaseTexturePtr& FurMaterial::get_heightTexture()
{
  return _heightTexture;
}

void FurMaterial::set_heightTexture(const BaseTexturePtr& value)
{
  if (_heightTexture != value) {
    _heightTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool FurMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void FurMaterial::set_disableLighting(bool value)
{
  if (_disableLighting != value) {
    _disableLighting = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

unsigned int FurMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void FurMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights != value) {
    _maxSimultaneousLights = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

float FurMaterial::get_furTime() const
{
  return _furTime;
}

void FurMaterial::set_furTime(float newFurTime)
{
  _furTime = newFurTime;
}

bool FurMaterial::needAlphaBlending() const
{
  return (alpha < 1.f);
}

bool FurMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr FurMaterial::getAlphaTestTexture()
{
  return nullptr;
}

void FurMaterial::updateFur()
{
  for (const auto& mesh : _meshes) {
    if (auto offsetFur = std::dynamic_pointer_cast<FurMaterial>(mesh->material())) {
      offsetFur->furLength      = furLength;
      offsetFur->furAngle       = furAngle;
      offsetFur->furGravity     = furGravity;
      offsetFur->furSpacing     = furSpacing;
      offsetFur->furSpeed       = furSpeed;
      offsetFur->furColor       = furColor;
      offsetFur->diffuseTexture = diffuseTexture();
      offsetFur->furTexture     = furTexture;
      offsetFur->highLevelFur   = highLevelFur;
      offsetFur->furTime        = furTime();
      offsetFur->furDensity     = furDensity;
    }
  }
}

bool FurMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh, bool useInstances)
{
  if (isFrozen()) {
    if (subMesh->effect() && subMesh->effect()->_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<FurMaterialDefines>();
  }

  auto definesPtr = std::static_pointer_cast<FurMaterialDefines>(subMesh->_materialDefines);
  auto& defines   = *definesPtr.get();
  auto scene      = getScene();

  if (_isReadyForSubMesh(subMesh)) {
    return true;
  }

  auto engine = scene->getEngine();

  // Textures
  if (defines._areTexturesDirty) {
    if (scene->texturesEnabled()) {
      if (_diffuseTexture && MaterialFlags::DiffuseTextureEnabled()) {
        if (!_diffuseTexture->isReady()) {
          return false;
        }
        else {
          defines._needUVs           = true;
          defines.boolDef["DIFFUSE"] = true;
        }
      }
      if (_heightTexture && engine->getCaps().maxVertexTextureImageUnits) {
        if (!_heightTexture->isReady()) {
          return false;
        }
        else {
          defines._needUVs             = true;
          defines.boolDef["HEIGHTMAP"] = true;
        }
      }
    }
  }

  // High level
  if (highLevelFur != defines["HIGHLEVEL"]) {
    defines.boolDef["HIGHLEVEL"] = true;
    defines.markAsUnprocessed();
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(), fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, maxSimultaneousLights(), _disableLighting);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(scene, engine, defines, useInstances);

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

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks, maxSimultaneousLights());

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKind};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKind);
    }

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKind);
    }

    if (defines["UV2"]) {
      attribs.emplace_back(VertexBuffer::UV2Kind);
    }

    if (defines["VERTEXCOLOR"]) {
      attribs.emplace_back(VertexBuffer::ColorKind);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines, *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    // Legacy browser patch
    const std::string shaderName{"fur"};
    auto join = defines.toString();
    const std::vector<std::string> uniforms{
      "world",         "view",        "viewProjection", "vEyePosition", "vLightsType",
      "vDiffuseColor", "vFogInfos",   "vFogColor",      "pointSize",    "vDiffuseInfos",
      "mBones",        "vClipPlane",  "vClipPlane2",    "vClipPlane3",  "vClipPlane4",
      "vClipPlane5",   "vClipPlane6", "diffuseMatrix",  "furLength",    "furAngle",
      "furColor",      "furOffset",   "furGravity",     "furTime",      "furSpacing",
      "furDensity",    "furOcclusion"};
    const std::vector<std::string> samplers{"diffuseSampler", "heightTexture", "furTexture"};
    const std::vector<std::string> uniformBuffers{};

    IEffectCreationOptions options;
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
    options.indexParameters       = {{"maxSimultaneousLights", maxSimultaneousLights()}};

    MaterialHelper::PrepareUniformsAndSamplersList(options);
    subMesh->setEffect(scene->getEngine()->createEffect(shaderName, options, engine), definesPtr);
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  defines._renderId                      = scene->getRenderId();
  subMesh->effect()->_wasPreviouslyReady = true;

  return true;
}

void FurMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines = static_cast<FurMaterialDefines*>(subMesh->_materialDefines.get());
  if (!defines) {
    return;
  }

  auto effect = subMesh->effect();
  if (!effect) {
    return;
  }
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(world);
  _activeEffect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  if (scene->getCachedMaterial() != this) {
    // Textures
    if (_diffuseTexture && MaterialFlags::DiffuseTextureEnabled()) {
      _activeEffect->setTexture("diffuseSampler", _diffuseTexture);

      _activeEffect->setFloat2("vDiffuseInfos",
                               static_cast<float>(_diffuseTexture->coordinatesIndex),
                               _diffuseTexture->level);
      _activeEffect->setMatrix("diffuseMatrix", *_diffuseTexture->getTextureMatrix());
    }

    if (_heightTexture) {
      _activeEffect->setTexture("heightTexture", _heightTexture);
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    MaterialHelper::BindEyePosition(effect, scene);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor, alpha * mesh->visibility);

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, *defines, maxSimultaneousLights());
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog() && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _activeEffect->setFloat("furLength", furLength);
  _activeEffect->setFloat("furAngle", furAngle);
  _activeEffect->setColor4("furColor", furColor, 1.f);

  if (highLevelFur) {
    _activeEffect->setVector3("furGravity", furGravity);
    _activeEffect->setFloat("furOffset", furOffset);
    _activeEffect->setFloat("furSpacing", furSpacing);
    _activeEffect->setFloat("furDensity", furDensity);
    _activeEffect->setFloat("furOcclusion", furOcclusion);

    _furTime += getScene()->getEngine()->getDeltaTime() / furSpeed;
    _activeEffect->setFloat("furTime", _furTime);

    _activeEffect->setTexture("furTexture", furTexture);
  }

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> FurMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_diffuseTexture && _diffuseTexture->animations.size() > 0) {
    results.emplace_back(_diffuseTexture);
  }

  if (_heightTexture && _heightTexture->animations.size() > 0) {
    results.emplace_back(_heightTexture);
  }

  return results;
}

std::vector<BaseTexturePtr> FurMaterial::getActiveTextures() const
{
  auto activeTextures = Material::getActiveTextures();

  if (_diffuseTexture) {
    activeTextures.emplace_back(_diffuseTexture);
  }

  if (_heightTexture) {
    activeTextures.emplace_back(_heightTexture);
  }

  return activeTextures;
}

bool FurMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  if (_diffuseTexture == texture) {
    return true;
  }

  if (_heightTexture == texture) {
    return true;
  }

  return false;
}

void FurMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                          bool /*notBoundToMesh*/)
{
  if (_diffuseTexture) {
    _diffuseTexture->dispose();
  }

  if (!_meshes.empty()) {
    for (auto& mesh : _meshes) {
      mesh->material()->dispose(forceDisposeEffect);
      mesh->dispose();
    }
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr FurMaterial::clone(const std::string& /*name*/, bool /*cloneChildren*/) const
{
  return nullptr;
}

json FurMaterial::serialize() const
{
  return nullptr;
}

std::string FurMaterial::getClassName() const
{
  return "FurMaterial";
}

FurMaterial* FurMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

DynamicTexturePtr FurMaterial::GenerateTexture(const std::string& /*name*/, Scene* /*scene*/)
{
  return nullptr;
}

std::vector<Mesh*> FurMaterial::FurifyMesh(const MeshPtr& /*sourceMesh*/, float /*quality*/)
{
  return std::vector<Mesh*>();
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
