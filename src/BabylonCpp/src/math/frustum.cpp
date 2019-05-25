#include <babylon/math/frustum.h>

#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

std::array<Plane, 6> Frustum::GetPlanes(const Matrix& transform)
{
  std::array<Plane, 6> frustumPlanes{
    {Plane(0.f, 0.f, 0.f, 0.f), Plane(0.f, 0.f, 0.f, 0.f),
     Plane(0.f, 0.f, 0.f, 0.f), Plane(0.f, 0.f, 0.f, 0.f),
     Plane(0.f, 0.f, 0.f, 0.f), Plane(0.f, 0.f, 0.f, 0.f)}};
  Frustum::GetPlanesToRef(transform, frustumPlanes);
  return frustumPlanes;
}

void Frustum::GetNearPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  const auto& m         = transform.m();
  frustumPlane.normal.x = m[3] + m[2];
  frustumPlane.normal.y = m[7] + m[6];
  frustumPlane.normal.z = m[11] + m[10];
  frustumPlane.d        = m[15] + m[14];
  frustumPlane.normalize();
}

void Frustum::GetFarPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  const auto& m         = transform.m();
  frustumPlane.normal.x = m[3] - m[2];
  frustumPlane.normal.y = m[7] - m[6];
  frustumPlane.normal.z = m[11] - m[10];
  frustumPlane.d        = m[15] - m[14];
  frustumPlane.normalize();
}

void Frustum::GetLeftPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  const auto& m         = transform.m();
  frustumPlane.normal.x = m[3] + m[0];
  frustumPlane.normal.y = m[7] + m[4];
  frustumPlane.normal.z = m[11] + m[8];
  frustumPlane.d        = m[15] + m[12];
  frustumPlane.normalize();
}

void Frustum::GetRightPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  const auto& m         = transform.m();
  frustumPlane.normal.x = m[3] - m[0];
  frustumPlane.normal.y = m[7] - m[4];
  frustumPlane.normal.z = m[11] - m[8];
  frustumPlane.d        = m[15] - m[12];
  frustumPlane.normalize();
}

void Frustum::GetTopPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  const auto& m         = transform.m();
  frustumPlane.normal.x = m[3] - m[1];
  frustumPlane.normal.y = m[7] - m[5];
  frustumPlane.normal.z = m[11] - m[9];
  frustumPlane.d        = m[15] - m[13];
  frustumPlane.normalize();
}

void Frustum::GetBottomPlaneToRef(const Matrix& transform, Plane& frustumPlane)
{
  const auto& m         = transform.m();
  frustumPlane.normal.x = m[3] + m[1];
  frustumPlane.normal.y = m[7] + m[5];
  frustumPlane.normal.z = m[11] + m[9];
  frustumPlane.d        = m[15] + m[13];
  frustumPlane.normalize();
}

void Frustum::GetPlanesToRef(const Matrix& transform,
                             std::array<Plane, 6>& frustumPlanes)
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
