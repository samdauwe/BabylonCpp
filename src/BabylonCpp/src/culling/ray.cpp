#include <babylon/culling/ray.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/intersection_info.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/math/plane.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

const float Ray::smallnum = 0.00000001f;
const float Ray::rayl     = 10e8f;

Ray::Ray()
    : origin{Vector3::Zero()}
    , direction{Vector3::Zero()}
    , length{0.f}
    , _vectorsSet{false}
    , _tmpRay{nullptr}
{
}

Ray::Ray(const Vector3& iOrigin, const Vector3& iDirection, float iLength)
    : origin{iOrigin}
    , direction{iDirection}
    , length{iLength}
    , _vectorsSet{false}
    , _tmpRay{nullptr}
{
}

Ray::Ray(const Ray& otherRay)
    : origin{otherRay.origin}
    , direction{otherRay.direction}
    , length{otherRay.length}
    , _vectorsSet{otherRay._vectorsSet}
    , _edge1{otherRay._edge1}
    , _edge2{otherRay._edge2}
    , _pvec{otherRay._pvec}
    , _tvec{otherRay._tvec}
    , _qvec{otherRay._qvec}
{
}

Ray::Ray(Ray&& otherRay)
{
  *this = std::move(otherRay);
}

Ray& Ray::operator=(const Ray& otherRay)
{
  if (&otherRay != this) {
    origin      = otherRay.origin;
    direction   = otherRay.direction;
    length      = otherRay.length;
    _vectorsSet = otherRay._vectorsSet;
    _edge1      = otherRay._edge1;
    _edge2      = otherRay._edge2;
    _pvec       = otherRay._pvec;
    _tvec       = otherRay._tvec;
    _qvec       = otherRay._qvec;
  }

  return *this;
}

Ray& Ray::operator=(Ray&& otherRay)
{
  if (&otherRay != this) {
    origin      = std::move(otherRay.origin);
    direction   = std::move(otherRay.direction);
    length      = std::move(otherRay.length);
    _vectorsSet = std::move(otherRay._vectorsSet);
    _edge1      = std::move(otherRay._edge1);
    _edge2      = std::move(otherRay._edge2);
    _pvec       = std::move(otherRay._pvec);
    _tvec       = std::move(otherRay._tvec);
    _qvec       = std::move(otherRay._qvec);
  }

  return *this;
}

Ray::~Ray()
{
}

std::unique_ptr<Ray> Ray::clone() const
{
  return std::make_unique<Ray>(*this);
}

std::ostream& operator<<(std::ostream& os, const Ray& ray)
{
  os << "{\"Origin\":" << ray.origin << ",\"Direction\":" << ray.direction
     << ",\"Length\":" << ray.length << "}";
  return os;
}

// Methods
bool Ray::intersectsBoxMinMax(const Vector3& minimum,
                              const Vector3& maximum) const
{
  float d        = 0.f;
  float maxValue = std::numeric_limits<float>::max();
  float inv      = 0.f;
  float min      = 0.f;
  float max      = 0.f;
  if (std::abs(direction.x) < 0.0000001f) {
    if (origin.x < minimum.x || origin.x > maximum.x) {
      return false;
    }
  }
  else {
    inv = 1.f / direction.x;
    min = (minimum.x - origin.x) * inv;
    max = (maximum.x - origin.x) * inv;
    if (stl_util::almost_equal(max, -std::numeric_limits<float>::infinity())) {
      max = std::numeric_limits<float>::infinity();
    }

    if (min > max) {
      std::swap(min, max);
    }

    d        = std::max(min, d);
    maxValue = std::min(max, maxValue);

    if (d > maxValue) {
      return false;
    }
  }

  if (std::abs(direction.y) < 0.0000001f) {
    if (origin.y < minimum.y || origin.y > maximum.y) {
      return false;
    }
  }
  else {
    inv = 1.f / direction.y;
    min = (minimum.y - origin.y) * inv;
    max = (maximum.y - origin.y) * inv;

    if (stl_util::almost_equal(max, -std::numeric_limits<float>::infinity())) {
      max = std::numeric_limits<float>::infinity();
    }

    if (min > max) {
      std::swap(min, max);
    }

    d        = std::max(min, d);
    maxValue = std::min(max, maxValue);

    if (d > maxValue) {
      return false;
    }
  }

  if (std::abs(direction.z) < 0.0000001f) {
    if (origin.z < minimum.z || origin.z > maximum.z) {
      return false;
    }
  }
  else {
    inv = 1.f / direction.z;
    min = (minimum.z - origin.z) * inv;
    max = (maximum.z - origin.z) * inv;

    if (stl_util::almost_equal(max, -std::numeric_limits<float>::infinity())) {
      max = std::numeric_limits<float>::infinity();
    }

    if (min > max) {
      std::swap(min, max);
    }

    d        = std::max(min, d);
    maxValue = std::min(max, maxValue);

    if (d > maxValue) {
      return false;
    }
  }
  return true;
}

