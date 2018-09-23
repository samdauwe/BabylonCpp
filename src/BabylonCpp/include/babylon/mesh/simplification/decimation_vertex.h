#ifndef BABYLON_MESH_SIMPLIFICATION_DECIMATION_VERTEX_H
#define BABYLON_MESH_SIMPLIFICATION_DECIMATION_VERTEX_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/simplification/quadratic_matrix.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT DecimationVertex {

public:
  DecimationVertex(const Vector3& position, int id);
  ~DecimationVertex();

  void updatePosition(const Vector3& newPosition);

public:
  QuadraticMatrix q;
  Vector3 position;
  int id;
  bool isBorder;
  int triangleStart;
  int triangleCount;
  Float32Array originalOffsets;

}; // end of class DecimationVertex

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_DECIMATION_VERTEX_H
