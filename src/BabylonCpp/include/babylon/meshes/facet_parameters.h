#ifndef BABYLON_MESHES_FACET_PARAMETERS_H
#define BABYLON_MESHES_FACET_PARAMETERS_H

#include <babylon/babylon_api.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

struct DepthSortedFacet {
  unsigned int ind;
  float sqDistance;
}; // end of struct DepthSortedFacets

/**
 * @brief Facet parameters.
 */
struct BABYLON_SHARED_EXPORT FacetParameters {

  FacetParameters();
  ~FacetParameters(); // = default

  // Array of facet positions (vector3)
  std::vector<Vector3> facetPositions;
  // Array of facet normals (vector3)
  std::vector<Vector3> facetNormals;
  // Partitioning array, facetPositions is required for facetPartitioning computation
  std::vector<Uint32Array> facetPartitioning;
  // Partitioning data about subdivsions on each axis (int), required for facetPartitioning
  // computation
  SubdivisionsPerAxis subDiv;
  // Partitioning ratio / bounding box, required for facetPartitioning computation
  std::optional<float> ratio;
  // Bounding box size data, required for facetPartitioning computation
  std::optional<Vector3> bbSize;
  // Bounding info, required for facetPartitioning computation
  BoundingInfo bInfo;
  // Whether or not to use a right-handed coordinate system
  bool useRightHandedSystem;
  // Whether or not to enable the facet depth sort computation
  bool depthSort;
  // Vector3 to compute the facet depth from this location
  std::optional<Vector3> distanceTo;
  // Array of depthSortedFacets to store the facet distances from the reference location
  std::vector<DepthSortedFacet> depthSortedFacets;

}; // end of struct FacetParameters

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_FACET_PARAMETERS_H
