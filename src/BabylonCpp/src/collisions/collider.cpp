#include <babylon/collisions/collider.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/plane.h>

namespace BABYLON {

Collider::Collider()
    : _radius{Vector3::One()}
    , retry{0}
    , basePointWorld{Vector3::Zero()}
    , velocityWorld{Vector3::Zero()}
    , normalizedVelocity{Vector3::Zero()}
    , intersectionPointSet{false}
    , collidedMesh{nullptr}
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
  const float determinant = b * b - 4.f * a * c;
  if (determinant < 0.f) {
    return result;
  }

  const float sqrtD = ::std::sqrt(determinant);
  float r1          = (-b - sqrtD) / (2.f * a);
  float r2          = (-b + sqrtD) / (2.f * a);

  if (r1 > r2) {
    ::std::swap(r1, r2);
  }

  if (r1 > 0 && r1 < maxR) {
    result.root  = r1;
    result.found = true;
    return result;
  }

  if (r2 > 0 && r2 < maxR) {
    result.root  = r2;
    result.found = true;
    return result;
  }

  return result;
}

int Collider::collisionMask() const
{
  return _collisionMask;
}

void Collider::setCollisionMask(int mask)
{
  _collisionMask = !isNan(mask) ? mask : -1;
}

void Collider::_initialize(Vector3& source, Vector3& dir, float e)
{
  velocity = dir;
  Vector3::NormalizeToRef(dir, normalizedVelocity);
  basePoint = source;

  source.multiplyToRef(_radius, basePointWorld);
  dir.multiplyToRef(_radius, velocityWorld);

  velocityWorldLength = velocityWorld.length();

  epsilon        = e;
  collisionFound = false;
}

bool Collider::_checkPointInTriangle(const Vector3& point, const Vector3& pa,
                                     const Vector3& pb, const Vector3& pc,
                                     const Vector3& n)
{
  pa.subtractToRef(point, _tempVector);
  pb.subtractToRef(point, _tempVector2);

  Vector3::CrossToRef(_tempVector, _tempVector2, _tempVector4);
  float d = Vector3::Dot(_tempVector4, n);
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
  float distance = Vector3::Distance(basePointWorld, sphereCenter);

  float max = stl_util::max(_radius.x, _radius.y, _radius.z);

  if (distance > velocityWorldLength + max + sphereRadius) {
    return false;
  }

  if (!IntersectBoxAASphere(vecMin, vecMax, basePointWorld,
                            velocityWorldLength + max)) {
    return false;
  }

  return true;
}

