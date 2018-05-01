#include <babylon/inspector/adapters/material_adapter.h>

#include <babylon/materials/material.h>

#include <babylon/inspector/properties/properties_view.h>

namespace BABYLON {

MaterialAdapter::MaterialAdapter(Material* material)
    : _material{material}, _properties{nullptr}
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

unique_ptr_t<PropertiesView>& MaterialAdapter::getProperties()
{
  return _properties;
}

vector_t<unique_ptr_t<AbstractTreeTool>>& MaterialAdapter::getTools()
{
  return _tools;
}

} // end of namespace BABYLON
