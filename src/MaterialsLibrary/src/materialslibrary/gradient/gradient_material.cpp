#include <babylon/materialslibrary/gradient/gradient_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

GradientMaterial::GradientMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , topColor{Color3(1.f, 0.f, 0.f)}
    , topColorAlpha{1.f}
    , bottomColor{Color3(0.f, 0.f, 1.f)}
    , bottomColorAlpha{1.f}
    , offset{0}
    , smoothness{1.f}
    , disableLighting{false}
    , _maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
{
}

GradientMaterial::~GradientMaterial()
{
}

bool GradientMaterial::needAlphaBlending()
{
  return (alpha < 1.f || topColorAlpha < 1.f || bottomColorAlpha < 1.f);
}

bool GradientMaterial::needAlphaTesting()
{
  return true;
}

BaseTexture* GradientMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool GradientMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                                         bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines
      = std_util::make_unique<GradientMaterialDefines>();
  }

  auto defines
    = *(static_cast<GradientMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, GMD::CLIPPLANE, GMD::ALPHATEST,
    GMD::INSTANCES);

  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, false, pointsCloud(), fogEnabled(), defines,
    GMD::LOGARITHMICDEPTH, GMD::POINTSIZE, GMD::FOG);

  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, false,
    GMD::SPECULARTERM, GMD::SHADOWFULLFLOAT);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, false, true, false, GMD::NORMAL, GMD::UV1, GMD::UV2,
    GMD::VERTEXCOLOR, GMD::VERTEXALPHA);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (defines[GMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (defines[GMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines[GMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines[GMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (defines[GMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  GMD::INSTANCES);

    // Legacy browser patch
    const std::string shaderName{"gradient"};
    auto join = defines.toString();

    std::vector<std::string> uniforms{
      "world",         "view",          "viewProjection", "vEyePosition",
      "vLightsType",   "vDiffuseColor", "vFogInfos",      "vFogColor",
      "pointSize",     "vDiffuseInfos", "mBones",         "vClipPlane",
      "diffuseMatrix", "depthValues",   "topColor",       "bottomColor",
      "offset",        "smoothness"};
    std::vector<std::string> samplers{"diffuseSampler"};
    std::vector<std::string> uniformBuffers{};

    EffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.uniformBuffersNames   = std::move(uniformBuffers);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.maxSimultaneousLights = 4;
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.indexParameters       = {{"maxSimultaneousLights", 4}};

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

void GradientMaterial::bindForSubMesh(Matrix* world, Mesh* mesh,
                                      SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines
    = static_cast<GradientMaterialDefines*>(subMesh->_materialDefines.get());
  if (!defines) {
    return;
  }

  auto effect   = subMesh->effect();
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(*world);
  _activeEffect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (_mustRebind(scene, effect)) {
    // Clip plane
    MaterialHelper::BindClipPlane(_effect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    _activeEffect->setVector3("vEyePosition",
                              scene->_mirroredCameraPosition ?
                                *scene->_mirroredCameraPosition :
                                scene->activeCamera.position);
  }

  _activeEffect->setColor4("vDiffuseColor", _scaledDiffuse,
                           alpha * mesh->visibility);

  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                               _maxSimultaneousLights, GMD::SPECULARTERM);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _activeEffect->setColor4("topColor", topColor, topColorAlpha);
  _activeEffect->setColor4("bottomColor", bottomColor, bottomColorAlpha);
  _activeEffect->setFloat("offset", offset);
  _activeEffect->setFloat("smoothness", smoothness);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatable*> GradientMaterial::getAnimatables()
{
  return std::vector<IAnimatable*>();
}

void GradientMaterial::dispose(bool forceDisposeEffect,
                               bool forceDisposeTextures)
{
  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* GradientMaterial::clone(const std::string& /*name*/,
                                  bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object GradientMaterial::serialize() const
{
  return Json::object();
}

GradientMaterial* GradientMaterial::Parse(const Json::value& /*source*/,
                                          Scene* /*scene*/,
                                          const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
