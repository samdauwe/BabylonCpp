#include <babylon/materialslibrary/terrain/terrain_material.h>

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
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

TerrainMaterial::TerrainMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.f, 0.f, 0.f)}
    , specularPower{64.f}
    , _mixTexture{nullptr}
    , _diffuseTexture1{nullptr}
    , _diffuseTexture2{nullptr}
    , _diffuseTexture3{nullptr}
    , _bumpTexture1{nullptr}
    , _bumpTexture2{nullptr}
    , _bumpTexture3{nullptr}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
{
}

TerrainMaterial::~TerrainMaterial()
{
}

bool TerrainMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool TerrainMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* TerrainMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool TerrainMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                                        bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std_util::make_unique<TerrainMaterialDefines>();
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
        defines._needUVs              = true;
        defines.defines[TMD::DIFFUSE] = true;
      }
    }
    if ((_bumpTexture1 || _bumpTexture2 || _bumpTexture3)
        && StandardMaterial::BumpTextureEnabled()) {
      defines._needUVs           = true;
      defines._needNormals       = true;
      defines.defines[TMD::BUMP] = true;
    }
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, false, pointsCloud(), fogEnabled(), defines,
    TMD::LOGARITHMICDEPTH, TMD::POINTSIZE, TMD::FOG);

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, _disableLighting,
    TMD::SPECULARTERM, TMD::SHADOWFULLFLOAT);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, TMD::CLIPPLANE, TMD::ALPHATEST,
    TMD::INSTANCES);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, true, true, false, TMD::NORMAL, TMD::UV1, TMD::UV2,
    TMD::VERTEXCOLOR, TMD::VERTEXALPHA);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (defines[TMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

    if (defines[TMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines[TMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines[TMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (defines[TMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  TMD::INSTANCES);

    // Legacy browser patch
    const std::string shaderName{"terrain"};
    auto join = defines.toString();

    const std::vector<std::string> uniforms{
      "world",        "view",          "viewProjection", "vEyePosition",
      "vLightsType",  "vDiffuseColor", "vSpecularColor", "vFogInfos",
      "vFogColor",    "pointSize",     "vTextureInfos",  "mBones",
      "vClipPlane",   "textureMatrix", "diffuse1Infos",  "diffuse2Infos",
      "diffuse3Infos"};

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

  auto effect   = subMesh->effect();
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
      _activeEffect->setFloat2("vTextureInfos", _mixTexture->coordinatesIndex,
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

    _activeEffect->setVector3("vEyePosition",
                              scene->_mirroredCameraPosition ?
                                *scene->_mirroredCameraPosition :
                                scene->activeCamera->position);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor,
                           alpha * mesh->visibility);

  if (defines[TMD::SPECULARTERM]) {
    _activeEffect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                               _maxSimultaneousLights, TMD::SPECULARTERM);
  }

  // View
  if (scene->fogEnabled && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatable*> TerrainMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (_mixTexture && _mixTexture->animations.size() > 0) {
    results.emplace_back(_mixTexture);
  }

  return results;
}

void TerrainMaterial::dispose(bool forceDisposeEffect,
                              bool forceDisposeTextures)
{
  if (_mixTexture) {
    _mixTexture->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* TerrainMaterial::clone(const std::string& /*name*/,
                                 bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object TerrainMaterial::serialize() const
{
  return Json::object();
}

TerrainMaterial* TerrainMaterial::Parse(const Json::value& /*source*/,
                                        Scene* /*scene*/,
                                        const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
