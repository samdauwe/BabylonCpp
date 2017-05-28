#ifndef BABYLON_MESH_FACET_PARAMETERS_H
#define BABYLON_MESH_FACET_PARAMETERS_H

#include <babylon/babylon_global.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Facet parameters.
 */
class BABYLON_SHARED_EXPORT FacetParameters {

public:
  FacetParameters();
  ~FacetParameters();

public:
  // Array of facet positions (vector3)
  std::vector<Vector3> facetPositions;
  // Array of facet normals (vector3)
  std::vector<Vector3> facetNormals;
  // Partitioning array, facetPositions is required for facetPartitioning
  // computation
  std::vector<Uint32Array> facetPartitioning;
  // Partitioning data about subdivsions on each axis (int), required for
  // facetPartitioning computation
  SubdivisionsPerAxis subDiv;
  // Partitioning ratio / bounding box, required for facetPartitioning
  // computation
  float ratio;
  // Bounding box size data, required for facetPartitioning computation
  Vector3 bbSize;
  // Bounding info, required for facetPartitioning computation
  BoundingInfo bInfo;
  // Whether or not to use a right-handed coordinate system
  bool useRightHandedSystem;

}; // end of class FacetParameters

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_FACET_PARAMETERS_H
