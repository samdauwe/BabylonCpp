#include <babylon/materialslibrary/triplanar/tri_planar_material.h>

#include <nlohmann/json.hpp>

#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/triplanar/tri_planar_fragment_fx.h>
#include <babylon/materialslibrary/triplanar/tri_planar_vertex_fx.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

TriPlanarMaterial::TriPlanarMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , mixTexture{this, &TriPlanarMaterial::get_mixTexture,
                 &TriPlanarMaterial::set_mixTexture}
    , diffuseTextureX{this, &TriPlanarMaterial::get_diffuseTextureX,
                      &TriPlanarMaterial::set_diffuseTextureX}
    , diffuseTextureY{this, &TriPlanarMaterial::get_diffuseTextureY,
                      &TriPlanarMaterial::set_diffuseTextureY}
    , diffuseTextureZ{this, &TriPlanarMaterial::get_diffuseTextureZ,
                      &TriPlanarMaterial::set_diffuseTextureZ}
    , normalTextureX{this, &TriPlanarMaterial::get_normalTextureX,
                     &TriPlanarMaterial::set_normalTextureX}
    , normalTextureY{this, &TriPlanarMaterial::get_normalTextureY,
                     &TriPlanarMaterial::set_normalTextureY}
    , normalTextureZ{this, &TriPlanarMaterial::get_normalTextureZ,
                     &TriPlanarMaterial::set_normalTextureZ}
    , tileSize{1.f}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.2f, 0.2f, 0.2f)}
    , specularPower{64.f}
    , disableLighting{this, &TriPlanarMaterial::get_disableLighting,
                      &TriPlanarMaterial::set_disableLighting}
    , maxSimultaneousLights{this, &TriPlanarMaterial::get_maxSimultaneousLights,
                            &TriPlanarMaterial::set_maxSimultaneousLights}
    , _mixTexture{nullptr}
    , _diffuseTextureX{nullptr}
    , _diffuseTextureY{nullptr}
    , _diffuseTextureZ{nullptr}
    , _normalTextureX{nullptr}
    , _normalTextureY{nullptr}
    , _normalTextureZ{nullptr}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
{
  // Vertex shader
  Effect::ShadersStore()["triPlanarVertexShader"] = triPlanarVertexShader;

  // Fragment shader
  Effect::ShadersStore()["triPlanarPixelShader"] = triPlanarPixelShader;
}

TriPlanarMaterial::~TriPlanarMaterial()
{
}

BaseTexturePtr& TriPlanarMaterial::get_mixTexture()
{
  return _mixTexture;
}

