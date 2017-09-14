#include <babylon/mesh/polygonmesh/polygon_points.h>

#include <babylon/mesh/polygonmesh/indexed_vector2.h>

namespace BABYLON {

PolygonPoints::PolygonPoints()
{
}

PolygonPoints::~PolygonPoints()
{
}

vector_t<IndexedVector2>
PolygonPoints::add(const vector_t<Vector2>& originalPoints)
{
  vector_t<IndexedVector2> result;

  for (auto& point : originalPoints) {
    if (result.empty() || !point.equalsWithEpsilon(result[0])) {
      IndexedVector2 newPoint(point, static_cast<int>(elements.size()));
      result.emplace_back(newPoint);
      elements.emplace_back(newPoint);
    }
  }

  return result;
}

Bounds PolygonPoints::computeBounds()
{
  Vector2 lmin(elements[0].x, elements[0].y);
  Vector2 lmax(elements[0].x, elements[0].y);

  for (auto& point : elements) {
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