bool Ray::intersectsBox(const BoundingBox& box) const
{
  return intersectsBoxMinMax(box.minimum, box.maximum);
}

bool Ray::intersectsSphere(const BoundingSphere& sphere) const
{
  float x    = sphere.center.x - origin.x;
  float y    = sphere.center.y - origin.y;
  float z    = sphere.center.z - origin.z;
  float pyth = (x * x) + (y * y) + (z * z);
  float rr   = sphere.radius * sphere.radius;

  if (pyth <= rr) {
    return true;
  }

  float dot = (x * direction.x) + (y * direction.y) + (z * direction.z);
  if (dot < 0.f) {
    return false;
  }

  float temp = pyth - (dot * dot);

  return temp <= rr;
}

std::optional<IntersectionInfo> Ray::intersectsTriangle(const Vector3& vertex0,
                                                        const Vector3& vertex1,
                                                        const Vector3& vertex2)
{
  if (!_vectorsSet) {
    _vectorsSet = true;
    _edge1      = Vector3::Zero();
    _edge2      = Vector3::Zero();
    _pvec       = Vector3::Zero();
    _tvec       = Vector3::Zero();
    _qvec       = Vector3::Zero();
  }

  vertex1.subtractToRef(vertex0, _edge1);
  vertex2.subtractToRef(vertex0, _edge2);
  Vector3::CrossToRef(direction, _edge2, _pvec);
  float det = Vector3::Dot(_edge1, _pvec);

  if (stl_util::almost_equal(det, 0.f)) {
    return std::nullopt;
  }

  float invdet = 1.f / det;

  origin.subtractToRef(vertex0, _tvec);

  float bu = Vector3::Dot(_tvec, _pvec) * invdet;

  if (bu < 0.f || bu > 1.f) {
    return std::nullopt;
  }

  Vector3::CrossToRef(_tvec, _edge1, _qvec);

  float bv = Vector3::Dot(direction, _qvec) * invdet;

  if (bv < 0.f || bu + bv > 1.f) {
    return std::nullopt;
  }

  // check if the distance is longer than the predefined length.
  float distance = Vector3::Dot(_edge2, _qvec) * invdet;
  if (distance > length) {
    return std::nullopt;
  }

  return IntersectionInfo(bu, bv, distance);
}

std::optional<float> Ray::intersectsPlane(const Plane& plane)
{
  float distance;
  float result1 = Vector3::Dot(plane.normal, direction);
  if (std::abs(result1) < 9.99999997475243E-07f) {
    return std::nullopt;
  }
  else {
    const float result2 = Vector3::Dot(plane.normal, origin);
    distance            = (-plane.d - result2) / result1;
    if (distance < 0.f) {
      if (distance < -9.99999997475243E-07f) {
        return std::nullopt;
      }
      else {
        return 0.f;
      }
    }

    return distance;
  }
}

PickingInfo Ray::intersectsMesh(AbstractMesh* mesh, bool fastCheck)
{
  auto& tm = Tmp::MatrixArray[0];

  mesh->getWorldMatrix().invertToRef(tm);

  if (_tmpRay) {
    Ray::TransformToRef(*this, tm, *_tmpRay);
  }
  else {
    _tmpRay = std::make_unique<Ray>(Ray::Transform(*this, tm));
  }

  return mesh->intersects(*_tmpRay, fastCheck);
}

std::vector<PickingInfo>
Ray::intersectsMeshes(std::vector<AbstractMesh*>& meshes, bool fastCheck,
                      std::vector<PickingInfo>& results)
{
  for (auto& mesh : meshes) {
    auto pickInfo = intersectsMesh(mesh, fastCheck);

    if (pickInfo.hit) {
      results.emplace_back(pickInfo);
    }
  }

  std::sort(results.begin(), results.end(), _comparePickingInfo);

  return results;
}

int Ray::_comparePickingInfo(const PickingInfo& pickingInfoA,
                             const PickingInfo& pickingInfoB)
{
  if (pickingInfoA.distance < pickingInfoB.distance) {
    return -1;
  }
  else if (pickingInfoA.distance > pickingInfoB.distance) {
    return 1;
  }
  else {
    return 0;
  }
}

