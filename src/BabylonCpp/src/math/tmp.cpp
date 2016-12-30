#include <babylon/math/tmp.h>

#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/math/quaternion.h>

namespace BABYLON {

std::array<Color3, 3> Tmp::Color3Array{
  {Color3::Black(), Color3::Black(), Color3::Black()}};
std::array<Vector2, 3> Tmp::Vector2Array{
  {Vector2::Zero(), Vector2::Zero(), Vector2::Zero()}};
std::array<Vector3, 9> Tmp::Vector3Array{
  {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero()}};
std::array<Vector4, 3> Tmp::Vector4Array{
  {Vector4::Zero(), Vector4::Zero(), Vector4::Zero()}};
std::array<Quaternion, 1> QuaternionArray{{Quaternion(0, 0, 0, 0)}};
std::array<Matrix, 6> Tmp::MatrixArray{{Matrix::Zero(), Matrix::Zero(),
                                              Matrix::Zero(), Matrix::Zero(),
                                              Matrix::Zero(), Matrix::Zero()}};

} // end of namespace BABYLON
