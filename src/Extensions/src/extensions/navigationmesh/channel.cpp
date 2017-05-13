#include <babylon/extensions/navigationmesh/channel.h>

namespace BABYLON {
namespace Extensions {

Channel::Channel()
{
}

Channel::~Channel()
{
}

void Channel::push(const Vector3& p)
{
  portals.emplace_back(Portal{p, p});
}

void Channel::push(const Vector3& p1, const Vector3& p2)
{
  portals.emplace_back(Portal{p1, p2});
}

bool Channel::_vequal(const Vector3& a, const Vector3& b)
{
  return Vector3::DistanceSquared(a, b) < 0.00001f;
}

float Channel::_triarea2(const Vector3& a, const Vector3& b, const Vector3& c)
{
  const float ax = b.x - a.x;
  const float az = b.z - a.z;
  const float bx = c.x - a.x;
  const float bz = c.z - a.z;
  return bx * az - ax * bz;
}

std::vector<Vector3>& Channel::stringPull()
{
  std::vector<Vector3> pts;
  // Init scan state
  size_t apexIndex = 0, leftIndex = 0, rightIndex = 0;

  auto& portalApex  = portals[0].left;
  auto& portalLeft  = portals[0].left;
  auto& portalRight = portals[0].right;

  // Add start point.
  pts.emplace_back(portalApex);

  for (size_t i = 1; i < portals.size(); ++i) {
    const auto& left  = portals[i].left;
    const auto& right = portals[i].right;

    // Update right vertex.
    if (_triarea2(portalApex, portalRight, right) >= 0.f) {
      if (_vequal(portalApex, portalRight)
          || _triarea2(portalApex, portalLeft, right) < 0.f) {
        // Tighten the funnel.
        portalRight = right;
        rightIndex  = i;
      }
      else {
        // Right over left, insert left to path and restart scan from portal
        // left point.
        pts.emplace_back(portalLeft);
        // Make current left the new apex.
        portalApex = portalLeft;
        apexIndex  = leftIndex;
        // Reset portal
        portalLeft  = portalApex;
        portalRight = portalApex;
        leftIndex   = apexIndex;
        rightIndex  = apexIndex;
        // Restart scan
        i = apexIndex;
        continue;
      }
    }

    // Update left vertex.
    if (_triarea2(portalApex, portalLeft, left) <= 0.f) {
      if (_vequal(portalApex, portalLeft)
          || _triarea2(portalApex, portalRight, left) > 0.f) {
        // Tighten the funnel.
        portalLeft = left;
        leftIndex  = i;
      }
      else {
        // Left over right, insert right to path and restart scan from portal
        // right point.
        pts.emplace_back(portalRight);
        // Make current right the new apex.
        portalApex = portalRight;
        apexIndex  = rightIndex;
        // Reset portal
        portalLeft  = portalApex;
        portalRight = portalApex;
        leftIndex   = apexIndex;
        rightIndex  = apexIndex;
        // Restart scan
        i = apexIndex;
        continue;
      }
    }
  }

  if ((pts.empty()) || (!_vequal(pts.back(), portals.back().left))) {
    // Append last point to path.
    pts.emplace_back(portals.back().left);
  }

  path = std::move(pts);
  return path;
}

} // end of namespace Extensions
} // end of namespace BABYLON
