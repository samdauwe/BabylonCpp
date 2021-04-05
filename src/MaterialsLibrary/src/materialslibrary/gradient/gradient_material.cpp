#include <babylon/materialslibrary/gradient/gradient_material.h>

#include <nlohmann/json.hpp>

#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materialslibrary/gradient/gradient_fragment_fx.h>
#include <babylon/materialslibrary/gradient/gradient_vertex_fx.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

GradientMaterial::GradientMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , maxSimultaneousLights{this, &GradientMaterial::get_maxSimultaneousLights,
                            &GradientMaterial::set_maxSimultaneousLights}
    , topColor{Color3(1.f, 0.f, 0.f)}
    , topColorAlpha{1.f}
    , bottomColor{Color3(0.f, 0.f, 1.f)}
    , bottomColorAlpha{1.f}
    , offset{0}
    , scale{1.f}
    , smoothness{1.f}
    , disableLighting{this, &GradientMaterial::get_disableLighting,
                      &GradientMaterial::set_disableLighting}
    , _maxSimultaneousLights{4}
    , _disableLighting{false}
{
  // Vertex shader
  Effect::ShadersStore()["gradientVertexShader"] = gradientVertexShader;

  // Fragment shader
  Effect::ShadersStore()["gradientPixelShader"] = gradientPixelShader;
}

GradientMaterial::~GradientMaterial() = default;

unsigned int GradientMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void GradientMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights != value) {
    _maxSimultaneousLights = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool GradientMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void GradientMaterial::set_disableLighting(bool value)
{
  if (_disableLighting != value) {
    _disableLighting = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool GradientMaterial::needAlphaBlending() const
{
  return (alpha < 1.f || topColorAlpha < 1.f || bottomColorAlpha < 1.f);
}

bool GradientMaterial::needAlphaTesting() const
{
  return true;
}

BaseTexturePtr GradientMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool GradientMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances)
{
  if (isFrozen()) {
    if (subMesh->effect() && subMesh->effect()->_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->materialDefines = std::make_shared<GradientMaterialDefines>();
  }

  auto definesPtr = std::static_pointer_cast<GradientMaterialDefines>(subMesh->_materialDefines);
  auto& defines   = *definesPtr.get();
  auto scene      = getScene();

  if (_isReadyForSubMesh(subMesh)) {
    return true;
  }

  auto engine = scene->getEngine();

  MaterialHelper::PrepareDefinesForFrameBoundValues(scene, engine, defines, useInstances);

  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(), fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, false, _maxSimultaneousLights, _disableLighting);

  defines.boolDef["EMISSIVE"] = _disableLighting;

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

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks);

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    defines.boolDef["IMAGEPROCESSINGPOSTPROCESS"]
      = scene->imageProcessingConfiguration()->applyByPostProcess();

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKind};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKind);
    }

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKind);
    }

    if (defines["UV2"]) {
      attribs.emplace_back(VertexBuffer::UV2Kind);
    }

    if (defines["VERTEXCOLOR"]) {
      attribs.emplace_back(VertexBuffer::ColorKind);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines, *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    // Legacy browser patch
    const std::string shaderName{"gradient"};
    auto join = defines.toString();
    const std::vector<std::string> uniforms{
      "world",       "view",        "viewProjection", "vEyePosition", "vLightsType",
      "vFogInfos",   "vFogColor",   "pointSize",      "mBones",       "vClipPlane",
      "vClipPlane2", "vClipPlane3", "vClipPlane4",    "vClipPlane5",  "vClipPlane6",
      "topColor",    "bottomColor", "offset",         "smoothness",   "scale"};
    const std::vector<std::string> samplers{"diffuseSampler"};
    const std::vector<std::string> uniformBuffers{};

    IEffectCreationOptions options;
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

    subMesh->setEffect(scene->getEngine()->createEffect(shaderName, options, engine), definesPtr,
                       _materialContext);
  }
  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  defines._renderId                      = scene->getRenderId();
  subMesh->effect()->_wasPreviouslyReady = true;

  return true;
}

void GradientMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines = static_cast<GradientMaterialDefines*>(subMesh->_materialDefines.get());
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
  MaterialHelper::BindBonesParameters(mesh, effect.get());

  if (_mustRebind(scene, effect)) {
    // Clip plane
    MaterialHelper::BindClipPlane(effect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    MaterialHelper::BindEyePosition(effect.get(), scene);
  }

  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect.get(), *defines, maxSimultaneousLights);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog() && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _activeEffect->setColor4("topColor", topColor, topColorAlpha);
  _activeEffect->setColor4("bottomColor", bottomColor, bottomColorAlpha);
  _activeEffect->setFloat("offset", offset);
  _activeEffect->setFloat("scale", scale);
  _activeEffect->setFloat("smoothness", smoothness);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> GradientMaterial::getAnimatables()
{
  return std::vector<IAnimatablePtr>();
}

void GradientMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                               bool /*notBoundToMesh*/)
{
  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr GradientMaterial::clone(const std::string& /*name*/, bool /*cloneChildren*/) const
{
  return nullptr;
}

json GradientMaterial::serialize() const
{
  return nullptr;
}

std::string GradientMaterial::getClassName() const
{
  return "GradientMaterial";
}

GradientMaterial* GradientMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                          const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
