#ifndef BABYLON_MATH_FUNCTIONS_H
#define BABYLON_MATH_FUNCTIONS_H

#include <babylon/babylon_common.h>
#include <babylon/core/structs.h>
#include <babylon/math/vector2.h>

namespace BABYLON {

/**
 * @brief Extracts minimum and maximum values from a list of indexed
 * positions.
 * @param positions defines the positions to use
 * @param indices defines the indices to the positions
 * @param indexStart defines the start index
 * @param indexCount defines the end index
 * @param bias defines bias value to add to the result
 * @return minimum and maximum values
 */
inline MinMax ExtractMinAndMaxIndexed(const Float32Array& positions,
                                      const Uint32Array& indices,
                                      size_t indexStart, size_t indexCount,
                                      const std::optional<Vector2>& bias
                                      = std::nullopt)
{
  Vector3 minimum(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
  Vector3 maximum(std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest());

  for (size_t index = indexStart; index < indexStart + indexCount; ++index) {
    const auto offset = indices[index] * 3;
    const auto x      = positions[offset];
    const auto y      = positions[offset + 1];
    const auto z      = positions[offset + 2];
    minimum.minimizeInPlaceFromFloats(x, y, z);
    maximum.maximizeInPlaceFromFloats(x, y, z);
  }

  if (bias) {
    const auto& _bias = *bias;
    minimum.x -= minimum.x * _bias.x + _bias.y;
    minimum.y -= minimum.y * _bias.x + _bias.y;
    minimum.z -= minimum.z * _bias.x + _bias.y;
    maximum.x += maximum.x * _bias.x + _bias.y;
    maximum.y += maximum.y * _bias.x + _bias.y;
    maximum.z += maximum.z * _bias.x + _bias.y;
  }

  return {minimum, maximum};
}

/**
 * @brief Extracts minimum and maximum values from a list of positions.
 * @param positions defines the positions to use
 * @param start defines the start index in the positions array
 * @param count defines the number of positions to handle
 * @param bias defines bias value to add to the result
 * @param stride defines the stride size to use (distance between two
 * positions in the positions array)
 * @return minimum and maximum values
 */
inline MinMax
ExtractMinAndMax(const Float32Array& positions, size_t start, size_t count,
                 const std::optional<Vector2>& bias = std::nullopt,
                 std::optional<unsigned int> stride = std::nullopt)
{
  Vector3 minimum(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
  Vector3 maximum(std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest());

  if (!stride) {
    stride = 3;
  }

  auto _stride = *stride;
  for (size_t index = start, offset = start * _stride; index < start + count;
       index++, offset += _stride) {
    const auto x = positions[offset];
    const auto y = positions[offset + 1];
    const auto z = positions[offset + 2];
    minimum.minimizeInPlaceFromFloats(x, y, z);
    maximum.maximizeInPlaceFromFloats(x, y, z);
  }

  if (bias) {
    const auto& _bias = *bias;
    minimum.x -= minimum.x * _bias.x + _bias.y;
    minimum.y -= minimum.y * _bias.x + _bias.y;
    minimum.z -= minimum.z * _bias.x + _bias.y;
    maximum.x += maximum.x * _bias.x + _bias.y;
    maximum.y += maximum.y * _bias.x + _bias.y;
    maximum.z += maximum.z * _bias.x + _bias.y;
  }

  return {minimum, maximum};
}

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_FRUSTUM_H
