#include <babylon/math/tmp_vectors.h>

#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

std::array<Color3, 3> TmpVectors::Color3Array{
  {Color3::Black(), Color3::Black(), Color3::Black()}};
std::array<Color4, 3> TmpVectors::Color4Array{{Color4(0.f, 0.f, 0.f, 0.f),
                                               Color4(0.f, 0.f, 0.f, 0.f),
                                               Color4(0.f, 0.f, 0.f, 0.f)}};
std::array<Vector2, 3> TmpVectors::Vector2Array{
  {Vector2::Zero(), Vector2::Zero(), Vector2::Zero()}};
std::array<Vector3, 13> TmpVectors::Vector3Array{
  {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero()}};
std::array<Vector4, 3> TmpVectors::Vector4Array{
  {Vector4::Zero(), Vector4::Zero(), Vector4::Zero()}};
std::array<Quaternion, 2> TmpVectors::QuaternionArray{
  {Quaternion::Zero(), Quaternion::Zero()}};
std::array<Matrix, 8> TmpVectors::MatrixArray{
  {Matrix::Identity(), Matrix::Identity(), Matrix::Identity(),
   Matrix::Identity(), Matrix::Identity(), Matrix::Identity(),
   Matrix::Identity(), Matrix::Identity()}};

} // end of namespace BABYLON
