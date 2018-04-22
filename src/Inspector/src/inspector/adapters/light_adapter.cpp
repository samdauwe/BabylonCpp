#include <babylon/inspector/adapters/light_adapter.h>

#include <babylon/inspector/treetools/checkbox.h>
#include <babylon/lights/light.h>

namespace BABYLON {

LightAdapter::LightAdapter(Light* light) : _light{light}
{
  _tools.emplace_back(::std::make_unique<Checkbox>(this));
}

LightAdapter::~LightAdapter()
{
}

string_t LightAdapter::id()
{
  return _light->name;
}

string_t LightAdapter::type()
{
  return _light->getClassName();
}

vector_t<unique_ptr_t<AbstractTreeTool>>& LightAdapter::getTools()
{
  return _tools;
}

void LightAdapter::setVisible(bool b)
{
  _light->setEnabled(b);
}

bool LightAdapter::isVisible() const
{
  return _light->isEnabled();
}

} // end of namespace BABYLON
