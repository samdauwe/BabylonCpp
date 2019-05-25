#include <babylon/math/tmp.h>

#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

std::array<Color3, 3> Tmp::Color3Array{
  {Color3::Black(), Color3::Black(), Color3::Black()}};
std::array<Color4, 3> Tmp::Color4Array{{Color4(0.f, 0.f, 0.f, 0.f),
                                        Color4(0.f, 0.f, 0.f, 0.f),
                                        Color4(0.f, 0.f, 0.f, 0.f)}};
std::array<Vector2, 3> Tmp::Vector2Array{
  {Vector2::Zero(), Vector2::Zero(), Vector2::Zero()}};
std::array<Vector3, 13> Tmp::Vector3Array{
  {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero()}};
std::array<Vector4, 3> Tmp::Vector4Array{
  {Vector4::Zero(), Vector4::Zero(), Vector4::Zero()}};
std::array<Quaternion, 2> Tmp::QuaternionArray{
  {Quaternion::Zero(), Quaternion::Zero()}};
std::array<Matrix, 8> Tmp::MatrixArray{
  {Matrix::Identity(), Matrix::Identity(), Matrix::Identity(),
   Matrix::Identity(), Matrix::Identity(), Matrix::Identity(),
   Matrix::Identity(), Matrix::Identity()}};

} // end of namespace BABYLON
