#include <babylon/culling/ray.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/intersection_info.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/math/plane.h>
#include <babylon/math/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

std::array<Vector3, 6> Ray::TmpVector3{Vector3::Zero(), Vector3::Zero(),
                                       Vector3::Zero(), Vector3::Zero(),
                                       Vector3::Zero(), Vector3::Zero()};

const float Ray::smallnum = 0.00000001f;
const float Ray::rayl     = 10e8f;

Ray::Ray()
    : origin{Vector3::Zero()}
    , direction{Vector3::Zero()}
    , length{0.f}
    , _tmpRay{nullptr}
{
}

Ray::Ray(const Vector3& iOrigin, const Vector3& iDirection, float iLength)
    : origin{iOrigin}, direction{iDirection}, length{iLength}, _tmpRay{nullptr}
{
}

Ray::Ray(const Ray& otherRay)
    : origin{otherRay.origin}
    , direction{otherRay.direction}
    , length{otherRay.length}
{
}

Ray::Ray(Ray&& otherRay)
{
  *this = std::move(otherRay);
}

Ray& Ray::operator=(const Ray& otherRay)
{
  if (&otherRay != this) {
    origin    = otherRay.origin;
    direction = otherRay.direction;
    length    = otherRay.length;
  }

  return *this;
}

Ray& Ray::operator=(Ray&& otherRay)
{
  if (&otherRay != this) {
    origin    = std::move(otherRay.origin);
    direction = std::move(otherRay.direction);
    length    = std::move(otherRay.length);
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

bool Ray::intersectsBoxMinMax(const Vector3& minimum, const Vector3& maximum,
                              float intersectionTreshold) const
{
  const auto& newMinimum = Ray::TmpVector3[0].copyFromFloats(
    minimum.x - intersectionTreshold, minimum.y - intersectionTreshold,
    minimum.z - intersectionTreshold);
  const auto& newMaximum = Ray::TmpVector3[1].copyFromFloats(
    maximum.x + intersectionTreshold, maximum.y + intersectionTreshold,
    maximum.z + intersectionTreshold);
  auto d        = 0.f;
  auto maxValue = std::numeric_limits<float>::max();
  auto inv      = 0.f;
  auto min      = 0.f;
  auto max      = 0.f;
  if (std::abs(direction.x) < 0.0000001f) {
    if (origin.x < newMinimum.x || origin.x > newMaximum.x) {
      return false;
    }
  }
  else {
    inv = 1.f / direction.x;
    min = (newMinimum.x - origin.x) * inv;
    max = (newMaximum.x - origin.x) * inv;
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
    if (origin.y < newMinimum.y || origin.y > newMaximum.y) {
      return false;
    }
  }
  else {
    inv = 1.f / direction.y;
    min = (newMinimum.y - origin.y) * inv;
    max = (newMaximum.y - origin.y) * inv;

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
    if (origin.z < newMinimum.z || origin.z > newMaximum.z) {
      return false;
    }
  }
  else {
    inv = 1.f / direction.z;
    min = (newMinimum.z - origin.z) * inv;
    max = (newMaximum.z - origin.z) * inv;

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

bool Ray::intersectsBox(const BoundingBox& box,
                        float intersectionTreshold) const
{
  return intersectsBoxMinMax(box.minimum, box.maximum, intersectionTreshold);
}

bool Ray::intersectsSphere(const BoundingSphere& sphere,
                           float intersectionTreshold) const
{
  const auto x      = sphere.center.x - origin.x;
  const auto y      = sphere.center.y - origin.y;
  const auto z      = sphere.center.z - origin.z;
  const auto pyth   = (x * x) + (y * y) + (z * z);
  const auto radius = sphere.radius + intersectionTreshold;
  const auto rr     = radius * radius;

  if (pyth <= rr) {
    return true;
  }

  const auto dot = (x * direction.x) + (y * direction.y) + (z * direction.z);
  if (dot < 0.f) {
    return false;
  }

  const auto temp = pyth - (dot * dot);

  return temp <= rr;
}

std::optional<IntersectionInfo> Ray::intersectsTriangle(const Vector3& vertex0,
                                                        const Vector3& vertex1,
                                                        const Vector3& vertex2)
{
  auto& edge1 = Ray::TmpVector3[0];
  auto& edge2 = Ray::TmpVector3[1];
  auto& pvec  = Ray::TmpVector3[2];
  auto& tvec  = Ray::TmpVector3[3];
  auto& qvec  = Ray::TmpVector3[4];

  vertex1.subtractToRef(vertex0, edge1);
  vertex2.subtractToRef(vertex0, edge2);
  Vector3::CrossToRef(direction, edge2, pvec);
  const auto det = Vector3::Dot(edge1, pvec);

  if (stl_util::almost_equal(det, 0.f)) {
    return std::nullopt;
  }

  const auto invdet = 1.f / det;

  origin.subtractToRef(vertex0, tvec);

  const auto bv = Vector3::Dot(tvec, pvec) * invdet;

  if (bv < 0.f || bv > 1.f) {
    return std::nullopt;
  }

  Vector3::CrossToRef(tvec, edge1, qvec);

  const auto bw = Vector3::Dot(direction, qvec) * invdet;

  if (bw < 0.f || bv + bw > 1.f) {
    return std::nullopt;
  }

  // check if the distance is longer than the predefined length.
  const auto distance = Vector3::Dot(edge2, qvec) * invdet;
  if (distance > length) {
    return std::nullopt;
  }

  return IntersectionInfo(1.f - bv - bw, bv, distance);
}

std::optional<float> Ray::intersectsPlane(const Plane& plane)
{
  auto distance = 0.f;
  auto result1  = Vector3::Dot(plane.normal, direction);
  if (std::abs(result1) < 9.99999997475243E-07f) {
    return std::nullopt;
  }
  else {
    const auto result2 = Vector3::Dot(plane.normal, origin);
    distance           = (-plane.d - result2) / result1;
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

std::optional<Vector3> Ray::intersectsAxis(const std::string& axis,
                                           float offset)
{
  if (axis == "y") {
    const auto t = (origin.y - offset) / direction.y;
    if (t > 0.f) {
      return std::nullopt;
    }
    return Vector3(origin.x + (direction.x * -t), offset,
                   origin.z + (direction.z * -t));
  }
  if (axis == "x") {
    const auto t = (origin.x - offset) / direction.x;
    if (t > 0.f) {
      return std::nullopt;
    }
    return Vector3(offset, origin.y + (direction.y * -t),
                   origin.z + (direction.z * -t));
  }
  if (axis == "z") {
    const auto t = (origin.z - offset) / direction.z;
    if (t > 0.f) {
      return std::nullopt;
    }
    return Vector3(origin.x + (direction.x * -t), origin.y + (direction.y * -t),
                   offset);
  }
  else {
    return std::nullopt;
  }
}

PickingInfo Ray::intersectsMesh(AbstractMesh* mesh, bool fastCheck)
{
  auto& tm = TmpVectors::MatrixArray[0];

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
Ray::intersectsMeshes(std::vector<AbstractMesh*>& meshes, bool fastCheck)
{
  std::vector<PickingInfo> results;
  results = intersectsMeshes(meshes, fastCheck, results);
  return results;
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
  const auto& o = origin;
  auto& u       = TmpVectors::Vector3Array[0];
  auto& rsegb   = TmpVectors::Vector3Array[1];
  auto& v       = TmpVectors::Vector3Array[2];
  auto& w       = TmpVectors::Vector3Array[3];

  segb.subtractToRef(sega, u);

  direction.scaleToRef(Ray::rayl, v);
  o.addToRef(v, rsegb);

  sega.subtractToRef(o, w);

  auto a  = Vector3::Dot(u, u); // always >= 0
  auto b  = Vector3::Dot(u, v);
  auto c  = Vector3::Dot(v, v); // always >= 0
  auto d  = Vector3::Dot(u, w);
  auto e  = Vector3::Dot(v, w);
  auto D  = a * c - b * b;         // always >= 0
  auto sc = 0.f, sN = 0.f, sD = D; // sc = sN / sD, default sD = D >= 0
  auto tc = 0.f, tN = 0.f, tD = D; // tc = tN / tD, default tD = D >= 0

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
  auto& qtc = TmpVectors::Vector3Array[4];
  v.scaleToRef(tc, qtc);
  auto& qsc = TmpVectors::Vector3Array[5];
  u.scaleToRef(sc, qsc);
  qsc.addInPlace(w);
  auto& dP = TmpVectors::Vector3Array[6];
  qsc.subtractToRef(qtc, dP); // = S1(sc) - S2(tc)

  const auto isIntersected
    = (tc > 0) && (tc <= length)
      && (dP.lengthSquared()
          < (threshold * threshold)); // return intersection result

  if (isIntersected) {
    return qsc.length();
  }
  return -1;
}

Ray& Ray::update(float x, float y, float viewportWidth, float viewportHeight,
                 Matrix& world, Matrix& view, Matrix& projection)
{
  unprojectRayToRef(x, y, viewportWidth, viewportHeight, world, view,
                    projection);
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
  const auto length
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

void Ray::unprojectRayToRef(float sourceX, float sourceY, float viewportWidth,
                            float viewportHeight, Matrix& world,
                            const Matrix& view, const Matrix& projection)
{
  auto& matrix = TmpVectors::MatrixArray[0];
  world.multiplyToRef(view, matrix);
  matrix.multiplyToRef(projection, matrix);
  matrix.invert();
  auto& nearScreenSource = TmpVectors::Vector3Array[0];
  nearScreenSource.x     = sourceX / viewportWidth * 2.f - 1.f;
  nearScreenSource.y     = -(sourceY / viewportHeight * 2.f - 1.f);
  nearScreenSource.z     = -1.f;
  auto& farScreenSource  = TmpVectors::Vector3Array[1].copyFromFloats(
    nearScreenSource.x, nearScreenSource.y, 1.f);
  auto& nearVec3 = TmpVectors::Vector3Array[2];
  auto& farVec3  = TmpVectors::Vector3Array[3];
  Vector3::_UnprojectFromInvertedMatrixToRef(nearScreenSource, matrix,
                                             nearVec3);
  Vector3::_UnprojectFromInvertedMatrixToRef(farScreenSource, matrix, farVec3);

  origin.copyFrom(nearVec3);
  farVec3.subtractToRef(nearVec3, direction);
  direction.normalize();
}

} // end of namespace BABYLON
