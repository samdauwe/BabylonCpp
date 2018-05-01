#include <babylon/inspector/properties/float_property.h>

#include <babylon/mesh/geometry.h>

#include <imgui.h>

namespace BABYLON {

FloatProperty::FloatProperty(const TFloatGetter& getter,
                             const TFloatSetter& setter)
    : _id{Geometry::RandomId()}, _getter{getter}, _setter{setter}
{
}

FloatProperty::FloatProperty(FloatProperty&& other)
    : _id{::std::move(other._id)}
    , _getter{::std::move(other._getter)}
    , _setter{::std::move(other._setter)}
{
}

FloatProperty::~FloatProperty()
{
}

void FloatProperty::render()
{
  float floatValue = _getter();
  if (ImGui::InputFloat("", &floatValue)) {
    _setter(floatValue);
  }
}

void FloatProperty::dispose()
{
}

} // end of namespace BABYLON
