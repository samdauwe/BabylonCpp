#include <cmath>
#include <babylon/extensions/hexplanetgeneration/terrain/tile.h>

#include <sstream>

#include <babylon/culling/ray.h>
#include <babylon/extensions/hexplanetgeneration/terrain/corner.h>
#include <babylon/extensions/hexplanetgeneration/utils/tools.h>
#include <babylon/math/plane.h>

namespace BABYLON {
namespace Extensions {

Tile::Tile(size_t p_id, const Vector3& p_position, size_t cornerCount, size_t borderCount,
           size_t tileCount)
    : UidObj{p_id}
    , position{p_position}
    , boundingSphere{Vector3::Zero(), Vector3::Zero()}
    , area{0}
    , elevation{0}
    , plate{nullptr}
    , temperature{0}
    , moisture{0}

{
  corners.resize(cornerCount);
  borders.resize(borderCount);
  tiles.resize(tileCount);
}

bool Tile::intersectRay(const Ray& ray)
{
  if (!Tools::intersectRayWithSphere(ray, boundingSphere)) {
    return false;
  }

  Plane surface = Tools::redefinedPlane(normal, averagePosition);
  if (surface.signedDistanceTo(ray.origin) <= 0) {
    return false;
  }

  auto denominator = Vector3::Dot(surface.normal, ray.direction);
  if (denominator == 0.f) {
    return false;
  }

  auto t = -(Vector3::Dot(ray.origin, surface.normal)
             - std::abs(surface.signedDistanceTo(Vector3::Zero())))
           / denominator;
  auto point = ray.direction * t + ray.origin;

  auto origin = Vector3::Zero();
  for (size_t i = 0; i < corners.size(); ++i) {
    auto j     = (i + 1) % corners.size();
    Plane side = Tools::redefinedPlane(corners[j]->position,
                                       corners[i]->position, origin);
    if (side.signedDistanceTo(point) < 0.f)
      return false;
  }

  return true;
}

std::string Tile::toString()
{
  std::ostringstream oss;
  oss << "Tile " << id << " (" << tiles.size() << " Neighbors) < " << position.x
      << ", " << position.y << ", " << position.z << " >";
  return oss.str();
}

} // end of namespace Extensions
} // end of namespace BABYLON
