#include <babylon/inspector/adapters/adapter.h>

#include <babylon/mesh/geometry.h>

namespace BABYLON {

string_t Adapter::_name = Geometry::RandomId();

Adapter::Adapter() : name{this, &Adapter::get_name}
{
}

Adapter::~Adapter()
{
}

string_t Adapter::get_name() const
{
  return Adapter::_name;
}

} // end of namespace BABYLON
