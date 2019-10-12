#include <babylon/collisions/collider.h>

#include <cmath>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/plane.h>

namespace BABYLON {

Collider::Collider()
    : intersectionPointSet{false}
    , collidedMesh{nullptr}
    , _radius{Vector3::One()}
    , _retry{0}
    , _basePointWorld{Vector3::Zero()}
    , collisionMask{this, &Collider::get_collisionMask,
                    &Collider::set_collisionMask}
    , _collisionPoint{Vector3::Zero()}
    , _planeIntersectionPoint{Vector3::Zero()}
    , _tempVector{Vector3::Zero()}
    , _tempVector2{Vector3::Zero()}
    , _tempVector3{Vector3::Zero()}
    , _tempVector4{Vector3::Zero()}
    , _edge{Vector3::Zero()}
    , _baseToVertex{Vector3::Zero()}
    , _destinationPoint{Vector3::Zero()}
    , _slidePlaneNormal{Vector3::Zero()}
    , _displacementVector{Vector3::Zero()}
    , _velocityWorld{Vector3::Zero()}
    , _normalizedVelocity{Vector3::Zero()}
    , _collisionMask{-1}
{
}

Collider::~Collider()
{
}

bool Collider::IntersectBoxAASphere(const Vector3& boxMin,
                                    const Vector3& boxMax,
                                    const Vector3& sphereCenter,
                                    float sphereRadius)
{
  if (boxMin.x > sphereCenter.x + sphereRadius) {
    return false;
  }

  if (sphereCenter.x - sphereRadius > boxMax.x) {
    return false;
  }

  if (boxMin.y > sphereCenter.y + sphereRadius) {
    return false;
  }

  if (sphereCenter.y - sphereRadius > boxMax.y) {
    return false;
  }

  if (boxMin.z > sphereCenter.z + sphereRadius) {
    return false;
  }

  if (sphereCenter.z - sphereRadius > boxMax.z) {
    return false;
  }

  return true;
}

LowestRoot Collider::GetLowestRoot(float a, float b, float c, float maxR)
{
  LowestRoot result{0.f, false};
  const auto determinant = b * b - 4.f * a * c;
  if (determinant < 0.f) {
    return result;
  }

  const auto sqrtD = std::sqrt(determinant);
  auto r1          = (-b - sqrtD) / (2.f * a);
  auto r2          = (-b + sqrtD) / (2.f * a);

  if (r1 > r2) {
    std::swap(r1, r2);
  }

  if (r1 > 0.f && r1 < maxR) {
    result.root  = r1;
    result.found = true;
    return result;
  }

  if (r2 > 0.f && r2 < maxR) {
    result.root  = r2;
    result.found = true;
    return result;
  }

  return result;
}

int Collider::get_collisionMask() const
{
  return _collisionMask;
}

void Collider::set_collisionMask(int mask)
{
  _collisionMask = !isNan(mask) ? mask : -1;
}

Vector3& Collider::slidePlaneNormal()
{
  return _slidePlaneNormal;
}

void Collider::_initialize(Vector3& source, Vector3& dir, float e)
{
  _velocity = dir;
  Vector3::NormalizeToRef(dir, _normalizedVelocity);
  _basePoint = source;

  source.multiplyToRef(_radius, _basePointWorld);
  dir.multiplyToRef(_radius, _velocityWorld);

  _velocityWorldLength = _velocityWorld.length();

  _epsilon       = e;
  collisionFound = false;
}

bool Collider::_checkPointInTriangle(const Vector3& point, const Vector3& pa,
                                     const Vector3& pb, const Vector3& pc,
                                     const Vector3& n)
{
  pa.subtractToRef(point, _tempVector);
  pb.subtractToRef(point, _tempVector2);

  Vector3::CrossToRef(_tempVector, _tempVector2, _tempVector4);
  auto d = Vector3::Dot(_tempVector4, n);
  if (d < 0.f) {
    return false;
  }

  pc.subtractToRef(point, _tempVector3);
  Vector3::CrossToRef(_tempVector2, _tempVector3, _tempVector4);
  d = Vector3::Dot(_tempVector4, n);
  if (d < 0.f) {
    return false;
  }

  Vector3::CrossToRef(_tempVector3, _tempVector, _tempVector4);
  d = Vector3::Dot(_tempVector4, n);
  return d >= 0.f;
}

bool Collider::_canDoCollision(const Vector3& sphereCenter, float sphereRadius,
                               const Vector3& vecMin,
                               const Vector3& vecMax) const
{
  auto distance = Vector3::Distance(_basePointWorld, sphereCenter);

  auto max = stl_util::max(_radius.x, _radius.y, _radius.z);

  if (distance > _velocityWorldLength + max + sphereRadius) {
    return false;
  }

  if (!IntersectBoxAASphere(vecMin, vecMax, _basePointWorld,
                            _velocityWorldLength + max)) {
    return false;
  }

  return true;
}

void Collider::_testTriangle(size_t faceIndex,
                             std::vector<Plane>& trianglePlaneArray,
                             const Vector3& p1, const Vector3& p2,
                             const Vector3& p3, bool hasMaterial,
                             const AbstractMeshPtr& hostMesh)
{
  auto f = 0.f, t0 = 0.f;
  auto embeddedInPlane = false;

  if (faceIndex >= trianglePlaneArray.size()) {
    for (size_t i = trianglePlaneArray.size(); i <= faceIndex; ++i) {
      trianglePlaneArray.emplace_back(Plane(0.f, 0.f, 0.f, 0.f));
    }
    trianglePlaneArray[faceIndex].copyFromPoints(p1, p2, p3);
  }

  auto& trianglePlane = trianglePlaneArray[faceIndex];

  if ((!hasMaterial)
      && !trianglePlane.isFrontFacingTo(_normalizedVelocity, 0)) {
    return;
  }

  auto signedDistToTrianglePlane = trianglePlane.signedDistanceTo(_basePoint);
  auto normalDotVelocity = Vector3::Dot(trianglePlane.normal, _velocity);

  if (stl_util::almost_equal(normalDotVelocity, 0.f)) {
    if (std::abs(signedDistToTrianglePlane) >= 1.f) {
      return;
    }
    embeddedInPlane = true;
    t0              = 0;
  }
  else {
    t0      = (-1.f - signedDistToTrianglePlane) / normalDotVelocity;
    auto t1 = (1.f - signedDistToTrianglePlane) / normalDotVelocity;

    if (t0 > t1) {
      std::swap(t0, t1);
    }

    if (t0 > 1.f || t1 < 0.f) {
      return;
    }

    if (t0 < 0.f) {
      t0 = 0.f;
    }
    if (t0 > 1.f) {
      t0 = 1.f;
    }
  }

  _collisionPoint.copyFromFloats(0.f, 0.f, 0.f);

  auto found = false;
  auto t     = 1.f;

  if (!embeddedInPlane) {
    _basePoint.subtractToRef(trianglePlane.normal, _planeIntersectionPoint);
    _velocity.scaleToRef(t0, _tempVector);
    _planeIntersectionPoint.addInPlace(_tempVector);

    if (_checkPointInTriangle(_planeIntersectionPoint, p1, p2, p3,
                              trianglePlane.normal)) {
      found = true;
      t     = t0;
      _collisionPoint.copyFrom(_planeIntersectionPoint);
    }
  }

  if (!found) {
    auto velocitySquaredLength = _velocity.lengthSquared();

    auto a = velocitySquaredLength;

    _basePoint.subtractToRef(p1, _tempVector);
    auto b = 2.f * (Vector3::Dot(_velocity, _tempVector));
    auto c = _tempVector.lengthSquared() - 1.f;

    auto lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      t     = lowestRoot.root;
      found = true;
      _collisionPoint.copyFrom(p1);
    }

    _basePoint.subtractToRef(p2, _tempVector);
    b = 2.f * (Vector3::Dot(_velocity, _tempVector));
    c = _tempVector.lengthSquared() - 1.f;

    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      t     = lowestRoot.root;
      found = true;
      _collisionPoint.copyFrom(p2);
    }

