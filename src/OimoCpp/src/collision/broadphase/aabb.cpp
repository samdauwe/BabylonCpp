#include <oimo/collision/broadphase/aabb.h>

#include <cmath>
#include <limits>
#include <utility>

#include <oimo/math/vec3.h>

namespace OIMO {

const float AABB::AABB_PROX = 0.005f;

AABB::AABB(float minX, float maxX, float minY, float maxY, float minZ,
           float maxZ)
{
  elements[0] = minX;
  elements[3] = maxX;
  elements[1] = minY;
  elements[4] = maxY;
  elements[2] = minZ;
  elements[5] = maxZ;
}

AABB::AABB(const AABB& aabb)
{
  elements = aabb.elements;
}

AABB::AABB(AABB&& aabb)
{
  *this = std::move(aabb);
}

AABB::~AABB()
{
}

AABB& AABB::operator=(const AABB& aabb)
{
  if (&aabb != this) {
    elements = aabb.elements;
  }

  return *this;
}

AABB& AABB::operator=(AABB&& aabb)
{
  if (&aabb != this) {
    std::swap(elements, aabb.elements);
  }

  return *this;
}

AABB AABB::clone() const
{
  return AABB(*this);
}

AABB* AABB::cloneToNewObject() const
{
  return new AABB(*this);
}

AABB& AABB::set(float minX, float maxX, float minY, float maxY, float minZ,
                float maxZ)
{
  elements[0] = minX;
  elements[3] = maxX;
  elements[1] = minY;
  elements[4] = maxY;
  elements[2] = minZ;
  elements[5] = maxZ;

  return *this;
}

bool AABB::intersectTest(const AABB& aabb) const
{
  return elements[0] > aabb.elements[3] || elements[1] > aabb.elements[4]
         || elements[2] > aabb.elements[5] || elements[3] < aabb.elements[0]
         || elements[4] < aabb.elements[1] || elements[5] < aabb.elements[2];
}

bool AABB::intersectTestTwo(const AABB& aabb) const
{
  return elements[0] < aabb.elements[0] || elements[1] < aabb.elements[1]
         || elements[2] < aabb.elements[2] || elements[3] > aabb.elements[3]
         || elements[4] > aabb.elements[4] || elements[5] > aabb.elements[5];
}

AABB& AABB::copy(const AABB& aabb, float margin)
{
  const std::array<float, 6>& me = aabb.elements;
  set(me[0] - margin, me[3] + margin, me[1] - margin, me[4] + margin,
      me[2] - margin, me[5] + margin);
  return *this;
}

AABB& AABB::fromArray(const std::array<float, 6>& array)
{
  set(array[0], array[1], array[2], array[3], array[4], array[5]);
  return *this;
}

AABB& AABB::combine(const AABB& aabb1, const AABB& aabb2)
{
  const std::array<float, 6>& a = aabb1.elements;
  const std::array<float, 6>& b = aabb2.elements;

  elements[0] = a[0] < b[0] ? a[0] : b[0];
  elements[1] = a[1] < b[1] ? a[1] : b[1];
  elements[2] = a[2] < b[2] ? a[2] : b[2];

  elements[3] = a[3] > b[3] ? a[3] : b[3];
  elements[4] = a[4] > b[4] ? a[4] : b[4];
  elements[5] = a[5] > b[5] ? a[5] : b[5];

  return *this;
}

float AABB::surfaceArea() const
{
  float a = elements[3] - elements[0];
  float h = elements[4] - elements[1];
  float d = elements[5] - elements[2];
  return 2.f * (a * (h + d) + h * d);
}

bool AABB::intersectsWithPoint(float x, float y, float z) const
{
  return x >= elements[0] && x <= elements[3] && y >= elements[1]
         && y <= elements[4] && z >= elements[2] && z <= elements[5];
}

void AABB::setFromPoints(const std::vector<Vertex>& arr)
{
  makeEmpty();
  for (auto& vertex : arr) {
    expandByPoint(Vec3(vertex.x, vertex.y, vertex.z));
  }
}

void AABB::setFromPoints(const std::vector<Vec3>& arr)
{
  makeEmpty();
  for (auto& vec3 : arr) {
    expandByPoint(vec3);
  }
}

void AABB::setFromPoints(const std::array<Vertex, 4>& arr)
{
  makeEmpty();
  for (auto& vertex : arr) {
    expandByPoint(Vec3(vertex.x, vertex.y, vertex.z));
  }
}

void AABB::setFromPoints(const std::array<Vec3, 4>& arr)
{
  makeEmpty();
  for (auto& vec3 : arr) {
    expandByPoint(vec3);
  }
}

void AABB::makeEmpty()
{
  set(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
      std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
}

void AABB::expandByPoint(const Vec3& pt)
{
  set(std::min(elements[0], pt.x), std::min(elements[1], pt.y),
      std::min(elements[2], pt.z), std::max(elements[3], pt.x),
      std::max(elements[4], pt.y), std::max(elements[5], pt.z));
}

void AABB::expandByScalar(float s)
{
  elements[0] += -s;
  elements[1] += -s;
  elements[2] += -s;
  elements[3] += s;
  elements[4] += s;
  elements[5] += s;
}

} // end of namespace OIMO
