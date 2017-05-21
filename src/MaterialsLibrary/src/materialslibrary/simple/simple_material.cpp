#include <babylon/materialslibrary/simple/simple_material.h>

#include <babylon/bones/skeleton.h>
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
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

SimpleMaterial::SimpleMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , _diffuseTexture{nullptr}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
{
}

SimpleMaterial::~SimpleMaterial()
{
}

bool SimpleMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool SimpleMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* SimpleMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool SimpleMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                                       bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<SimpleMaterialDefines>();
  }

  auto defines
    = *(static_cast<SimpleMaterialDefines*>(subMesh->_materialDefines.get()));
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
          defines._needUVs              = true;
          defines.defines[SMD::DIFFUSE] = true;
        }
      }
    }
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, false, pointsCloud(), fogEnabled(), defines,
    SMD::LOGARITHMICDEPTH, SMD::POINTSIZE, SMD::FOG);

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, _disableLighting,
    SMD::SPECULARTERM, SMD::SHADOWFULLFLOAT);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, SMD::CLIPPLANE, SMD::ALPHATEST,
    SMD::INSTANCES);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, true, true, false, SMD::NORMAL, SMD::UV1, SMD::UV2,
    SMD::VERTEXCOLOR, SMD::VERTEXALPHA);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines[SMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (defines[SMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines[SMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines[SMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (defines[SMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  SMD::INSTANCES);

    const std::string shaderName{"simple"};
    auto join = defines.toString();
    const std::vector<std::string> uniforms{
      "world",         "view",          "viewProjection", "vEyePosition",
      "vLightsType",   "vDiffuseColor", "vFogInfos",      "vFogColor",
      "pointSize",     "vDiffuseInfos", "mBones",         "vClipPlane",
      "diffuseMatrix", "depthValues"};
    const std::vector<std::string> samplers{"diffuseSampler"};
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

void SimpleMaterial::bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines
    = static_cast<SimpleMaterialDefines*>(subMesh->_materialDefines.get());
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

  if (_mustRebind(scene, effect)) {
    // Textures
    if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
      _activeEffect->setTexture("diffuseSampler", _diffuseTexture);

      _activeEffect->setFloat2("vDiffuseInfos",
                               _diffuseTexture->coordinatesIndex,
                               _diffuseTexture->level);
      _activeEffect->setMatrix("diffuseMatrix",
                               *_diffuseTexture->getTextureMatrix());
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

  // Lights
  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, *defines,
                               _maxSimultaneousLights, SMD::SPECULARTERM);
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

std::vector<IAnimatable*> SimpleMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (_diffuseTexture && _diffuseTexture->animations.size() > 0) {
    results.emplace_back(_diffuseTexture);
  }

  return results;
}

void SimpleMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (_diffuseTexture) {
    _diffuseTexture->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* SimpleMaterial::clone(const std::string& /*name*/,
                                bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object SimpleMaterial::serialize() const
{
  return Json::object();
}

SimpleMaterial* SimpleMaterial::Parse(const Json::value& /*source*/,
                                      Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