float Ray::intersectionSegment(const Vector3& sega, const Vector3& segb,
                               float threshold) const
{
  auto rsegb
    = origin.add(direction.multiplyByFloats(Ray::rayl, Ray::rayl, Ray::rayl));

  auto u   = segb.subtract(sega);
  auto v   = rsegb.subtract(origin);
  auto w   = sega.subtract(origin);
  float a  = Vector3::Dot(u, u); // always >= 0
  float b  = Vector3::Dot(u, v);
  float c  = Vector3::Dot(v, v); // always >= 0
  float d  = Vector3::Dot(u, w);
  float e  = Vector3::Dot(v, w);
  float D  = a * c - b * b;         // always >= 0
  float sc = 0.f, sN = 0.f, sD = D; // sc = sN / sD, default sD = D >= 0
  float tc = 0.f, tN = 0.f, tD = D; // tc = tN / tD, default tD = D >= 0

  // compute the line parameters of the two closest points
  if (D < Ray::smallnum) { // the lines are almost parallel
    sN = 0.f;              // force using point P0 on segment S1
    sD = 1.f;              // to prevent possible division by 0.0 later
    tN = e;
    tD = c;
  }
  else { // get the closest points on the infinite lines
    sN = (b * e - c * d);
    tN = (a * e - b * d);
    if (sN < 0.f) { // sc < 0 => the s=0 edge is visible
      sN = 0.f;
      tN = e;
      tD = c;
    }
    else if (sN > sD) { // sc > 1 => the s=1 edge is visible
      sN = sD;
      tN = e + b;
      tD = c;
    }
  }

  if (tN < 0.f) { // tc < 0 => the t=0 edge is visible
    tN = 0.f;
    // recompute sc for this edge
    if (-d < 0.f) {
      sN = 0.f;
    }
    else if (-d > a) {
      sN = sD;
    }
    else {
      sN = -d;
      sD = a;
    }
  }
  else if (tN > tD) { // tc > 1 => the t=1 edge is visible
    tN = tD;
    // recompute sc for this edge
    if ((-d + b) < 0.f) {
      sN = 0.f;
    }
    else if ((-d + b) > a) {
      sN = sD;
    }
    else {
      sN = (-d + b);
      sD = a;
    }
  }
  // finally do the division to get sc and tc
  sc = (std::abs(sN) < Ray::smallnum ? 0.f : sN / sD);
  tc = (std::abs(tN) < Ray::smallnum ? 0.f : tN / tD);

  // get the difference of the two closest points
  auto qtc = v.multiplyByFloats(tc, tc, tc);
  auto dP
    = w.add(u.multiplyByFloats(sc, sc, sc)).subtract(qtc); // = S1(sc) - S2(tc)

  // return intersection result
  bool isIntersected = (tc > 0) && (tc <= length)
                       && (dP.lengthSquared() < (threshold * threshold));

  if (isIntersected) {
    return qtc.length();
  }
  return -1;
}

Ray& Ray::update(float x, float y, float viewportWidth, float viewportHeight,
                 Matrix& world, Matrix& view, Matrix& projection)
{
  Vector3::UnprojectFloatsToRef(x, y, 0.f, viewportWidth, viewportHeight, world,
                                view, projection, origin);
  Vector3::UnprojectFloatsToRef(x, y, 1.f, viewportWidth, viewportHeight, world,
                                view, projection, Tmp::Vector3Array[0]);

  Tmp::Vector3Array[0].subtractToRef(origin, direction);
  direction.normalize();
  return *this;
}

Ray Ray::Zero()
{
  return Ray(Vector3::Zero(), Vector3::Zero());
}

Ray Ray::CreateNew(float x, float y, float viewportWidth, float viewportHeight,
                   Matrix& world, Matrix& view, Matrix& projection)
{
  auto result = Ray::Zero();

  return result.update(x, y, viewportWidth, viewportHeight, world, view,
                       projection);
}

Ray Ray::CreateNewFromTo(const Vector3& origin, const Vector3& end,
                         const Matrix& world)
{
  auto direction = end.subtract(origin);
  float length
    = std::sqrt((direction.x * direction.x) + (direction.y * direction.y)
                + (direction.z * direction.z));
  direction.normalize();

  return Ray::Transform(Ray(origin, direction, length), world);
}

Ray Ray::Transform(const Ray& ray, const Matrix& matrix)
{
  Ray result(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f));
  Ray::TransformToRef(ray, matrix, result);

  return result;
}

void Ray::TransformToRef(const Ray& ray, const Matrix& matrix, Ray& result)
{
  Vector3::TransformCoordinatesToRef(ray.origin, matrix, result.origin);
  Vector3::TransformNormalToRef(ray.direction, matrix, result.direction);
  result.length = ray.length;

  auto& dir = result.direction;
  auto len  = dir.length();

  if (!(len == 0.f || len == 1.f)) {
    float num = 1.f / len;
    dir.x *= num;
    dir.y *= num;
    dir.z *= num;
    result.length *= len;
  }
}

} // end of namespace BABYLON
