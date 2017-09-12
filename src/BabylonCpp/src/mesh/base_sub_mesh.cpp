#include <babylon/mesh/base_sub_mesh.h>

#include <babylon/materials/material_defines.h>

namespace BABYLON {

BaseSubMesh::BaseSubMesh() : _materialDefines{nullptr}, _materialEffect{nullptr}
{
}

BaseSubMesh::~BaseSubMesh()
{
}

Effect* BaseSubMesh::effect()
{
  return _materialEffect;
}

void BaseSubMesh::setEffect(Effect* effect)
{
  if (_materialEffect == effect) {
    if (!effect) {
      _materialDefines = nullptr;
    }
    return;
  }
  _materialDefines = nullptr;
  _materialEffect  = effect;
}

void BaseSubMesh::setEffect(Effect* effect, const MaterialDefines& defines)
{
  if (_materialEffect == effect) {
    if (!effect) {
      _materialDefines = nullptr;
    }
    return;
  }
  _materialDefines = ::std::make_unique<MaterialDefines>(defines);
  _materialEffect  = effect;
}

} // end of namespace BABYLON
