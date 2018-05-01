#include <babylon/inspector/properties/color3_property.h>

#include <babylon/math/color3.h>
#include <babylon/mesh/geometry.h>

#include <imgui.h>

namespace BABYLON {

Color3Property::Color3Property(const TColor3Getter& getter,
                               const TColor3Setter& setter)
    : _id{Geometry::RandomId()}, _getter{getter}, _setter{setter}
{
}

Color3Property::Color3Property(Color3Property&& other)
    : _id{::std::move(other._id)}
    , _getter{::std::move(other._getter)}
    , _setter{::std::move(other._setter)}
{
}

Color3Property::~Color3Property()
{
}

void Color3Property::render()
{
  const auto& color = _getter();
  _colorArray       = {{color.r, color.g, color.b}};
  if (ImGui::ColorEdit3("", _colorArray.data())) {
    _setter(Color3{_colorArray[0], _colorArray[1], _colorArray[2]});
  }
}

void Color3Property::dispose()
{
}

} // end of namespace BABYLON
