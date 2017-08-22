#include <babylon/materialslibrary/fur/fur_material.h>

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
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

FurMaterial::FurMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , furLength{1.f}
    , furAngle{0.f}
    , furColor{Color3(0.44f, 0.21f, 0.02f)}
    , furOffset{0.f}
    , furSpacing{12.f}
    , furGravity{Vector3(0.f, 0.f, 0.f)}
    , furSpeed{100.f}
    , furDensity{20.f}
    , furTexture{nullptr}
    , highLevelFur{true}
    , _diffuseTexture{nullptr}
    , _heightTexture{nullptr}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
    , _furTime{0.f}
{
}

FurMaterial::~FurMaterial()
{
}

float FurMaterial::furTime() const
{
  return _furTime;
}

void FurMaterial::setFurTime(float newFurTime)
{
  _furTime = newFurTime;
}

bool FurMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool FurMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* FurMaterial::getAlphaTestTexture()
{
  return nullptr;
}

void FurMaterial::updateFur()
{
  for (unsigned int i = 1; i < _meshes.size(); ++i) {
    if (auto offsetFur = static_cast<FurMaterial*>(_meshes[i]->material())) {
      offsetFur->furLength       = furLength;
      offsetFur->furAngle        = furAngle;
      offsetFur->furGravity      = furGravity;
      offsetFur->furSpacing      = furSpacing;
      offsetFur->furSpeed        = furSpeed;
      offsetFur->furColor        = furColor;
      offsetFur->_diffuseTexture = _diffuseTexture;
      offsetFur->furTexture      = furTexture;
      offsetFur->highLevelFur    = highLevelFur;
      offsetFur->setFurTime(furTime());
      offsetFur->furDensity = furDensity;
    }
  }
}

bool FurMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                    bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<FurMaterialDefines>();
  }

  auto defines
    = *(static_cast<FurMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Textures
  if (defines._areTexturesDirty) {
    if (scene->texturesEnabled()) {
      if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
        if (!_diffuseTexture->isReady()) {
          return false;
        }
        else {
          defines._needUVs              = true;
          defines.defines[FMD::DIFFUSE] = true;
        }
      }
      if (_heightTexture && engine->getCaps().maxVertexTextureImageUnits) {
        if (!_heightTexture->isReady()) {
          return false;
        }
        else {
          defines._needUVs                = true;
          defines.defines[FMD::HEIGHTMAP] = true;
        }
      }
    }
  }

  // High level
  if (highLevelFur != defines[FMD::HIGHLEVEL]) {
    defines.defines[FMD::HIGHLEVEL] = true;
    defines.markAsUnprocessed();
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, false, pointsCloud(), fogEnabled(), defines,
    FMD::LOGARITHMICDEPTH, FMD::POINTSIZE, FMD::FOG);

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, _disableLighting,
    FMD::SPECULARTERM, FMD::SHADOWFULLFLOAT);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, FMD::CLIPPLANE, FMD::ALPHATEST,
    FMD::INSTANCES);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, true, true, false, FMD::NORMAL, FMD::UV1, FMD::UV2,
    FMD::VERTEXCOLOR, FMD::VERTEXALPHA);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines[FMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (defines[FMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines[FMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines[FMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (defines[FMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  FMD::INSTANCES);

    // Legacy browser patch
    const std::string shaderName{"fur"};
    auto join = defines.toString();
    const std::vector<std::string> uniforms{
      "world",         "view",          "viewProjection", "vEyePosition",
      "vLightsType",   "vDiffuseColor", "vFogInfos",      "vFogColor",
      "pointSize",     "vDiffuseInfos", "mBones",         "vClipPlane",
      "diffuseMatrix", "furLength",     "furAngle",       "furColor",
      "furOffset",     "furGravity",    "furTime",        "furSpacing",
      "furDensity"};
    const std::vector<std::string> samplers{"diffuseSampler", "heightTexture",
                                            "furTexture"};
    const std::vector<std::string> uniformBuffers{};

    EffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.uniformBuffersNames   = std::move(uniformBuffers);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.maxSimultaneousLights = _maxSimultaneousLights;
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.indexParameters
      = {{"maxSimultaneousLights", _maxSimultaneousLights}};

    MaterialHelper::PrepareUniformsAndSamplersList(options);
    subMesh->setEffect(
      scene->getEngine()->createEffect(shaderName, options, engine), defines);
  }

  if (!subMesh->effect()->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void FurMaterial::bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines
    = static_cast<FurMaterialDefines*>(subMesh->_materialDefines.get());
  if (!defines) {
    return;
  }

  auto effect   = subMesh->effect();
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(*world);
  _activeEffect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  if (scene->getCachedMaterial() != this) {
    // Textures
    if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
      _activeEffect->setTexture("diffuseSampler", _diffuseTexture);

      _activeEffect->setFloat2("vDiffuseInfos",
                               _diffuseTexture->coordinatesIndex,
                               _diffuseTexture->level);
      _activeEffect->setMatrix("diffuseMatrix",
                               *_diffuseTexture->getTextureMatrix());
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

    _activeEffect->setVector3("vEyePosition",
                              scene->_mirroredCameraPosition ?
                                *scene->_mirroredCameraPosition :
                                scene->activeCamera->position);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor,
                           alpha * mesh->visibility);

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, *defines,
                               _maxSimultaneousLights, FMD::SPECULARTERM);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
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

    _furTime += getScene()->getEngine()->getDeltaTime() / furSpeed;
    _activeEffect->setFloat("furTime", _furTime);

    _activeEffect->setTexture("furTexture", furTexture);
  }

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatable*> FurMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (_diffuseTexture && _diffuseTexture->animations.size() > 0) {
    results.emplace_back(_diffuseTexture);
  }

  if (_heightTexture && _heightTexture->animations.size() > 0) {
    results.emplace_back(_heightTexture);
  }

  return results;
}

std::vector<BaseTexture*> FurMaterial::getActiveTextures() const
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

bool FurMaterial::hasTexture(BaseTexture* texture) const
{
  if (Material::hasTexture(texture)) {
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

void FurMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
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

Material* FurMaterial::clone(const std::string& /*name*/,
                             bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object FurMaterial::serialize() const
{
  return Json::object();
}

FurMaterial* FurMaterial::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

DynamicTexture* FurMaterial::GenerateTexture(const std::string& /*name*/,
                                             Scene* /*scene*/)
{
  return nullptr;
}

std::vector<Mesh*> FurMaterial::FurifyMesh(Mesh* /*sourceMesh*/,
                                           unsigned int /*quality*/)
{
  return std::vector<Mesh*>();
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
