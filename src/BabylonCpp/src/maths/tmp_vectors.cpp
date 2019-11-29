#include <babylon/maths/tmp_vectors.h>

#include <babylon/maths/color3.h>
#include <babylon/maths/color4.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/vector2.h>
#include <babylon/maths/vector3.h>
#include <babylon/maths/vector4.h>

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
