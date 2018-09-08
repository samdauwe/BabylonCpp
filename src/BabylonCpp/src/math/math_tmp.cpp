#include <babylon/math/math_tmp.h>

#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

std::array<Vector3, 6> MathTmp::Vector3Array{
  {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero()}};
std::array<Matrix, 2> MathTmp::MatrixArray{{Matrix::Zero(), Matrix::Zero()}};
std::array<Quaternion, 3> MathTmp::QuaternionArray{
  {Quaternion::Zero(), Quaternion::Zero(), Quaternion::Zero()}};

} // end of namespace BABYLON
