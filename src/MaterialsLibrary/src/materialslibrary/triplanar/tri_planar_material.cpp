#include <babylon/materialslibrary/triplanar/tri_planar_material.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

TriPlanarMaterial::TriPlanarMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , tileSize{1.f}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.2f, 0.2f, 0.2f)}
    , specularPower{64.f}
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
}

TriPlanarMaterial::~TriPlanarMaterial()
{
}

bool TriPlanarMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool TriPlanarMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* TriPlanarMaterial::getAlphaTestTexture()
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
    subMesh->_materialDefines = std::make_unique<TriPlanarMaterialDefines>();
  }

  auto defines = *(
    static_cast<TriPlanarMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

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
        std::vector<Texture*> textures{_diffuseTextureX, _diffuseTextureY,
                                       _diffuseTextureZ};
        const Uint32Array textureDefines{TPMD::DIFFUSEX, TPMD::DIFFUSEY,
                                         TPMD::DIFFUSEZ};
        for (unsigned int i = 0; i < textures.size(); i++) {
          if (textures[i]) {
            if (!textures[i]->isReady()) {
              return false;
            }
            else {
              defines.defines[textureDefines[i]] = true;
            }
          }
        }
      }
      if (StandardMaterial::BumpTextureEnabled()) {
        std::vector<Texture*> textures{_normalTextureX, _normalTextureY,
                                       _normalTextureZ};
        const Uint32Array textureDefines{TPMD::BUMPX, TPMD::BUMPY, TPMD::BUMPZ};

        for (unsigned int i = 0; i < textures.size(); i++) {
          if (textures[i]) {
            if (!textures[i]->isReady()) {
              return false;
            }
            else {
              defines.defines[textureDefines[i]] = true;
            }
          }
        }
      }
    }
  }

  // Misc.
  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, false, pointsCloud(), fogEnabled(), defines,
    TPMD::LOGARITHMICDEPTH, TPMD::POINTSIZE, TPMD::FOG);

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, _disableLighting,
    TPMD::SPECULARTERM, TPMD::SHADOWFULLFLOAT);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, TPMD::CLIPPLANE, TPMD::ALPHATEST,
    TPMD::INSTANCES);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, true, true, false, TPMD::NORMAL, TPMD::UV1, TPMD::UV2,
    TPMD::VERTEXCOLOR, TPMD::VERTEXALPHA);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines[TPMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

    if (defines[TPMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines[TPMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  TPMD::INSTANCES);

    // Legacy browser patch
    const std::string shaderName{"triplanar"};
    auto join = defines.toString();

    const std::vector<std::string> uniforms{
      "world",       "view",          "viewProjection", "vEyePosition",
      "vLightsType", "vDiffuseColor", "vSpecularColor", "vFogInfos",
      "vFogColor",   "pointSize",     "mBones",         "vClipPlane",
      "tileSize"};

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
      scene->getEngine()->createEffect(shaderName, options, engine), defines);
  }

  if (!subMesh->effect()->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void TriPlanarMaterial::bindForSubMesh(Matrix* world, Mesh* mesh,
                                       SubMesh* subMesh)
{
  auto scene = getScene();

  auto _defines
    = static_cast<TriPlanarMaterialDefines*>(subMesh->_materialDefines.get());
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

    _activeEffect->setVector3("vEyePosition",
                              scene->_mirroredCameraPosition ?
                                *scene->_mirroredCameraPosition :
                                scene->activeCamera->position);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor,
                           alpha * mesh->visibility);

  if (defines[TPMD::SPECULARTERM]) {
    _activeEffect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                               _maxSimultaneousLights, TPMD::SPECULARTERM);
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

std::vector<IAnimatable*> TriPlanarMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (_mixTexture && _mixTexture->animations.size() > 0) {
    results.emplace_back(_mixTexture);
  }

  return results;
}

std::vector<BaseTexture*> TriPlanarMaterial::getActiveTextures() const
{
  auto activeTextures = Material::getActiveTextures();

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

bool TriPlanarMaterial::hasTexture(BaseTexture* texture) const
{
  if (Material::hasTexture(texture)) {
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

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* TriPlanarMaterial::clone(const std::string& /*name*/,
                                   bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object TriPlanarMaterial::serialize() const
{
  return Json::object();
}

TriPlanarMaterial* TriPlanarMaterial::Parse(const Json::value& /*source*/,
                                            Scene* /*scene*/,
                                            const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
