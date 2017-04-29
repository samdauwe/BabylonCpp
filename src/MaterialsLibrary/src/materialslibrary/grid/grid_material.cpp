#include <babylon/materialslibrary/grid/grid_material.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_helper.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

GridMaterial::GridMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , mainColor{Color3::White()}
    , lineColor{Color3::Black()}
    , gridRatio{1.f}
    , majorUnitFrequency{10.f}
    , minorUnitVisibility{0.33f}
    , opacity{1.f}
    , _gridControl{Vector4(gridRatio, majorUnitFrequency, minorUnitVisibility,
                           opacity)}
    , _cachedDefines{std_util::make_unique<GridMaterialDefines>()}
{
}

GridMaterial::~GridMaterial()
{
}

bool GridMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool GridMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
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

bool GridMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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

  if (opacity < 1.f) {
    _defines.defines[GMD::TRANSPARENT] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[GMD::FOG] = true;
  }

  // Get correct effect
  if (!_effect || !_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);
    scene->resetCachedMaterial();

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars,
                                     VertexBuffer::NormalKindChars};

    // Uniforms
    std::vector<std::string> uniforms{
      "worldViewProjection", "mainColor", "lineColor", "gridControl",
      "vFogInfos",           "vFogColor", "world",     "view"};

    // Samplers
    std::vector<std::string> samplers;

    // Effect
    std::string shaderName = scene->getEngine()->getCaps().standardDerivatives ?
                               "grid" :
                               "legacygrid";

    // Defines
    std::string join = _defines.toString();
    _effect = engine->createEffect(shaderName, attribs, uniforms, samplers,
                                   join, nullptr, onCompiled, onError);
  }

  if (!_effect->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void GridMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  auto scene = getScene();

  _effect->setMatrix("worldViewProjection",
                     world.multiply(getScene()->getTransformMatrix()));
  _effect->setMatrix("world", world);
  _effect->setMatrix("view", scene->getViewMatrix());
}

void GridMaterial::bind(Matrix* world, Mesh* mesh)
{
  auto scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);

  // Uniforms
  if (scene->getCachedMaterial() != this) {
    _effect->setColor3("mainColor", mainColor);
    _effect->setColor3("lineColor", lineColor);

    _gridControl.x = gridRatio;
    _gridControl.y = std::round(majorUnitFrequency);
    _gridControl.z = minorUnitVisibility;
    _gridControl.w = opacity;
    _effect->setVector4("gridControl", _gridControl);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  Material::bind(world, mesh);
}

void GridMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* GridMaterial::clone(const std::string& /*name*/,
                              bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object GridMaterial::serialize() const
{
  return Json::object();
}

GridMaterial* GridMaterial::Parse(const Json::value& /*source*/,
                                  Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
