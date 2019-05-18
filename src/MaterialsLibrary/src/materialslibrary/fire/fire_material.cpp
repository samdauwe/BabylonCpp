#include <babylon/materialslibrary/fire/fire_material.h>

#include <nlohmann/json.hpp>

#include <babylon/cameras/camera.h>
#include <babylon/core/time.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materialslibrary/fire/fire_fragment_fx.h>
#include <babylon/materialslibrary/fire/fire_vertex_fx.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

unsigned int FireMaterial::maxSimultaneousLights = 4;

FireMaterial::FireMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , diffuseTexture{this, &FireMaterial::get_diffuseTexture,
                     &FireMaterial::set_diffuseTexture}
    , distortionTexture{this, &FireMaterial::get_distortionTexture,
                        &FireMaterial::set_distortionTexture}
    , opacityTexture{this, &FireMaterial::get_opacityTexture,
                     &FireMaterial::set_opacityTexture}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , speed{1.f}
    , _diffuseTexture{nullptr}
    , _distortionTexture{nullptr}
    , _opacityTexture{nullptr}
    , _renderId{-1}
    , _lastTime{0.f}
{
  // Vertex shader
  Effect::ShadersStore()["fireVertexShader"] = fireVertexShader;

  // Fragment shader
  Effect::ShadersStore()["firePixelShader"] = firePixelShader;
}

FireMaterial::~FireMaterial()
{
}

BaseTexturePtr& FireMaterial::get_diffuseTexture()
{
  return _diffuseTexture;
}

void FireMaterial::set_diffuseTexture(const BaseTexturePtr& value)
{
  if (_diffuseTexture != value) {
    _diffuseTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

BaseTexturePtr& FireMaterial::get_distortionTexture()
{
  return _distortionTexture;
}

void FireMaterial::set_distortionTexture(const BaseTexturePtr& value)
{
  if (_distortionTexture != value) {
    _distortionTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

BaseTexturePtr& FireMaterial::get_opacityTexture()
{
  return _opacityTexture;
}

void FireMaterial::set_opacityTexture(const BaseTexturePtr& value)
{
  if (_opacityTexture != value) {
    _opacityTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool FireMaterial::needAlphaBlending() const
{
  return false;
}

bool FireMaterial::needAlphaTesting() const
{
  return true;
}

BaseTexturePtr FireMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool FireMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                     bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<FireMaterialDefines>();
  }

  auto definesPtr
    = std::static_pointer_cast<FireMaterialDefines>(subMesh->_materialDefines);
  auto& defines = *definesPtr.get();
  auto scene    = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
      if (!_diffuseTexture->isReady()) {
        return false;
      }
      else {
        defines._needUVs            = true;
        _defines.boolDef["DIFFUSE"] = true;
      }
    }
  }

  _defines.boolDef["ALPHATEST"] = _opacityTexture ? true : false;

  // Misc.
  if (defines._areMiscDirty) {
    _defines.boolDef["POINTSIZE"]
      = (pointsCloud() || scene->forcePointsCloud());
    _defines.boolDef["FOG"]
      = (scene->fogEnabled() && mesh->applyFog()
         && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled());
  }

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances ? true : false);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(mesh, defines, false, true);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::unique_ptr<EffectFallbacks>();
    if (defines["FOG"]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKind};

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKind);
    }

    if (defines["VERTEXCOLOR"]) {
      attribs.emplace_back(VertexBuffer::ColorKind);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    // Legacy browser patch
    const std::string shaderName{"fire"};
    auto join = defines.toString();
    EffectCreationOptions options;
    options.attributes = std::move(attribs);
    options.uniformsNames
      = {"world", "view", "viewProjection", "vEyePosition", "vFogInfos",
         "vFogColor", "pointSize", "vDiffuseInfos", "mBones", "vClipPlane",
         "vClipPlane2", "vClipPlane3", "vClipPlane4", "diffuseMatrix",
         // Fire
         "time", "speed"};
    options.samplers              = {"diffuseSampler",
                        // Fire
                        "distortionSampler", "opacitySampler"};
    options.defines               = std::move(join);
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.maxSimultaneousLights = 4;

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

void FireMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto iDefines
    = static_cast<FireMaterialDefines*>(subMesh->_materialDefines.get());
  if (!iDefines) {
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
    // Textures
    if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
      _activeEffect->setTexture("diffuseSampler", _diffuseTexture);

      _activeEffect->setFloat2(
        "vDiffuseInfos", static_cast<float>(_diffuseTexture->coordinatesIndex),
        _diffuseTexture->level);
      _activeEffect->setMatrix("diffuseMatrix",
                               *_diffuseTexture->getTextureMatrix());

      _activeEffect->setTexture("distortionSampler", _distortionTexture);
      _activeEffect->setTexture("opacitySampler", _opacityTexture);
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    MaterialHelper::BindEyePosition(effect, scene);
  }

  _activeEffect->setColor4("vDiffuseColor", _scaledDiffuse,
                           alpha * mesh->visibility);

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  // Time
  _lastTime += scene->getEngine()->getDeltaTime();
  _activeEffect->setFloat("time", _lastTime);

  // Speed
  _activeEffect->setFloat("speed", speed);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> FireMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_diffuseTexture && _diffuseTexture->animations.size() > 0) {
    results.emplace_back(_diffuseTexture);
  }

  if (_distortionTexture && _distortionTexture->animations.size() > 0) {
    results.emplace_back(_distortionTexture);
  }

  if (_opacityTexture && _opacityTexture->animations.size() > 0) {
    results.emplace_back(_opacityTexture);
  }

  return results;
}

std::vector<BaseTexturePtr> FireMaterial::getActiveTextures() const
{
  auto activeTextures = Material::getActiveTextures();

  if (_diffuseTexture) {
    activeTextures.emplace_back(_diffuseTexture);
  }

  if (_distortionTexture) {
    activeTextures.emplace_back(_distortionTexture);
  }

  if (_opacityTexture) {
    activeTextures.emplace_back(_opacityTexture);
  }

  return activeTextures;
}

bool FireMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  if (_diffuseTexture == texture) {
    return true;
  }

  if (_distortionTexture == texture) {
    return true;
  }

  if (_opacityTexture == texture) {
    return true;
  }

  return false;
}

const std::string FireMaterial::getClassName() const
{
  return "FireMaterial";
}

void FireMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (_diffuseTexture) {
    _diffuseTexture->dispose();
  }

  if (_distortionTexture) {
    _distortionTexture->dispose();
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr FireMaterial::clone(const std::string& /*name*/,
                                bool /*cloneChildren*/) const
{
  return nullptr;
}

json FireMaterial::serialize() const
{
  return nullptr;
}

FireMaterial* FireMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
