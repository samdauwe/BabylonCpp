#include <babylon/inspector/properties/bool_property.h>

#include <babylon/mesh/geometry.h>

#include <imgui.h>

namespace BABYLON {

BoolProperty::BoolProperty(const TBoolGetter& getter, const TBoolSetter& setter)
    : _id{Geometry::RandomId()}, _getter{getter}, _setter{setter}
{
}

BoolProperty::BoolProperty(BoolProperty&& other)
    : _id{::std::move(other._id)}
    , _getter{::std::move(other._getter)}
    , _setter{::std::move(other._setter)}
{
}

BoolProperty::~BoolProperty()
{
}

void BoolProperty::render()
{
  bool boolValue = _getter();
  if (ImGui::Checkbox("", &boolValue)) {
    _setter(boolValue);
  }
}

void BoolProperty::dispose()
{
}

} // end of namespace BABYLON
