#include <babylon/math/tmp.h>

#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

array_t<Color3, 3> Tmp::Color3Array{
  {Color3::Black(), Color3::Black(), Color3::Black()}};
array_t<Color4, 2> Tmp::Color4Array{
  {Color4(0.f, 0.f, 0.f, 0.f), Color4(0.f, 0.f, 0.f, 0.f)}};
array_t<Vector2, 3> Tmp::Vector2Array{
  {Vector2::Zero(), Vector2::Zero(), Vector2::Zero()}};
array_t<Vector3, 9> Tmp::Vector3Array{
  {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero()}};
array_t<Vector4, 3> Tmp::Vector4Array{
  {Vector4::Zero(), Vector4::Zero(), Vector4::Zero()}};
array_t<Quaternion, 2> Tmp::QuaternionArray{
  {Quaternion::Zero(), Quaternion::Zero()}};
array_t<Matrix, 8> Tmp::MatrixArray{
  {Matrix::Zero(), Matrix::Zero(), Matrix::Zero(), Matrix::Zero(),
   Matrix::Zero(), Matrix::Zero(), Matrix::Zero(), Matrix::Zero()}};

} // end of namespace BABYLON
