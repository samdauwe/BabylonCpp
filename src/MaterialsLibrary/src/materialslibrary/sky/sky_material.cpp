#include <babylon/materialslibrary/sky/sky_material.h>

#include <nlohmann/json.hpp>

#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materialslibrary/sky/sky_fragment_fx.h>
#include <babylon/materialslibrary/sky/sky_vertex_fx.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

SkyMaterial::SkyMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , luminance{1.f}
    , turbidity{10.f}
    , rayleigh{2.f}
    , mieCoefficient{0.005f}
    , mieDirectionalG{0.8f}
    , distance{500.f}
    , inclination{0.49f}
    , azimuth{0.25f}
    , sunPosition{Vector3(0.f, 100.f, 0.f)}
    , useSunPosition{false}
    , cameraOffset{Vector3::Zero()}
    , _cameraPosition{Vector3::Zero()}
    , _renderId{-1}
{
  // Vertex shader
  Effect::ShadersStore()["skyVertexShader"] = skyVertexShader;

  // Fragment shader
  Effect::ShadersStore()["skyPixelShader"] = skyPixelShader;
}

SkyMaterial::~SkyMaterial() = default;

bool SkyMaterial::needAlphaBlending() const
{
  return (alpha < 1.f);
}

bool SkyMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr SkyMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool SkyMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh, bool /*useInstances*/)
{
  if (isFrozen()) {
    if (subMesh->effect() && subMesh->effect()->_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<SkyMaterialDefines>();
  }

  auto definesPtr = std::static_pointer_cast<SkyMaterialDefines>(subMesh->_materialDefines);
  auto& defines   = *definesPtr.get();
  auto scene      = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, pointsCloud(), fogEnabled(), false,
                                        defines);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(mesh, defines, true, false);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines["FOG"]) {
      fallbacks->addFallback(1, "FOG");
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKind};

    if (defines["VERTEXCOLOR"]) {
      attribs.emplace_back(VertexBuffer::ColorKind);
    }

    const std::string shaderName{"sky"};
    auto join = defines.toString();
    const std::vector<std::string> uniforms{
      "world",          "viewProjection",  "view",        "vFogInfos",      "vFogColor",
      "pointSize",      "vClipPlane",      "vClipPlane2", "vClipPlane3",    "vClipPlane4",
      "vClipPlane5",    "vClipPlane6",     "luminance",   "turbidity",      "rayleigh",
      "mieCoefficient", "mieDirectionalG", "sunPosition", "cameraPosition", "cameraOffset"};
    const std::vector<std::string> samplers{};
    const std::vector<std::string> uniformBuffers{};

    IEffectCreationOptions options;
    options.attributes          = std::move(attribs);
    options.uniformsNames       = std::move(uniforms);
    options.uniformBuffersNames = std::move(uniformBuffers);
    options.samplers            = std::move(samplers);
    options.materialDefines     = &defines;
    options.defines             = std::move(join);
    options.fallbacks           = std::move(fallbacks);
    options.onCompiled          = onCompiled;
    options.onError             = onError;

    subMesh->setEffect(scene->getEngine()->createEffect(shaderName, options, engine), definesPtr);
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  _renderId                              = scene->getRenderId();
  subMesh->effect()->_wasPreviouslyReady = true;

  return true;
}

void SkyMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines = static_cast<SkyMaterialDefines*>(subMesh->_materialDefines.get());
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

  if (_mustRebind(scene, effect)) {
    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog() && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  // Sky
  const auto& camera = scene->activeCamera();
  if (camera) {
    const auto& cameraWorldMatrixM = camera->getWorldMatrix().m();
    _cameraPosition.x              = cameraWorldMatrixM[12];
    _cameraPosition.y              = cameraWorldMatrixM[13];
    _cameraPosition.z              = cameraWorldMatrixM[14];
    _activeEffect->setVector3("cameraPosition", _cameraPosition);
  }

  _activeEffect->setVector3("cameraOffset", cameraOffset);

  if (luminance > 0.f) {
    _activeEffect->setFloat("luminance", luminance);
  }

  _activeEffect->setFloat("turbidity", turbidity);
  _activeEffect->setFloat("rayleigh", rayleigh);
  _activeEffect->setFloat("mieCoefficient", mieCoefficient);
  _activeEffect->setFloat("mieDirectionalG", mieDirectionalG);

  if (!useSunPosition) {
    auto theta = Math::PI * (inclination - 0.5f);
    auto phi   = 2.f * Math::PI * (azimuth - 0.5f);

    sunPosition.x = distance * std::cos(phi);
    sunPosition.y = distance * std::sin(phi) * std::sin(theta);
    sunPosition.z = distance * std::sin(phi) * std::cos(theta);
  }

  _activeEffect->setVector3("sunPosition", sunPosition);

  _afterBind(mesh, _activeEffect);
}

std::vector<IAnimatablePtr> SkyMaterial::getAnimatables()
{
  return std::vector<IAnimatablePtr>();
}

void SkyMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                          bool /*notBoundToMesh*/)
{
  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr SkyMaterial::clone(const std::string& /*name*/, bool /*cloneChildren*/) const
{
  return nullptr;
}

json SkyMaterial::serialize() const
{
  return nullptr;
}

std::string SkyMaterial::getClassName() const
{
  return "SkyMaterial";
}

SkyMaterial* SkyMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
