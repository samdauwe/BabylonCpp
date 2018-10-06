#ifndef BABYLON_MATH_POSITION_NORMAL_VERTEX_H
#define BABYLON_MATH_POSITION_NORMAL_VERTEX_H

#include <memory>
#include <ostream>

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Contains position and normal vectors for a vertex
 */
class BABYLON_SHARED_EXPORT PositionNormalVertex {

public:
  /**
   * @brief Creates a PositionNormalVertex.
   * @param position the position of the vertex (defaut: 0,0,0)
   * @param normal the normal of the vertex (defaut: 0,1,0)
   */
  PositionNormalVertex();
  PositionNormalVertex(const Vector3& position, const Vector3& normal);
  PositionNormalVertex(const PositionNormalVertex& other);
  PositionNormalVertex(PositionNormalVertex&& other);
  PositionNormalVertex& operator=(const PositionNormalVertex& other);
  PositionNormalVertex& operator=(PositionNormalVertex&& other);
  ~PositionNormalVertex();
  PositionNormalVertex copy() const;

  /**
   * @brief Clones the PositionNormalVertex.
   * @returns the cloned PositionNormalVertex
   */
  std::unique_ptr<PositionNormalVertex> clone() const;

  friend std::ostream&
  operator<<(std::ostream& os,
             const PositionNormalVertex& positionNormalVertex);

public:
  /**
   * the position of the vertex (defaut: 0,0,0)
   */
  Vector3 position;

  /**
   * the normal of the vertex (defaut: 0,1,0)
   */
  Vector3 normal;

}; // end of class PositionNormalVertex

std::ostream& operator<<(std::ostream& os,
                         const PositionNormalVertex& positionNormalVertex);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_POSITION_NORMAL_VERTEX_H
