#include <babylon/meshes/polygonmesh/polygon_points.h>

namespace BABYLON {

PolygonPoints::PolygonPoints() = default;

PolygonPoints::~PolygonPoints() = default;

std::vector<IndexedVector2> PolygonPoints::add(const std::vector<Vector2>& originalPoints)
{
  std::vector<IndexedVector2> result;

  for (const auto& point : originalPoints) {
    IndexedVector2 newPoint(point, static_cast<int>(elements.size()));
    result.emplace_back(newPoint);
    elements.emplace_back(newPoint);
  }

  return result;
}

Bounds PolygonPoints::computeBounds()
{
  Vector2 lmin(elements[0].x, elements[0].y);
  Vector2 lmax(elements[0].x, elements[0].y);

  for (const auto& point : elements) {
    // x
    if (point.x < lmin.x) {
      lmin.x = point.x;
    }
    else if (point.x > lmax.x) {
      lmax.x = point.x;
    }
    // y
    if (point.y < lmin.y) {
      lmin.y = point.y;
    }
    else if (point.y > lmax.y) {
      lmax.y = point.y;
    }
  }

  Bounds result;
  result.min    = lmin;
  result.max    = lmax;
  result.width  = lmax.x - lmin.x;
  result.height = lmax.y - lmin.y;

  return result;
}

} // end of namespace BABYLON
