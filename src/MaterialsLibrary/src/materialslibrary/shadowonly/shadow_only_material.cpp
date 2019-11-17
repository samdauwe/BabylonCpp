#include <babylon/materialslibrary/shadowonly/shadow_only_material.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

ShadowOnlyMaterial::ShadowOnlyMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , shadowColor{Color3::Black()}
    , activeLight{this, &ShadowOnlyMaterial::get_activeLight,
                  &ShadowOnlyMaterial::set_activeLight}
    , _renderId{-1}
    , _activeLight{nullptr}
{
}

ShadowOnlyMaterial::~ShadowOnlyMaterial() = default;

IShadowLightPtr& ShadowOnlyMaterial::get_activeLight()
{
  return _activeLight;
}

void ShadowOnlyMaterial::set_activeLight(const IShadowLightPtr& light)
{
  _activeLight = light;
}

bool ShadowOnlyMaterial::needAlphaBlending() const
{
  return true;
}

bool ShadowOnlyMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr ShadowOnlyMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool ShadowOnlyMaterial::isReadyForSubMesh(AbstractMesh* mesh,
                                           BaseSubMesh* subMesh,
                                           bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<ShadowOnlyMaterialDefines>();
  }

  auto definesPtr = std::static_pointer_cast<ShadowOnlyMaterialDefines>(
    subMesh->_materialDefines);
  auto& defines = *definesPtr.get();
  auto scene    = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Ensure that active light is the first shadow light
  if (_activeLight) {
    for (const auto& light : mesh->_lightSources) {
      if (light->shadowEnabled) {
        auto iActiveLight = std::dynamic_pointer_cast<Light>(_activeLight);
        if (iActiveLight == light) {
          break; // We are good
        }

        auto it = std::find(mesh->_lightSources.begin(),
                            mesh->_lightSources.end(), iActiveLight);

        if (it != mesh->_lightSources.end()) {
          mesh->_lightSources.erase(it);
          mesh->_lightSources.insert(mesh->_lightSources.begin(), iActiveLight);
        }
        break;
      }
    }
  }

  MaterialHelper::PrepareDefinesForFrameBoundValues(scene, engine, defines, useInstances);

  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(),
                                        fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  defines._needNormals
    = MaterialHelper::PrepareDefinesForLights(scene, mesh, defines, false, 1);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(mesh, defines, false, true);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines["FOG"]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks, 1);

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKind};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKind);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    const std::string shaderName{"shadowOnly"};
    auto join = defines.toString();
    EffectCreationOptions options;
    options.attributes = std::move(attribs);
    options.uniformsNames
      = {"world",       "view",        "viewProjection", "vEyePosition",
         "vLightsType", "vFogInfos",   "vFogColor",      "pointSize",
         "alpha",       "shadowColor", "mBones",         "vClipPlane",
         "vClipPlane2", "vClipPlane3", "vClipPlane4"};
    options.uniformBuffersNames   = {};
    options.samplers              = {};
    options.defines               = std::move(join);
    options.maxSimultaneousLights = 1;
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.indexParameters       = {{"maxSimultaneousLights", 1}};

    subMesh->setEffect(
      scene->getEngine()->createEffect(shaderName, options, engine),
      definesPtr);
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void ShadowOnlyMaterial::bindForSubMesh(Matrix& world, Mesh* mesh,
                                        SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines
    = static_cast<ShadowOnlyMaterialDefines*>(subMesh->_materialDefines.get());
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

  if (_mustRebind(scene, effect)) {
    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    _activeEffect->setFloat("alpha", alpha);
    _activeEffect->setColor3("shadowColor", shadowColor);

    MaterialHelper::BindEyePosition(effect, scene);
  }

  // Lights
  if (scene->lightsEnabled()) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, *defines, 1);
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

MaterialPtr ShadowOnlyMaterial::clone(const std::string& /*name*/,
                                      bool /*cloneChildren*/) const
{
  return nullptr;
}

json ShadowOnlyMaterial::serialize() const
{
  return nullptr;
}

const std::string ShadowOnlyMaterial::getClassName() const
{
  return "ShadowOnlyMaterial";
}

ShadowOnlyMaterial* ShadowOnlyMaterial::Parse(const json& /*source*/,
                                              Scene* /*scene*/,
                                              const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
