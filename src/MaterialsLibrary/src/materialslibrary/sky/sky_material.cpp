#include <babylon/materialslibrary/sky/sky_material.h>

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

SkyMaterial::SkyMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
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
    , _cameraPosition{Vector3::Zero()}
    , _renderId{-1}
    , _cachedDefines{std_util::make_unique<SkyMaterialDefines>()}
{
}

SkyMaterial::~SkyMaterial()
{
}

bool SkyMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool SkyMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* SkyMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool SkyMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                              bool /*useInstances*/)
{
  if (!mesh) {
    return true;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool SkyMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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

  auto engine = scene->getEngine();
  _defines.reset();

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[SMD::CLIPPLANE] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[SMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[SMD::FOG] = true;
  }

  // Attribs
  if (mesh) {
    if (mesh->useVertexColors()
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[SMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha()) {
        _defines.defines[SMD::VERTEXALPHA] = true;
      }
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines) || !_effect) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines[SMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (_defines[SMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    // Legacy browser patch
    std::string shaderName = "sky";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{
      "world",       "viewProjection", "view",           "vFogInfos",
      "vFogColor",   "pointSize",      "vClipPlane",     "luminance",
      "turbidity",   "rayleigh",       "mieCoefficient", "mieDirectionalG",
      "sunPosition", "cameraPosition"};
    std::vector<std::string> samplers;

    _effect = engine->createEffect(shaderName, attribs, uniforms, samplers,
                                   join, fallbacks.get(), onCompiled, onError);
  }

  if (!_effect->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  if (mesh) {
    if (!mesh->_materialDefines) {
      mesh->_materialDefines = std_util::make_unique<SkyMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void SkyMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void SkyMaterial::bind(Matrix* world, Mesh* mesh)
{
  auto scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);
  _effect->setMatrix("viewProjection", scene->getTransformMatrix());

  if (scene->getCachedMaterial() != this) {
    // Clip plane
    if (scene->clipPlane()) {
      auto clipPlane = scene->clipPlane();
      _effect->setFloat4("vClipPlane", clipPlane->normal.x, clipPlane->normal.y,
                         clipPlane->normal.z, clipPlane->d);
    }

    // Point size
    if (pointsCloud()) {
      _effect->setFloat("pointSize", pointSize);
    }
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  // Sky
  auto camera = scene->activeCamera;
  if (camera) {
    auto cameraWorldMatrix = camera->getWorldMatrix();
    _cameraPosition.x      = cameraWorldMatrix->m[12];
    _cameraPosition.y      = cameraWorldMatrix->m[13];
    _cameraPosition.z      = cameraWorldMatrix->m[14];
    _effect->setVector3("cameraPosition", _cameraPosition);
  }

  if (luminance > 0.f) {
    _effect->setFloat("luminance", luminance);
  }

  _effect->setFloat("luminance", luminance);
  _effect->setFloat("turbidity", turbidity);
  _effect->setFloat("rayleigh", rayleigh);
  _effect->setFloat("mieCoefficient", mieCoefficient);
  _effect->setFloat("mieDirectionalG", mieDirectionalG);

  if (!useSunPosition) {
    float theta = Math::PI * (inclination - 0.5f);
    float phi   = Math::PI2 * (azimuth - 0.5f);

    sunPosition.x = distance * std::cos(phi);
    sunPosition.y = distance * std::sin(phi) * std::sin(theta);
    sunPosition.z = distance * std::sin(phi) * std::cos(theta);
  }

  _effect->setVector3("sunPosition", sunPosition);

  Material::bind(world, mesh);
}

std::vector<IAnimatable*> SkyMaterial::getAnimatables()
{
  return std::vector<IAnimatable*>();
}

void SkyMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* SkyMaterial::clone(const std::string& /*name*/,
                             bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object SkyMaterial::serialize() const
{
  return Json::object();
}

SkyMaterial* SkyMaterial::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
