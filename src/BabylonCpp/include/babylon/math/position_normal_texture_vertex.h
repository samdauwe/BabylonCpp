#ifndef BABYLON_MATH_POSITION_NORMAL_TEXTURE_VERTEX_H
#define BABYLON_MATH_POSITION_NORMAL_TEXTURE_VERTEX_H

#include <iostream>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Contains position, normal and uv vectors for a vertex.
 */
class BABYLON_SHARED_EXPORT PositionNormalTextureVertex {

public:
  /**
   * @brief Creates a PositionNormalTextureVertex.
   * @param position the position of the vertex (defaut: 0,0,0)
   * @param normal the normal of the vertex (defaut: 0,1,0)
   * @param uv the uv of the vertex (default: 0,0)
   */
  PositionNormalTextureVertex(const Vector3& position = Vector3::Zero(),
                              const Vector3& normal   = Vector3::Up(),
                              const Vector2& uv       = Vector2::Zero());
  PositionNormalTextureVertex(const PositionNormalTextureVertex& other);
  PositionNormalTextureVertex(PositionNormalTextureVertex&& other);
  PositionNormalTextureVertex&
  operator=(const PositionNormalTextureVertex& other);
  PositionNormalTextureVertex& operator=(PositionNormalTextureVertex&& other);
  ~PositionNormalTextureVertex(); // = default
  PositionNormalTextureVertex copy() const;

  /**
   * @brief Clones the PositionNormalTextureVertex.
   * @returns the cloned PositionNormalTextureVertex
   */
  std::unique_ptr<PositionNormalTextureVertex> clone() const;

  friend std::ostream&
  operator<<(std::ostream& os,
             const PositionNormalTextureVertex& positionNormalTextureVertex);

public:
  /**
   * the position of the vertex (defaut: 0,0,0)
   */
  Vector3 position;

  /**
   * the normal of the vertex (defaut: 0,1,0)
   */
  Vector3 normal;

  /**
   * the uv of the vertex (default: 0,0)
   */
  Vector2 uv;

}; // end of class PositionNormalTextureVertex

std::ostream&
operator<<(std::ostream& os,
           const PositionNormalTextureVertex& positionNormalTextureVertex);

} // namespace BABYLON

#endif // end of BABYLON_MATH_POSITION_NORMAL_TEXTURE_VERTEX_H
