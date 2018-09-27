#include <babylon/inspector/adapters/adapter.h>

#include <babylon/mesh/geometry.h>

namespace BABYLON {

std::string Adapter::_name = Geometry::RandomId();

Adapter::Adapter() : name{this, &Adapter::get_name}
{
}

Adapter::~Adapter()
{
}

std::string Adapter::get_name() const
{
  return Adapter::_name;
}

} // end of namespace BABYLON