    _basePoint.subtractToRef(p3, _tempVector);
    b = 2.f * (Vector3::Dot(_velocity, _tempVector));
    c = _tempVector.lengthSquared() - 1.f;

    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      t     = lowestRoot.root;
      found = true;
      _collisionPoint.copyFrom(p3);
    }

    p2.subtractToRef(p1, _edge);
    p1.subtractToRef(_basePoint, _baseToVertex);
    auto edgeSquaredLength   = _edge.lengthSquared();
    auto edgeDotVelocity     = Vector3::Dot(_edge, _velocity);
    auto edgeDotBaseToVertex = Vector3::Dot(_edge, _baseToVertex);

    a = edgeSquaredLength * (-velocitySquaredLength)
        + edgeDotVelocity * edgeDotVelocity;
    b = edgeSquaredLength * (2.f * Vector3::Dot(_velocity, _baseToVertex))
        - 2.f * edgeDotVelocity * edgeDotBaseToVertex;
    c = edgeSquaredLength * (1.f - _baseToVertex.lengthSquared())
        + edgeDotBaseToVertex * edgeDotBaseToVertex;

    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      auto _f = (edgeDotVelocity * lowestRoot.root - edgeDotBaseToVertex)
                / edgeSquaredLength;

      if (_f >= 0.f && _f <= 1.f) {
        t     = lowestRoot.root;
        found = true;
        _edge.scaleInPlace(_f);
        p1.addToRef(_edge, _collisionPoint);
      }
    }

    p3.subtractToRef(p2, _edge);
    p2.subtractToRef(_basePoint, _baseToVertex);
    edgeSquaredLength   = _edge.lengthSquared();
    edgeDotVelocity     = Vector3::Dot(_edge, _velocity);
    edgeDotBaseToVertex = Vector3::Dot(_edge, _baseToVertex);

    a = edgeSquaredLength * (-velocitySquaredLength)
        + edgeDotVelocity * edgeDotVelocity;
    b = edgeSquaredLength * (2.f * Vector3::Dot(_velocity, _baseToVertex))
        - 2.f * edgeDotVelocity * edgeDotBaseToVertex;
    c = edgeSquaredLength * (1.f - _baseToVertex.lengthSquared())
        + edgeDotBaseToVertex * edgeDotBaseToVertex;
    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      auto _f = (edgeDotVelocity * lowestRoot.root - edgeDotBaseToVertex)
                / edgeSquaredLength;

      if (_f >= 0.f && _f <= 1.f) {
        t     = lowestRoot.root;
        found = true;
        _edge.scaleInPlace(_f);
        p2.addToRef(_edge, _collisionPoint);
      }
    }

    p1.subtractToRef(p3, _edge);
    p3.subtractToRef(_basePoint, _baseToVertex);
    edgeSquaredLength   = _edge.lengthSquared();
    edgeDotVelocity     = Vector3::Dot(_edge, _velocity);
    edgeDotBaseToVertex = Vector3::Dot(_edge, _baseToVertex);

    a = edgeSquaredLength * (-velocitySquaredLength)
        + edgeDotVelocity * edgeDotVelocity;
    b = edgeSquaredLength * (2.f * Vector3::Dot(_velocity, _baseToVertex))
        - 2.f * edgeDotVelocity * edgeDotBaseToVertex;
    c = edgeSquaredLength * (1.f - _baseToVertex.lengthSquared())
        + edgeDotBaseToVertex * edgeDotBaseToVertex;

    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      f = (edgeDotVelocity * lowestRoot.root - edgeDotBaseToVertex)
          / edgeSquaredLength;

      if (f >= 0.f && f <= 1.f) {
        t     = lowestRoot.root;
        found = true;
        _edge.scaleInPlace(f);
        p3.addToRef(_edge, _collisionPoint);
      }
    }
  }

  if (found) {
    auto distToCollision = t * _velocity.length();

    if (!collisionFound || distToCollision < _nearestDistance) {
      if (!intersectionPointSet) {
        intersectionPoint    = _collisionPoint;
        intersectionPointSet = true;
      }
      else {
        intersectionPoint.copyFrom(_collisionPoint);
      }
      _nearestDistance = distToCollision;
      collisionFound   = true;
      collidedMesh     = hostMesh;
    }
  }
}

