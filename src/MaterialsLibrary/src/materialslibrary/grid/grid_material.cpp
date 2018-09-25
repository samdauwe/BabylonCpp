#include <babylon/materialslibrary/grid/grid_material.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materialslibrary/grid/grid_fragment_fx.h>
#include <babylon/materialslibrary/grid/grid_vertex_fx.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

GridMaterial::GridMaterial(const std::string& iName, Scene* scene)
    : PushMaterial{iName, scene}
    , mainColor{Color3::Black()}
    , lineColor{Color3::Teal()}
    , gridRatio{1.f}
    , gridOffset{Vector3::Zero()}
    , majorUnitFrequency{10.f}
    , minorUnitVisibility{0.33f}
    , opacity{1.f}
    , preMultiplyAlpha{false}
    , _gridControl{Vector4(gridRatio, majorUnitFrequency, minorUnitVisibility,
                           opacity)}
    , _renderId{-1}
{
  // Vertex shader
  Effect::ShadersStore()["gridVertexShader"] = gridVertexShader;

  // Fragment shader
  Effect::ShadersStore()["gridPixelShader"] = gridPixelShader;
}

GridMaterial::~GridMaterial()
{
}

bool GridMaterial::needAlphaBlending() const
{
  return (opacity < 1.f);
}

bool GridMaterial::needAlphaBlendingForMesh(const AbstractMesh& /*mesh*/) const
{
  return needAlphaBlending();
}

bool GridMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                     bool /*useInstances*/)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<GridMaterialDefines>();
  }

  auto defines
    = *(static_cast<GridMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  if (defines["TRANSPARENT"] != (opacity < 1.f)) {
    defines.boolDef["TRANSPARENT"] = !defines["TRANSPARENT"];
    defines.markAsUnprocessed();
  }

  if (defines["PREMULTIPLYALPHA"] != preMultiplyAlpha) {
    defines.boolDef["PREMULTIPLYALPHA"] = !defines["PREMULTIPLYALPHA"];
    defines.markAsUnprocessed();
  }

  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, false, fogEnabled(),
                                        false, defines);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars,
                                     VertexBuffer::NormalKindChars};

    // Defines
    const std::string join = defines.toString();

    // Uniforms
    const std::vector<std::string> uniforms{
      "projection", "worldView", "mainColor", "lineColor", "gridControl",
      "gridOffset", "vFogInfos", "vFogColor", "world",     "view"};

    // Samplers
    const std::vector<std::string> samplers{};

    EffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.maxSimultaneousLights = 4;
    options.fallbacks             = nullptr;
    options.onCompiled            = onCompiled;
    options.onError               = onError;

    subMesh->setEffect(
      scene->getEngine()->createEffect("grid", options, engine), defines);
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void GridMaterial::bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines
    = static_cast<GridMaterialDefines*>(subMesh->_materialDefines.get());
  if (!defines) {
    return;
  }

  auto effect = subMesh->effect();
  if (!effect) {
    return;
  }
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(*world);
  _activeEffect->setMatrix("worldView",
                           world->multiply(scene->getViewMatrix()));
  _activeEffect->setMatrix("view", scene->getViewMatrix());
  _activeEffect->setMatrix("projection", scene->getProjectionMatrix());

  // Uniforms
  if (_mustRebind(scene, effect)) {
    _activeEffect->setColor3("mainColor", mainColor);
    _activeEffect->setColor3("lineColor", lineColor);

    _activeEffect->setVector3("gridOffset", gridOffset);

    _gridControl.x = gridRatio;
    _gridControl.y = std::round(majorUnitFrequency);
    _gridControl.z = minorUnitVisibility;
    _gridControl.w = opacity;
    _activeEffect->setVector4("gridControl", _gridControl);
  }
  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _afterBind(mesh, _activeEffect);
}

void GridMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr GridMaterial::clone(const std::string& /*name*/,
                                bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object GridMaterial::serialize() const
{
  return Json::object();
}

const std::string GridMaterial::getClassName() const
{
  return "GridMaterial";
}

GridMaterial* GridMaterial::Parse(const Json::value& /*source*/,
                                  Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
