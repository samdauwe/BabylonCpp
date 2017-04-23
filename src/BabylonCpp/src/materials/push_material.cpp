#include <babylon/materials/push_material.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/sub_mesh.h>

namespace BABYLON {

PushMaterial::PushMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}, _activeEffect{nullptr}
{
  storeEffectOnSubMeshes = true;
}

PushMaterial::~PushMaterial()
{
}

const char* PushMaterial::getClassName() const
{
  return "PushMaterial";
}

IReflect::Type PushMaterial::type() const
{
  return IReflect::Type::PUSHMATERIAL;
}

Effect* PushMaterial::getEffect()
{
  return _activeEffect;
}

bool PushMaterial::isReady(AbstractMesh* mesh, bool useInstances)
{
  if (!mesh) {
    return false;
  }

  if (mesh->subMeshes.empty()) {
    return true;
  }

  return isReadyForSubMesh(mesh, mesh->subMeshes[0].get(), useInstances);
}

void PushMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _activeEffect->setMatrix("world", world);
}

void PushMaterial::bind(Matrix* world, Mesh* mesh)
{
  if (!mesh) {
    return;
  }

  bindForSubMesh(world, mesh, mesh->subMeshes[0].get());
}

void PushMaterial::_afterBind(Mesh* mesh, Effect* effect)
{
  Material::_afterBind(mesh);
  getScene()->_cachedEffect = effect;
}

bool PushMaterial::_mustRebind(Scene* scene, Effect* effect)
{
  return scene->getCachedEffect() != effect
         || scene->getCachedMaterial() != this;
}

void PushMaterial::markAsDirty(unsigned int flag)
{
  if (flag & Material::TextureDirtyFlag) {
    _markAllSubMeshesAsTexturesDirty();
  }

  if (flag & Material::LightDirtyFlag) {
    _markAllSubMeshesAsLightsDirty();
  }

  if (flag & Material::FresnelDirtyFlag) {
    _markAllSubMeshesAsFresnelDirty();
  }

  if (flag & Material::AttributesDirtyFlag) {
    _markAllSubMeshesAsAttributesDirty();
  }

  if (flag & Material::MiscDirtyFlag) {
    _markAllSubMeshesAsMiscDirty();
  }
}

void PushMaterial::_markAllSubMeshesAsDirty(
  const std::function<void(MaterialDefines* defines)>& func)
{
  for (auto& mesh : getScene()->meshes) {
    if (mesh->subMeshes.empty()) {
      continue;
    }
    for (auto& subMesh : mesh->subMeshes) {
      if (subMesh->getMaterial() != this) {
        continue;
      }

      if (!subMesh->_materialDefines) {
        return;
      }

      func(subMesh->_materialDefines);
    }
  }
}

void PushMaterial::_markAllSubMeshesAsTexturesDirty()
{
}

void PushMaterial::_markAllSubMeshesAsFresnelDirty()
{
}

void PushMaterial::_markAllSubMeshesAsLightsDirty()
{
}

void PushMaterial::_markAllSubMeshesAsAttributesDirty()
{
}

void PushMaterial::_markAllSubMeshesAsMiscDirty()
{
}

} // end of namespace BABYLON
