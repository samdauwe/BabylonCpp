#include <babylon/math/math_tmp.h>

#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

std::array<Vector3, 1> MathTmp::Vector3Array{{Vector3::Zero()}};
std::array<Matrix, 2> MathTmp::MatrixArray{{Matrix::Zero(), Matrix::Zero()}};
std::array<Quaternion, 1> MathTmp::QuaternionArray{{Quaternion::Zero()}};

} // end of namespace BABYLON
