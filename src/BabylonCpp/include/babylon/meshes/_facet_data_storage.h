#ifndef BABYLON_MESHES_FACET_DATA_STORAGE_H
#define BABYLON_MESHES_FACET_DATA_STORAGE_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/maths/vector3.h>
#include <babylon/meshes/facet_parameters.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _FacetDataStorage {
  // FacetData private properties
  // Facet local positions
  std::vector<Vector3> facetPositions;
  // Facet local normals
  std::vector<Vector3> facetNormals;
  // Partitioning array of facet index arrays
  std::vector<Uint32Array> facetPartitioning;
  // facet number
  size_t facetNb = 0;
  // Number of subdivisions per axis in the partitioning space
  unsigned int partitioningSubdivisions = 10;
  // The partitioning array space is by default 1% bigger than the bounding box
  float partitioningBBoxRatio = 1.01f;
  // Is the facet data feature enabled on this mesh ?
  bool facetDataEnabled = false;
  // Keep a reference to the object parameters to avoid memory re-allocation
  FacetParameters facetParameters;
  // bbox size approximated for facet data
  Vector3 bbSize = Vector3::Zero();
  // Actual number of subdivisions per axis for ComputeNormals()
  SubdivisionsPerAxis subDiv{
    1, // max
    1, // X
    1, // Y
    1  // Z
  };

  // is the facet depth sort to be computed
  bool facetDepthSort = false;
  // is the facet depth sort initialized
  bool facetDepthSortEnabled = false;
  // copy of the indices array to store them once sorted
  IndicesArray depthSortedIndices;
  // array of depth sorted facets
  std::vector<DepthSortedFacet> depthSortedFacets;
  // facet depth sort function
  std::function<int(const DepthSortedFacet& f1, const DepthSortedFacet& f2)> facetDepthSortFunction
    = nullptr;
  // location where to depth sort from
  std::optional<Vector3> facetDepthSortFrom = std::nullopt;
  // same as facetDepthSortFrom but expressed in the mesh local space
  Vector3 facetDepthSortOrigin;
  Matrix invertedMatrix;

}; // end of class _FacetDataStorage

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_FACET_DATA_STORAGE_H