void Collider::_testTriangle(size_t faceIndex,
                             vector_t<Plane>& trianglePlaneArray,
                             const Vector3& p1, const Vector3& p2,
                             const Vector3& p3, bool hasMaterial)
{
  float f, t0;
  bool embeddedInPlane = false;

  if (faceIndex >= trianglePlaneArray.size()) {
    for (size_t i = trianglePlaneArray.size(); i <= faceIndex; ++i) {
      trianglePlaneArray.emplace_back(Plane(0.f, 0.f, 0.f, 0.f));
    }
    trianglePlaneArray[faceIndex].copyFromPoints(p1, p2, p3);
  }

  auto& trianglePlane = trianglePlaneArray[faceIndex];

  if ((!hasMaterial) && !trianglePlane.isFrontFacingTo(normalizedVelocity, 0)) {
    return;
  }

  float signedDistToTrianglePlane = trianglePlane.signedDistanceTo(basePoint);
  float normalDotVelocity = Vector3::Dot(trianglePlane.normal, velocity);

  if (stl_util::almost_equal(normalDotVelocity, 0.f)) {
    if (::std::abs(signedDistToTrianglePlane) >= 1.f) {
      return;
    }
    embeddedInPlane = true;
    t0              = 0;
  }
  else {
    t0       = (-1.f - signedDistToTrianglePlane) / normalDotVelocity;
    float t1 = (1.f - signedDistToTrianglePlane) / normalDotVelocity;

    if (t0 > t1) {
      ::std::swap(t0, t1);
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

  bool found = false;
  float t    = 1.f;

  if (!embeddedInPlane) {
    basePoint.subtractToRef(trianglePlane.normal, _planeIntersectionPoint);
    velocity.scaleToRef(t0, _tempVector);
    _planeIntersectionPoint.addInPlace(_tempVector);

    if (_checkPointInTriangle(_planeIntersectionPoint, p1, p2, p3,
                              trianglePlane.normal)) {
      found = true;
      t     = t0;
      _collisionPoint.copyFrom(_planeIntersectionPoint);
    }
  }

  if (!found) {
    float velocitySquaredLength = velocity.lengthSquared();

    float a = velocitySquaredLength;

    basePoint.subtractToRef(p1, _tempVector);
    float b = 2.f * (Vector3::Dot(velocity, _tempVector));
    float c = _tempVector.lengthSquared() - 1.f;

    auto lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      t     = lowestRoot.root;
      found = true;
      _collisionPoint.copyFrom(p1);
    }

    basePoint.subtractToRef(p2, _tempVector);
    b = 2.f * (Vector3::Dot(velocity, _tempVector));
    c = _tempVector.lengthSquared() - 1.f;

    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      t     = lowestRoot.root;
      found = true;
      _collisionPoint.copyFrom(p2);
    }

    basePoint.subtractToRef(p3, _tempVector);
    b = 2.f * (Vector3::Dot(velocity, _tempVector));
    c = _tempVector.lengthSquared() - 1.f;

    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      t     = lowestRoot.root;
      found = true;
      _collisionPoint.copyFrom(p3);
    }

    p2.subtractToRef(p1, _edge);
    p1.subtractToRef(basePoint, _baseToVertex);
    float edgeSquaredLength   = _edge.lengthSquared();
    float edgeDotVelocity     = Vector3::Dot(_edge, velocity);
    float edgeDotBaseToVertex = Vector3::Dot(_edge, _baseToVertex);

    a = edgeSquaredLength * (-velocitySquaredLength)
        + edgeDotVelocity * edgeDotVelocity;
    b = edgeSquaredLength * (2.f * Vector3::Dot(velocity, _baseToVertex))
        - 2.f * edgeDotVelocity * edgeDotBaseToVertex;
    c = edgeSquaredLength * (1.f - _baseToVertex.lengthSquared())
        + edgeDotBaseToVertex * edgeDotBaseToVertex;

    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      float _f = (edgeDotVelocity * lowestRoot.root - edgeDotBaseToVertex)
                 / edgeSquaredLength;

      if (_f >= 0.f && _f <= 1.f) {
        t     = lowestRoot.root;
        found = true;
        _edge.scaleInPlace(_f);
        p1.addToRef(_edge, _collisionPoint);
      }
    }

    p3.subtractToRef(p2, _edge);
    p2.subtractToRef(basePoint, _baseToVertex);
    edgeSquaredLength   = _edge.lengthSquared();
    edgeDotVelocity     = Vector3::Dot(_edge, velocity);
    edgeDotBaseToVertex = Vector3::Dot(_edge, _baseToVertex);

    a = edgeSquaredLength * (-velocitySquaredLength)
        + edgeDotVelocity * edgeDotVelocity;
    b = edgeSquaredLength * (2.f * Vector3::Dot(velocity, _baseToVertex))
        - 2.f * edgeDotVelocity * edgeDotBaseToVertex;
    c = edgeSquaredLength * (1.f - _baseToVertex.lengthSquared())
        + edgeDotBaseToVertex * edgeDotBaseToVertex;
    lowestRoot = GetLowestRoot(a, b, c, t);
    if (lowestRoot.found) {
      float _f = (edgeDotVelocity * lowestRoot.root - edgeDotBaseToVertex)
                 / edgeSquaredLength;

      if (_f >= 0.f && _f <= 1.f) {
        t     = lowestRoot.root;
        found = true;
        _edge.scaleInPlace(_f);
        p2.addToRef(_edge, _collisionPoint);
      }
    }

    p1.subtractToRef(p3, _edge);
    p3.subtractToRef(basePoint, _baseToVertex);
    edgeSquaredLength   = _edge.lengthSquared();
    edgeDotVelocity     = Vector3::Dot(_edge, velocity);
    edgeDotBaseToVertex = Vector3::Dot(_edge, _baseToVertex);

    a = edgeSquaredLength * (-velocitySquaredLength)
        + edgeDotVelocity * edgeDotVelocity;
    b = edgeSquaredLength * (2.f * Vector3::Dot(velocity, _baseToVertex))
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
    float distToCollision = t * velocity.length();

    if (!collisionFound || distToCollision < nearestDistance) {
      if (!intersectionPointSet) {
        intersectionPoint    = _collisionPoint;
        intersectionPointSet = true;
      }
      else {
        intersectionPoint.copyFrom(_collisionPoint);
      }
      nearestDistance = distToCollision;
      collisionFound  = true;
    }
  }
}

void Collider::_collide(vector_t<Plane>& trianglePlaneArray,
                        const vector_t<Vector3> pts,
                        const IndicesArray& indices, size_t indexStart,
                        size_t indexEnd, unsigned int decal, bool hasMaterial)
{
  for (size_t i = indexStart; i < indexEnd; i += 3) {
    const auto& p1 = pts[indices[i] - decal];
    const auto& p2 = pts[indices[i + 1] - decal];
    const auto& p3 = pts[indices[i + 2] - decal];

    _testTriangle(i, trianglePlaneArray, p3, p2, p1, hasMaterial);
  }
}

void Collider::_getResponse(Vector3& pos, Vector3& vel)
{
  pos.addToRef(vel, _destinationPoint);
  vel.scaleInPlace((nearestDistance / vel.length()));

  basePoint.addToRef(vel, pos);
  pos.subtractToRef(intersectionPoint, _slidePlaneNormal);
  _slidePlaneNormal.normalize();
  _slidePlaneNormal.scaleToRef(epsilon, _displacementVector);

  pos.addInPlace(_displacementVector);
  intersectionPoint.addInPlace(_displacementVector);

  _slidePlaneNormal.scaleInPlace(
    Plane::SignedDistanceToPlaneFromPositionAndNormal(
      intersectionPoint, _slidePlaneNormal, _destinationPoint));
  _destinationPoint.subtractInPlace(_slidePlaneNormal);

  _destinationPoint.subtractToRef(intersectionPoint, vel);
}

} // end of namespace BABYLON
