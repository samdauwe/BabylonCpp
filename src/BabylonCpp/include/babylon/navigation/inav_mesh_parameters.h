#ifndef BABYLON_NAVIGATION_INAV_MESH_PARAMETERS_H
#define BABYLON_NAVIGATION_INAV_MESH_PARAMETERS_H

namespace BABYLON {

/**
 * @brief Configures the navigation mesh creation.
 */
struct INavMeshParameters {
  /**
   * The xz-plane cell size to use for fields. [Limit: > 0] [Units: wu]
   */
  float cs;

  /**
   * The y-axis cell size to use for fields. [Limit: > 0] [Units: wu]
   */
  float ch;

  /**
   * The maximum slope that is considered walkable. [Limits: 0 <= value < 90] [Units: Degrees]
   */
  float walkableSlopeAngle;

  /**
   * Minimum floor to 'ceiling' height that will still allow the floor area to
   * be considered walkable. [Limit: >= 3] [Units: vx]
   */
  int walkableHeight;

  /**
   * Maximum ledge height that is considered to still be traversable. [Limit: >=0] [Units: vx]
   */
  int walkableClimb;

  /**
   * The distance to erode/shrink the walkable area of the heightfield away from
   * obstructions.  [Limit: >=0] [Units: vx]
   */
  int walkableRadius;

  /**
   * The maximum allowed length for contour edges along the border of the mesh. [Limit: >=0] [Units:
   * vx]
   */
  int maxEdgeLen;

  /**
   * The maximum distance a simplfied contour's border edges should deviate
   * the original raw contour. [Limit: >=0] [Units: vx]
   */
  float maxSimplificationError;

  /**
   * The minimum number of cells allowed to form isolated island areas. [Limit: >=0] [Units: vx]
   */
  int minRegionArea;

  /**
   * Any regions with a span count smaller than this value will, if possible,
   * be merged with larger regions. [Limit: >=0] [Units: vx]
   */
  int mergeRegionArea;

  /**
   * The maximum number of vertices allowed for polygons generated during the
   * contour to polygon conversion process. [Limit: >= 3]
   */
  int maxVertsPerPoly;

  /**
   * Sets the sampling distance to use when generating the detail mesh.
   * (For height detail only.) [Limits: 0 or >= 0.9] [Units: wu]
   */
  float detailSampleDist;

  /**
   * The maximum distance the detail mesh surface should deviate from heightfield
   * data. (For height detail only.) [Limit: >=0] [Units: wu]
   */
  float detailSampleMaxError;
}; // end of struct INavMeshParameters

} // end of namespace BABYLON

#endif // end of BABYLON_NAVIGATION_INAV_MESH_PARAMETERS_H
