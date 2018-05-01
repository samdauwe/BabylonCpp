#include <babylon/inspector/properties/string_property.h>

#include <babylon/mesh/geometry.h>

#include <imgui.h>

namespace BABYLON {

StringProperty::StringProperty(const TStringGetter& getter,
                               const TStringSetter& setter)
    : _id{Geometry::RandomId()}, _getter{getter}, _setter{setter}
{
}

StringProperty::StringProperty(StringProperty&& other)
    : _id{::std::move(other._id)}
    , _getter{::std::move(other._getter)}
    , _setter{::std::move(other._setter)}
{
}

StringProperty::~StringProperty()
{
}

void StringProperty::render()
{
  string_t stringValue = _getter();
  char* cstr           = &stringValue[0u];
  if (ImGui::InputText("", cstr, stringValue.size())) {
  }
}

void StringProperty::dispose()
{
}

} // end of namespace BABYLON