void Collider::_collide(std::vector<Plane>& trianglePlaneArray,
                        const std::vector<Vector3> pts,
                        const IndicesArray& indices, size_t indexStart,
                        size_t indexEnd, unsigned int decal, bool hasMaterial,
                        const AbstractMeshPtr& hostMesh)
{
  for (size_t i = indexStart; i < indexEnd; i += 3) {
    const auto& p1 = pts[indices[i] - decal];
    const auto& p2 = pts[indices[i + 1] - decal];
    const auto& p3 = pts[indices[i + 2] - decal];

    _testTriangle(i, trianglePlaneArray, p3, p2, p1, hasMaterial, hostMesh);
  }
}

void Collider::_getResponse(Vector3& pos, Vector3& vel)
{
  pos.addToRef(vel, _destinationPoint);
  vel.scaleInPlace((_nearestDistance / vel.length()));

  _basePoint.addToRef(vel, pos);
  pos.subtractToRef(intersectionPoint, _slidePlaneNormal);
  _slidePlaneNormal.normalize();
  _slidePlaneNormal.scaleToRef(_epsilon, _displacementVector);

  pos.addInPlace(_displacementVector);
  intersectionPoint.addInPlace(_displacementVector);

  _slidePlaneNormal.scaleInPlace(
    Plane::SignedDistanceToPlaneFromPositionAndNormal(
      intersectionPoint, _slidePlaneNormal, _destinationPoint));
  _destinationPoint.subtractInPlace(_slidePlaneNormal);

  _destinationPoint.subtractToRef(intersectionPoint, vel);
}

} // end of namespace BABYLON
