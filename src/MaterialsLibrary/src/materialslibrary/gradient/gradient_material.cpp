#include <babylon/materialslibrary/gradient/gradient_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

unsigned int GradientMaterial::maxSimultaneousLights = 4;

GradientMaterial::GradientMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , topColor{Color3(1.f, 0.f, 0.f)}
    , topColorAlpha{1.f}
    , bottomColor{Color3(0.f, 0.f, 1.f)}
    , bottomColorAlpha{1.f}
    , offset{0}
    , smoothness{1.f}
    , disableLighting{false}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
    , _cachedDefines{std_util::make_unique<GradientMaterialDefines>()}
{
  _cachedDefines->BonesPerMesh         = 0;
  _cachedDefines->NUM_BONE_INFLUENCERS = 0;
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

bool GradientMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                                   bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[GMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool GradientMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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
  auto needNormals = false;
  auto needUVs     = false;

  _defines.reset();

  // No textures

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[GMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[GMD::ALPHATEST] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[GMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[GMD::FOG] = true;
  }

  if (scene->lightsEnabled() && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights, GMD::SPECULARTERM,
      GMD::SHADOWS);
  }

  // Attribs
  if (mesh) {
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[GMD::NORMAL] = true;
    }
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[GMD::UV1] = true;
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        _defines.defines[GMD::UV2] = true;
      }
    }
    if (mesh->useVertexColors()
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[GMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha()) {
        _defines.defines[GMD::VERTEXALPHA] = true;
      }
    }
    if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers();
      _defines.BonesPerMesh         = mesh->skeleton()->bones.size() + 1;
    }

    // Instances
    if (useInstances) {
      _defines.defines[GMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines[GMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get());

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (_defines[GMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (_defines[GMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (_defines[GMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (_defines[GMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  GMD::INSTANCES);

    // Legacy browser patch
    std::string shaderName = "gradient";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{"world",           "view",
                                      "viewProjection",  "vEyePosition",
                                      "vLightsType",     "vDiffuseColor",
                                      "vLightData0",     "vLightDiffuse0",
                                      "vLightSpecular0", "vLightDirection0",
                                      "vLightGround0",   "lightMatrix0",
                                      "vLightData1",     "vLightDiffuse1",
                                      "vLightSpecular1", "vLightDirection1",
                                      "vLightGround1",   "lightMatrix1",
                                      "vLightData2",     "vLightDiffuse2",
                                      "vLightSpecular2", "vLightDirection2",
                                      "vLightGround2",   "lightMatrix2",
                                      "vLightData3",     "vLightDiffuse3",
                                      "vLightSpecular3", "vLightDirection3",
                                      "vLightGround3",   "lightMatrix3",
                                      "vFogInfos",       "vFogColor",
                                      "pointSize",       "vDiffuseInfos",
                                      "mBones",          "vClipPlane",
                                      "diffuseMatrix",   "shadowsInfo0",
                                      "shadowsInfo1",    "shadowsInfo2",
                                      "shadowsInfo3",    "depthValues",
                                      "topColor",        "bottomColor",
                                      "offset",          "smoothness"};
    std::vector<std::string> samplers{"diffuseSampler", "shadowSampler0",
                                      "shadowSampler1", "shadowSampler2",
                                      "shadowSampler3"};
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
      mesh->_materialDefines = std_util::make_unique<GradientMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void GradientMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void GradientMaterial::bind(Matrix* world, Mesh* mesh)
{
  auto scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);
  _effect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (scene->getCachedMaterial() != this) {
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

  _effect->setColor4("vDiffuseColor", _scaledDiffuse, alpha * mesh->visibility);

  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _effect,
                               _defines.defines[GMD::SPECULARTERM]);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  _effect->setColor4("topColor", topColor, topColorAlpha);
  _effect->setColor4("bottomColor", bottomColor, bottomColorAlpha);
  _effect->setFloat("offset", offset);
  _effect->setFloat("smoothness", smoothness);

  Material::bind(world, mesh);
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