void TriPlanarMaterial::set_mixTexture(const BaseTexturePtr& value)
{
  if (_mixTexture != value) {
    _mixTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TriPlanarMaterial::get_diffuseTextureX()
{
  return _diffuseTextureX;
}

void TriPlanarMaterial::set_diffuseTextureX(const TexturePtr& value)
{
  if (_diffuseTextureX != value) {
    _diffuseTextureX = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TriPlanarMaterial::get_diffuseTextureY()
{
  return _diffuseTextureY;
}

void TriPlanarMaterial::set_diffuseTextureY(const TexturePtr& value)
{
  if (_diffuseTextureY != value) {
    _diffuseTextureY = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TriPlanarMaterial::get_diffuseTextureZ()
{
  return _diffuseTextureZ;
}

void TriPlanarMaterial::set_diffuseTextureZ(const TexturePtr& value)
{
  if (_diffuseTextureZ != value) {
    _diffuseTextureZ = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TriPlanarMaterial::get_normalTextureX()
{
  return _normalTextureX;
}

void TriPlanarMaterial::set_normalTextureX(const TexturePtr& value)
{
  if (_normalTextureX != value) {
    _normalTextureX = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TriPlanarMaterial::get_normalTextureY()
{
  return _normalTextureY;
}

void TriPlanarMaterial::set_normalTextureY(const TexturePtr& value)
{
  if (_normalTextureY != value) {
    _normalTextureY = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

TexturePtr& TriPlanarMaterial::get_normalTextureZ()
{
  return _normalTextureZ;
}

void TriPlanarMaterial::set_normalTextureZ(const TexturePtr& value)
{
  if (_normalTextureZ != value) {
    _normalTextureZ = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool TriPlanarMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void TriPlanarMaterial::set_disableLighting(bool value)
{
  if (_disableLighting != value) {
    _disableLighting = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

unsigned int TriPlanarMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void TriPlanarMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights != value) {
    _maxSimultaneousLights = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool TriPlanarMaterial::needAlphaBlending() const
{
  return (alpha < 1.f);
}

bool TriPlanarMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr TriPlanarMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool TriPlanarMaterial::isReadyForSubMesh(AbstractMesh* mesh,
                                          BaseSubMesh* subMesh,
                                          bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<TriPlanarMaterialDefines>();
  }

  auto definesPtr = std::static_pointer_cast<TriPlanarMaterialDefines>(
    subMesh->_materialDefines);
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
    if (scene->texturesEnabled()) {
      if (StandardMaterial::DiffuseTextureEnabled()) {
        const std::vector<TexturePtr> textures{
          _diffuseTextureX, _diffuseTextureY, _diffuseTextureZ};
        const std::vector<std::string> textureDefines{"DIFFUSEX", "DIFFUSEY",
                                                      "DIFFUSEZ"};

        for (unsigned int i = 0; i < textures.size(); i++) {
          if (textures[i]) {
            if (!textures[i]->isReady()) {
              return false;
            }
            else {
              defines.boolDef[textureDefines[i]] = true;
            }
          }
        }
      }
      if (StandardMaterial::BumpTextureEnabled()) {
        const std::vector<TexturePtr> textures{_normalTextureX, _normalTextureY,
                                               _normalTextureZ};
        const std::vector<std::string> textureDefines{"BUMPX", "BUMPY",
                                                      "BUMPZ"};

        for (unsigned int i = 0; i < textures.size(); i++) {
          if (textures[i]) {
            if (!textures[i]->isReady()) {
              return false;
            }
            else {
              defines.boolDef[textureDefines[i]] = true;
            }
          }
        }
      }
    }
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(),
                                        fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, _disableLighting);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances ? true : false);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(mesh, defines, true, true);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines["FOG"]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              maxSimultaneousLights());

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKind};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKind);
    }

    if (defines["VERTEXCOLOR"]) {
      attribs.emplace_back(VertexBuffer::ColorKind);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    // Legacy browser patch
    const std::string shaderName{"triPlanar"};
    auto join = defines.toString();

    const std::vector<std::string> uniforms{
      "world",       "view",          "viewProjection", "vEyePosition",
      "vLightsType", "vDiffuseColor", "vSpecularColor", "vFogInfos",
      "vFogColor",   "pointSize",     "mBones",         "vClipPlane",
      "vClipPlane2", "vClipPlane3",   "vClipPlane4",    "tileSize"};

    const std::vector<std::string> samplers{
      "diffuseSamplerX", "diffuseSamplerY", "diffuseSamplerZ",
      "normalSamplerX",  "normalSamplerY",  "normalSamplerZ"};
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

void TriPlanarMaterial::bindForSubMesh(Matrix& world, Mesh* mesh,
                                       SubMesh* subMesh)
{
  auto scene = getScene();

  auto _defines
    = static_cast<TriPlanarMaterialDefines*>(subMesh->_materialDefines.get());
  if (!_defines) {
    return;
  }
  auto defines = *_defines;

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

  _activeEffect->setFloat("tileSize", tileSize);

  if (scene->getCachedMaterial() != this) {
    // Textures
    if (_diffuseTextureX) {
      _activeEffect->setTexture("diffuseSamplerX", _diffuseTextureX);
    }
    if (_diffuseTextureY) {
      _activeEffect->setTexture("diffuseSamplerY", _diffuseTextureY);
    }
    if (_diffuseTextureZ) {
      _activeEffect->setTexture("diffuseSamplerZ", _diffuseTextureZ);
    }
    if (_normalTextureX) {
      _activeEffect->setTexture("normalSamplerX", _normalTextureX);
    }
    if (_normalTextureY) {
      _activeEffect->setTexture("normalSamplerY", _normalTextureY);
    }
    if (_normalTextureZ) {
      _activeEffect->setTexture("normalSamplerZ", _normalTextureZ);
    }
    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    MaterialHelper::BindEyePosition(effect, scene);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor,
                           alpha * mesh->visibility);

  if (defines["SPECULARTERM"]) {
    _activeEffect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                               maxSimultaneousLights());
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

std::vector<IAnimatablePtr> TriPlanarMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_mixTexture && _mixTexture->animations.size() > 0) {
    results.emplace_back(_mixTexture);
  }

  return results;
}

std::vector<BaseTexturePtr> TriPlanarMaterial::getActiveTextures() const
{
  auto activeTextures = PushMaterial::getActiveTextures();

  if (_diffuseTextureX) {
    activeTextures.emplace_back(_diffuseTextureX);
  }

  if (_diffuseTextureY) {
    activeTextures.emplace_back(_diffuseTextureY);
  }

  if (_diffuseTextureZ) {
    activeTextures.emplace_back(_diffuseTextureZ);
  }

  if (_normalTextureX) {
    activeTextures.emplace_back(_normalTextureX);
  }

  if (_normalTextureY) {
    activeTextures.emplace_back(_normalTextureY);
  }

  if (_normalTextureZ) {
    activeTextures.emplace_back(_normalTextureZ);
  }

  return activeTextures;
}

bool TriPlanarMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  if (_diffuseTextureX == texture) {
    return true;
  }

  if (_diffuseTextureY == texture) {
    return true;
  }

  if (_diffuseTextureZ == texture) {
    return true;
  }

  if (_normalTextureX == texture) {
    return true;
  }

  if (_normalTextureY == texture) {
    return true;
  }

  if (_normalTextureZ == texture) {
    return true;
  }
  return false;
}

void TriPlanarMaterial::dispose(bool forceDisposeEffect,
                                bool forceDisposeTextures)
{

  if (_mixTexture) {
    _mixTexture->dispose();
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr TriPlanarMaterial::clone(const std::string& /*name*/,
                                     bool /*cloneChildren*/) const
{
  return nullptr;
}

json TriPlanarMaterial::serialize() const
{
  return nullptr;
}

const std::string TriPlanarMaterial::getClassName() const
{
  return "TriPlanarMaterial";
}

TriPlanarMaterial* TriPlanarMaterial::Parse(const json& /*source*/,
                                            Scene* /*scene*/,
                                            const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
