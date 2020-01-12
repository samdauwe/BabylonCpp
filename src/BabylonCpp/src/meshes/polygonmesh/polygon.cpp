#include <babylon/meshes/polygonmesh/polygon.h>

#include <babylon/maths/path2.h>
#include <babylon/maths/vector2.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

std::vector<Vector2> Polygon::Rectangle(float xmin, float ymin, float xmax, float ymax)
{
  return {
    Vector2(xmin, ymin), //
    Vector2(xmax, ymin), //
    Vector2(xmax, ymax), //
    Vector2(xmin, ymax)  //
  };
}

std::vector<Vector2> Polygon::Circle(float radius, float cx, float cy, size_t numberOfSides)
{
  std::vector<Vector2> result;

  float angle     = 0.f;
  float increment = Math::PI2 / static_cast<float>(numberOfSides);

  for (size_t i = 0; i < numberOfSides; ++i) {
    result.emplace_back(Vector2(cx + std::cos(angle) * radius, cy + std::sin(angle) * radius));
    angle -= increment;
  }

  return result;
}

std::vector<Vector2> Polygon::Parse(const std::string& input)
{
  std::vector<Vector2> result;
  std::string inputCpy = input;
  StringTools::replaceInPlace(inputCpy, "\\", "");
  StringTools::replaceInPlace(inputCpy, "  ", " ");
  std::vector<std::string> floats = StringTools::split(inputCpy, ' ');
  size_t floatsSize               = floats.size() - (floats.size() % 2);
  result.reserve(floatsSize / 2);
  for (size_t i = 0; i < floatsSize; i += 2) {
    result.emplace_back(Vector2(std::stof(floats[i]), std::stof(floats[i + 1])));
  }

  return result;
}

Path2 Polygon::StartingAt(float x, float y)
{
  return Path2::StartingAt(x, y);
}

} // end of namespace BABYLON
