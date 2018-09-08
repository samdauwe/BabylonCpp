#ifndef BABYLON_MATH_POSITION_NORMAL_TEXTURE_VERTEX_H
#define BABYLON_MATH_POSITION_NORMAL_TEXTURE_VERTEX_H

#include <iostream>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Represents a PositionNormalTextureVertex.
 */
class BABYLON_SHARED_EXPORT PositionNormalTextureVertex {

public:
  PositionNormalTextureVertex(const Vector3& position = Vector3::Zero(),
                              const Vector3& normal   = Vector3::Up(),
                              const Vector2& uv       = Vector2::Zero());
  PositionNormalTextureVertex(const PositionNormalTextureVertex& other);
  PositionNormalTextureVertex(PositionNormalTextureVertex&& other);
  PositionNormalTextureVertex&
  operator=(const PositionNormalTextureVertex& other);
  PositionNormalTextureVertex& operator=(PositionNormalTextureVertex&& other);
  ~PositionNormalTextureVertex();
  PositionNormalTextureVertex copy() const;
  std::unique_ptr<PositionNormalTextureVertex> clone() const;
  friend std::ostream&
  operator<<(std::ostream& os,
             const PositionNormalTextureVertex& positionNormalTextureVertex);

public:
  Vector3 position;
  Vector3 normal;
  Vector2 uv;

}; // end of class PositionNormalTextureVertex

std::ostream&
operator<<(std::ostream& os,
           const PositionNormalTextureVertex& positionNormalTextureVertex);

} // namespace BABYLON

#endif // end of BABYLON_MATH_POSITION_NORMAL_TEXTURE_VERTEX_H
