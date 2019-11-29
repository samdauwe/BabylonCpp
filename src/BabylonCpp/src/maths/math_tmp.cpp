#include <babylon/maths/math_tmp.h>

#include <babylon/maths/matrix.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

std::array<Vector3, 6> MathTmp::Vector3Array{
  {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero()}};
std::array<Matrix, 2> MathTmp::MatrixArray{
  {Matrix::Identity(), Matrix::Identity()}};
std::array<Quaternion, 3> MathTmp::QuaternionArray{
  {Quaternion::Zero(), Quaternion::Zero(), Quaternion::Zero()}};

} // end of namespace BABYLON
