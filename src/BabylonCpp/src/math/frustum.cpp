#include <babylon/math/frustum.h>

#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

array_t<Plane, 6> Frustum::GetPlanes(const Matrix& transform)
{
  array_t<Plane, 6> frustumPlanes{
    {Plane(0.f, 0.f, 0.f, 0.f), Plane(0.f, 0.f, 0.f, 0.f),
     Plane(0.f, 0.f, 0.f, 0.f), Plane(0.f, 0.f, 0.f, 0.f),
     Plane(0.f, 0.f, 0.f, 0.f), Plane(0.f, 0.f, 0.f, 0.f)}};

  Frustum::GetPlanesToRef(transform, frustumPlanes);

  return frustumPlanes;
}

void Frustum::GetNearPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  frustumPlane.normal.x = transform.m[3] + transform.m[2];
  frustumPlane.normal.y = transform.m[7] + transform.m[6];
  frustumPlane.normal.z = transform.m[11] + transform.m[10];
  frustumPlane.d        = transform.m[15] + transform.m[14];
  frustumPlane.normalize();
}

void Frustum::GetFarPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  frustumPlane.normal.x = transform.m[3] - transform.m[2];
  frustumPlane.normal.y = transform.m[7] - transform.m[6];
  frustumPlane.normal.z = transform.m[11] - transform.m[10];
  frustumPlane.d        = transform.m[15] - transform.m[14];
  frustumPlane.normalize();
}

void Frustum::GetLeftPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  frustumPlane.normal.x = transform.m[3] + transform.m[0];
  frustumPlane.normal.y = transform.m[7] + transform.m[4];
  frustumPlane.normal.z = transform.m[11] + transform.m[8];
  frustumPlane.d        = transform.m[15] + transform.m[12];
  frustumPlane.normalize();
}

void Frustum::GetRightPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  frustumPlane.normal.x = transform.m[3] - transform.m[0];
  frustumPlane.normal.y = transform.m[7] - transform.m[4];
  frustumPlane.normal.z = transform.m[11] - transform.m[8];
  frustumPlane.d        = transform.m[15] - transform.m[12];
  frustumPlane.normalize();
}

void Frustum::GetTopPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  frustumPlane.normal.x = transform.m[3] - transform.m[1];
  frustumPlane.normal.y = transform.m[7] - transform.m[5];
  frustumPlane.normal.z = transform.m[11] - transform.m[9];
  frustumPlane.d        = transform.m[15] - transform.m[13];
  frustumPlane.normalize();
}

void Frustum::GetBottomPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  frustumPlane.normal.x = transform.m[3] + transform.m[1];
  frustumPlane.normal.y = transform.m[7] + transform.m[5];
  frustumPlane.normal.z = transform.m[11] + transform.m[9];
  frustumPlane.d        = transform.m[15] + transform.m[13];
  frustumPlane.normalize();
}

void Frustum::GetPlanesToRef(const Matrix& transform,
                             array_t<Plane, 6>& frustumPlanes)
{
  // Near
  Frustum::GetNearPlaneToRef(transform, frustumPlanes[0]);

  // Far
  Frustum::GetFarPlaneToRef(transform, frustumPlanes[1]);

  // Left
  Frustum::GetLeftPlaneToRef(transform, frustumPlanes[2]);

  // Right
  Frustum::GetRightPlaneToRef(transform, frustumPlanes[3]);

  // Top
  Frustum::GetTopPlaneToRef(transform, frustumPlanes[4]);

  // Bottom
  Frustum::GetBottomPlaneToRef(transform, frustumPlanes[5]);
}

} // end of namespace BABYLON
