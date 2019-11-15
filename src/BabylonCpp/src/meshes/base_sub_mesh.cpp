#include <babylon/meshes/base_sub_mesh.h>

#include <babylon/materials/material_defines.h>

namespace BABYLON {

BaseSubMesh::BaseSubMesh()
    : _materialDefines{nullptr}
    , _materialEffect{nullptr}
    , effect{this, &BaseSubMesh::get_effect}
{
}

BaseSubMesh::~BaseSubMesh() = default;

EffectPtr& BaseSubMesh::get_effect()
{
  return _materialEffect;
}

void BaseSubMesh::setEffect(const EffectPtr& iEffect,
                            const MaterialDefinesPtr& defines)
{
  if (_materialEffect == iEffect) {
    if (!iEffect) {
      _materialDefines = nullptr;
    }
    return;
  }
  _materialDefines = defines;
  _materialEffect  = iEffect;
}

} // end of namespace BABYLON
