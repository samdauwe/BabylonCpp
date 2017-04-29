#include <babylon/materialslibrary/fur/fur_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/time.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

unsigned int FurMaterial::maxSimultaneousLights = 4;

FurMaterial::FurMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , diffuseTexture{nullptr}
    , heightTexture{nullptr}
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
    , disableLighting{false}
    , highLevelFur{true}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
    , _furTime{0.f}
    , _cachedDefines{std_util::make_unique<FurMaterialDefines>()}
{
  _cachedDefines->BonesPerMesh         = 0;
  _cachedDefines->NUM_BONE_INFLUENCERS = 0;
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
      offsetFur->furLength      = furLength;
      offsetFur->furAngle       = furAngle;
      offsetFur->furGravity     = furGravity;
      offsetFur->furSpacing     = furSpacing;
      offsetFur->furSpeed       = furSpeed;
      offsetFur->furColor       = furColor;
      offsetFur->diffuseTexture = diffuseTexture;
      offsetFur->furTexture     = furTexture;
      offsetFur->highLevelFur   = highLevelFur;
      offsetFur->setFurTime(furTime());
      offsetFur->furDensity = furDensity;
    }
  }
}

bool FurMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                              bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[FMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool FurMaterial::isReady(AbstractMesh* mesh, bool useInstances)
{
  if (checkReadyOnlyOnce) {
    if (_wasPreviouslyReady) {
      return true;
    }
  }

  auto scene = getScene();

  if (!checkReadyOnEveryCall) {
    if (_renderId == scene->getRenderId()) {
      if (_checkCache(scene, mesh, useInstances)) {
        return true;
      }
    }
  }

  auto engine      = scene->getEngine();
  bool needNormals = false;
  bool needUVs     = false;

  _defines.reset();

  // Textures
  if (scene->texturesEnabled()) {
    if (diffuseTexture && StandardMaterial::DiffuseTextureEnabled) {
      if (!diffuseTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[FMD::DIFFUSE] = true;
      }
    }
    if (heightTexture) {
      if (!heightTexture->isReady()) {
        return false;
      }
      else {
        needUVs                          = true;
        _defines.defines[FMD::HEIGHTMAP] = true;
      }
    }
  }

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[FMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[FMD::ALPHATEST] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[FMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[FMD::FOG] = true;
  }

  // High level
  if (highLevelFur) {
    _defines.defines[FMD::HIGHLEVEL] = true;
  }

  // Lights
  if (scene->lightsEnabled() && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights, FMD::SPECULARTERM,
      FMD::SHADOWS);
  }

  // Attribs
  if (mesh) {
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[FMD::NORMAL] = true;
    }
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[FMD::UV1] = true;
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        _defines.defines[FMD::UV2] = true;
      }
    }
    if (mesh->useVertexColors()
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[FMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha()) {
        _defines.defines[FMD::VERTEXALPHA] = true;
      }
    }

    if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers();
      _defines.BonesPerMesh         = mesh->skeleton()->bones.size() + 1;
    }

    // Instances
    if (useInstances) {
      _defines.defines[FMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines[FMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get(),
                                              maxSimultaneousLights);

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (_defines[FMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (_defines[FMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (_defines[FMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (_defines[FMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  FMD::INSTANCES);

    // Legacy browser patch
    std::string shaderName = "fur";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{
      "world",         "view",          "viewProjection", "vEyePosition",
      "vLightsType",   "vDiffuseColor", "vFogInfos",      "vFogColor",
      "pointSize",     "vDiffuseInfos", "mBones",         "vClipPlane",
      "diffuseMatrix", "furLength",     "furAngle",       "furColor",
      "furOffset",     "furGravity",    "furTime",        "furSpacing",
      "furDensity"};
    std::vector<std::string> samplers{"diffuseSampler", "heightTexture",
                                      "furTexture"};

    MaterialHelper::PrepareUniformsAndSamplersList(uniforms, samplers, _defines,
                                                   maxSimultaneousLights);

    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", maxSimultaneousLights}};

    _effect = engine->createEffect(shaderName, attribs, uniforms, samplers,
                                   join, fallbacks.get(), onCompiled, onError,
                                   indexParameters);
  }
  if (!_effect->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  if (mesh) {
    if (!mesh->_materialDefines) {
      mesh->_materialDefines = std_util::make_unique<FurMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void FurMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void FurMaterial::bind(Matrix* world, Mesh* mesh)
{
  auto scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);
  _effect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (scene->getCachedMaterial() != this) {
    // Textures
    if (diffuseTexture && StandardMaterial::DiffuseTextureEnabled) {
      _effect->setTexture("diffuseSampler", diffuseTexture);

      _effect->setFloat2("vDiffuseInfos",
                         static_cast<float>(diffuseTexture->coordinatesIndex),
                         diffuseTexture->level);
      _effect->setMatrix("diffuseMatrix", *diffuseTexture->getTextureMatrix());
    }

    if (heightTexture) {
      _effect->setTexture("heightTexture", heightTexture);
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_effect, scene);

    // Point size
    if (pointsCloud()) {
      _effect->setFloat("pointSize", pointSize);
    }

    _effect->setVector3("vEyePosition", scene->_mirroredCameraPosition ?
                                          *scene->_mirroredCameraPosition :
                                          scene->activeCamera->position);
  }

  _effect->setColor4("vDiffuseColor", diffuseColor, alpha * mesh->visibility);

  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _effect,
                               _defines.defines[FMD::SPECULARTERM],
                               maxSimultaneousLights);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  _effect->setFloat("furLength", furLength);
  _effect->setFloat("furAngle", furAngle);
  _effect->setColor4("furColor", furColor, 1.f);

  if (highLevelFur) {
    _effect->setVector3("furGravity", furGravity);
    _effect->setFloat("furOffset", furOffset);
    _effect->setFloat("furSpacing", furSpacing);
    _effect->setFloat("furDensity", furDensity);

    _furTime += Time::fpMillisecondsDuration<float>(
                  getScene()->getEngine()->getDeltaTime())
                / furSpeed;
    _effect->setFloat("furTime", _furTime);

    _effect->setTexture("furTexture", furTexture);
  }

  Material::bind(world, mesh);
}

std::vector<IAnimatable*> FurMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (diffuseTexture && diffuseTexture->animations.size() > 0) {
    results.emplace_back(diffuseTexture);
  }

  if (heightTexture && heightTexture->animations.size() > 0) {
    results.emplace_back(heightTexture);
  }

  return results;
}

void FurMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (diffuseTexture) {
    diffuseTexture->dispose();
  }

  if (!_meshes.empty()) {
    for (auto& mesh : _meshes) {
      mesh->material()->dispose(forceDisposeEffect);
      mesh->dispose();
    }
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
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
