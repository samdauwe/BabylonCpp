#include <babylon/materialslibrary/grid/grid_material.h>

#include <nlohmann/json.hpp>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materialslibrary/grid/grid_fragment_fx.h>
#include <babylon/materialslibrary/grid/grid_vertex_fx.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

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
    , opacityTexture{this, &GridMaterial::get_opacityTexture, &GridMaterial::set_opacityTexture}
    , _opacityTexture{nullptr}
    , _gridControl{Vector4(gridRatio, majorUnitFrequency, minorUnitVisibility, opacity)}
{
  // Vertex shader
  Effect::ShadersStore()["gridVertexShader"] = gridVertexShader;

  // Fragment shader
  Effect::ShadersStore()["gridPixelShader"] = gridPixelShader;
}

GridMaterial::~GridMaterial() = default;

BaseTexturePtr& GridMaterial::get_opacityTexture()
{
  return _opacityTexture;
}

void GridMaterial::set_opacityTexture(const BaseTexturePtr& value)
{
  if (_opacityTexture != value) {
    _opacityTexture = value;
    _markAllSubMeshesAsTexturesDirty();
  }
}

bool GridMaterial::needAlphaBlending() const
{
  return opacity < 1.f || (_opacityTexture && _opacityTexture->isReady());
}

bool GridMaterial::needAlphaBlendingForMesh(const AbstractMesh& /*mesh*/) const
{
  return needAlphaBlending();
}

bool GridMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh, bool useInstances)
{
  if (isFrozen()) {
    if (subMesh->effect() && subMesh->effect()->_wasPreviouslyReady) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<GridMaterialDefines>();
  }

  auto definesPtr = std::static_pointer_cast<GridMaterialDefines>(subMesh->_materialDefines);
  auto& defines   = *definesPtr.get();
  auto scene      = getScene();

  if (_isReadyForSubMesh(subMesh)) {
    return true;
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

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (_opacityTexture && MaterialFlags::OpacityTextureEnabled()) {
        if (!_opacityTexture->isReady()) {
          return false;
        }
        else {
          defines._needUVs           = true;
          defines.boolDef["OPACITY"] = true;
        }
      }
    }
  }

  MaterialHelper::PrepareDefinesForMisc(mesh, scene, false, false, fogEnabled(), false, defines);

  // Values that need to be evaluated on every frame
  MaterialHelper::PrepareDefinesForFrameBoundValues(scene, scene->getEngine(), defines,
                                                    useInstances);

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Attributes
    MaterialHelper::PrepareDefinesForAttributes(mesh, defines, false, false);
    std::vector<std::string> attribs{VertexBuffer::PositionKind, VertexBuffer::NormalKind};

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKind);
    }
    if (defines["UV2"]) {
      attribs.emplace_back(VertexBuffer::UV2Kind);
    }

    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    // Defines
    const std::string join = defines.toString();

    // Uniforms
    const std::vector<std::string> uniforms{
      "projection", "mainColor", "lineColor", "gridControl",   "gridOffset",   "vFogInfos",
      "vFogColor",  "world",     "view",      "opacityMatrix", "vOpacityInfos"};

    // Samplers
    const std::vector<std::string> samplers{"opacitySampler"};

    IEffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.maxSimultaneousLights = 4;
    options.fallbacks             = nullptr;
    options.onCompiled            = onCompiled;
    options.onError               = onError;

    subMesh->setEffect(scene->getEngine()->createEffect("grid", options, engine), definesPtr);
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  defines._renderId                      = scene->getRenderId();
  subMesh->effect()->_wasPreviouslyReady = true;

  return true;
}

void GridMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto defines = static_cast<GridMaterialDefines*>(subMesh->_materialDefines.get());
  if (!defines) {
    return;
  }

  auto effect = subMesh->effect();
  if (!effect) {
    return;
  }
  _activeEffect = effect;

  // Matrices
  if (!(*defines)["INSTANCES"] || (*defines)["THIN_INSTANCE"]) {
    bindOnlyWorldMatrix(world);
  }
  _activeEffect->setMatrix("worldView", world.multiply(scene->getViewMatrix()));
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

    if (_opacityTexture && MaterialFlags::OpacityTextureEnabled()) {
      _activeEffect->setTexture("opacitySampler", _opacityTexture);
      _activeEffect->setFloat2("vOpacityInfos",
                               static_cast<float>(_opacityTexture->coordinatesIndex),
                               _opacityTexture->level);
      _activeEffect->setMatrix("opacityMatrix", *_opacityTexture->getTextureMatrix());
    }
  }
  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  _afterBind(mesh, _activeEffect);
}

void GridMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                           bool /*notBoundToMesh*/)
{
  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr GridMaterial::clone(const std::string& /*name*/, bool /*cloneChildren*/) const
{
  return nullptr;
}

json GridMaterial::serialize() const
{
  return nullptr;
}

std::string GridMaterial::getClassName() const
{
  return "GridMaterial";
}

GridMaterial* GridMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
