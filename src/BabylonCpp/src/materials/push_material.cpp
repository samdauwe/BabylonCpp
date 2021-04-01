#include <babylon/materials/push_material.h>

#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_defines.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/sub_mesh.h>

namespace BABYLON {

PushMaterial::PushMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}, _activeEffect{nullptr}
{
  _storeEffectOnSubMeshes = true;
}

PushMaterial::~PushMaterial() = default;

std::string PushMaterial::getClassName() const
{
  return "PushMaterial";
}

Type PushMaterial::type() const
{
  return Type::PUSHMATERIAL;
}

EffectPtr& PushMaterial::getEffect()
{
  return _activeEffect;
}

bool PushMaterial::isReady(AbstractMesh* mesh, bool useInstances, SubMesh* /*subMesh*/)
{
  if (!mesh) {
    return false;
  }

  if (mesh->subMeshes.empty()) {
    return true;
  }

  return isReadyForSubMesh(mesh, mesh->subMeshes[0].get(), useInstances);
}

bool PushMaterial::_isReadyForSubMesh(SubMesh* subMesh)
{
  const auto& defines = subMesh->_materialDefines;
  if (!checkReadyOnEveryCall && subMesh->effect() && defines) {
    if (defines->_renderId == getScene()->getRenderId()) {
      return true;
    }
  }

  return false;
}

void PushMaterial::bindOnlyWorldMatrix(Matrix& world, const EffectPtr& /*effectOverride*/)
{
  _activeEffect->setMatrix("world", world);
}

void PushMaterial::bindOnlyNormalMatrix(Matrix& normalMatrix)
{
  _activeEffect->setMatrix("normalMatrix", normalMatrix);
}

void PushMaterial::bind(Matrix& world, Mesh* mesh, const EffectPtr& /*effectOverride*/)
{
  if (!mesh) {
    return;
  }

  bindForSubMesh(world, mesh, mesh->subMeshes[0].get());
}

void PushMaterial::_afterBind(Mesh* mesh, const EffectPtr& effect)
{
  Material::_afterBind(mesh, effect);
  getScene()->_cachedEffect = effect;
}

bool PushMaterial::_mustRebind(Scene* scene, const EffectPtr& effect, float visibility)
{
  return scene->isCachedMaterialInvalid(this, effect, visibility);
}

} // end of namespace BABYLON
