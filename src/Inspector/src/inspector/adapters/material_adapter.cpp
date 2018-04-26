#include <babylon/inspector/adapters/material_adapter.h>

#include <babylon/materials/material.h>

namespace BABYLON {

MaterialAdapter::MaterialAdapter(Material* material) : _material{material}
{
}

MaterialAdapter::~MaterialAdapter()
{
}

string_t MaterialAdapter::id()
{
  return _material->name;
}

string_t MaterialAdapter::type()
{
  return _material->getClassName();
}

vector_t<unique_ptr_t<AbstractTreeTool>>& MaterialAdapter::getTools()
{
  return _tools;
}

} // end of namespace BABYLON
