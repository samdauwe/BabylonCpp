#include <babylon/materialslibrary/shadowonly/shadow_only_material.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

ShadowOnlyMaterial::ShadowOnlyMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
{
}

ShadowOnlyMaterial::~ShadowOnlyMaterial()
{
}

bool ShadowOnlyMaterial::needAlphaBlending()
{
  return true;
}

bool ShadowOnlyMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* ShadowOnlyMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool ShadowOnlyMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                                           bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<ShadowOnlyMaterialDefines>();
  }

  auto defines = *(
    static_cast<ShadowOnlyMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, SMD::CLIPPLANE, SMD::ALPHATEST,
    SMD::INSTANCES);

  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, false, pointsCloud(), fogEnabled(), defines,
    SMD::LOGARITHMICDEPTH, SMD::POINTSIZE, SMD::FOG);

  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, 1, false, SMD::SPECULARTERM,
    SMD::SHADOWFULLFLOAT);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, false, true, false, SMD::NORMAL, SMD::UV1, SMD::UV2,
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

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks, 1);

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (defines[SMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  SMD::INSTANCES);

    const std::string shaderName{"shadowOnly"};
    auto join = defines.toString();
    EffectCreationOptions options;
    options.attributes = std::move(attribs);
    options.uniformsNames
      = {"world",       "view",       "viewProjection", "vEyePosition",
         "vLightsType", "vFogInfos",  "vFogColor",      "pointSize",
         "mBones",      "vClipPlane", "depthValues"};
    options.uniformBuffersNames   = {};
    options.samplers              = {};
    options.defines               = std::move(join);
    options.maxSimultaneousLights = 1;
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.indexParameters       = {{"maxSimultaneousLights", 1}};

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

void ShadowOnlyMaterial::bindForSubMesh(Matrix* world, Mesh* mesh,
                                        SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines
    = static_cast<ShadowOnlyMaterialDefines*>(subMesh->_materialDefines.get());
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

  // Lights
  if (scene->lightsEnabled()) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, *defines, 1,
                               SMD::SPECULARTERM);
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

Material* ShadowOnlyMaterial::clone(const std::string& /*name*/,
                                    bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object ShadowOnlyMaterial::serialize() const
{
  return Json::object();
}

ShadowOnlyMaterial* ShadowOnlyMaterial::Parse(const Json::value& /*source*/,
                                              Scene* /*scene*/,
                                              const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
