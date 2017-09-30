#include <babylon/materialslibrary/fire/fire_material.h>

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
#include <babylon/materials/textures/base_texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

unsigned int FireMaterial::maxSimultaneousLights = 4;

FireMaterial::FireMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , speed{1.f}
    , _diffuseTexture{nullptr}
    , _distortionTexture{nullptr}
    , _opacityTexture{nullptr}
    , _renderId{-1}
    , _lastTime{0.f}
{
}

FireMaterial::~FireMaterial()
{
}

bool FireMaterial::needAlphaBlending()
{
  return false;
}

bool FireMaterial::needAlphaTesting()
{
  return true;
}

BaseTexture* FireMaterial::getAlphaTestTexture()
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
    subMesh->_materialDefines = std::make_unique<FireMaterialDefines>();
  }

  auto defines
    = *(static_cast<FireMaterialDefines*>(subMesh->_materialDefines.get()));
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
    if (_diffuseTexture && StandardMaterial::DiffuseTextureEnabled()) {
      if (!_diffuseTexture->isReady()) {
        return false;
      }
      else {
        defines._needUVs               = true;
        _defines.defines[FMD::DIFFUSE] = true;
      }
    }
  }

  // Misc.
  if (defines._areMiscDirty) {
    _defines.defines[FMD::POINTSIZE]
      = (pointsCloud() || scene->forcePointsCloud());
    _defines.defines[FMD::FOG]
      = (scene->fogEnabled() && mesh->applyFog()
         && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled());
  }

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, FMD::CLIPPLANE, FMD::ALPHATEST,
    FMD::INSTANCES);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, false, true, FMD::NORMAL, FMD::UV1, FMD::UV2,
    FMD::VERTEXCOLOR, FMD::VERTEXALPHA, 0, 0);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::unique_ptr<EffectFallbacks>();
    if (defines[FMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (defines[FMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines[FMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  FMD::INSTANCES);

    // Legacy browser patch
    const std::string shaderName{"fire"};
    auto join = defines.toString();
    EffectCreationOptions options;
    options.attributes = std::move(attribs);
    options.uniformsNames
      = {"world", "view", "viewProjection", "vEyePosition", "vFogInfos",
         "vFogColor", "pointSize", "vDiffuseInfos", "mBones", "vClipPlane",
         "diffuseMatrix",
         // Fire
         "time", "speed"};
    options.samplers   = {"diffuseSampler",
                        // Fire
                        "distortionSampler", "opacitySampler"};
    options.defines    = std::move(join);
    options.fallbacks  = std::move(fallbacks);
    options.onCompiled = onCompiled;
    options.onError    = onError;

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

void FireMaterial::bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto _defines
    = static_cast<FireMaterialDefines*>(subMesh->_materialDefines.get());
  if (!_defines) {
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

      _activeEffect->setFloat2(
        "vDiffuseInfos", static_cast<float>(_diffuseTexture->coordinatesIndex),
        _diffuseTexture->level);
      _activeEffect->setMatrix("diffuseMatrix",
                               *_diffuseTexture->getTextureMatrix());

      _activeEffect->setTexture("distortionSampler", _distortionTexture);
      _activeEffect->setTexture("opacitySampler", _opacityTexture);
    }

    // Clip plane
    if (scene->clipPlane()) {
      auto clipPlane = scene->clipPlane();
      _activeEffect->setFloat4("vClipPlane", clipPlane->normal.x,
                               clipPlane->normal.y, clipPlane->normal.z,
                               clipPlane->d);
    }

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    _activeEffect->setVector3("vEyePosition",
                              scene->_mirroredCameraPosition ?
                                *scene->_mirroredCameraPosition :
                                scene->activeCamera->position);
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

std::vector<IAnimatable*> FireMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

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

std::vector<BaseTexture*> FireMaterial::getActiveTextures() const
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

bool FireMaterial::hasTexture(BaseTexture* texture) const
{
  if (Material::hasTexture(texture)) {
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

void FireMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (_diffuseTexture) {
    _diffuseTexture->dispose();
  }

  if (_distortionTexture) {
    _distortionTexture->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* FireMaterial::clone(const std::string& /*name*/,
                              bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object FireMaterial::serialize() const
{
  return Json::object();
}

FireMaterial* FireMaterial::Parse(const Json::value& /*source*/,
                                  Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
