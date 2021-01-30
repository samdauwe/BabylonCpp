#ifndef BABYLON_EXTENSIONS_RECASTJS_RECASTJS_PLUGIN_H
#define BABYLON_EXTENSIONS_RECASTJS_RECASTJS_PLUGIN_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/recastjs/recastjs.h>
#include <babylon/navigation/inavigation_engine_plugin.h>

namespace BABYLON {
namespace Extensions {

/**
 * @brief RecastJS navigation plugin.
 */
class BABYLON_SHARED_EXPORT RecastJSPlugin : public INavigationEnginePlugin {

public:
  RecastJSPlugin();
  ~RecastJSPlugin() override;

  /**
   * @brief Set the time step of the navigation tick update.
   * Default is 1/60.
   * A value of 0 will disable fixed time update
   * @param newTimeStep the new timestep to apply to this world.
   */
  void setTimeStep(float newTimeStep = 1.f / 60.f) override;

  /**
   * @brief Get the time step of the navigation tick update.
   * @returns the current time step
   */
  float getTimeStep() const override;

  /**
   * @brief If delta time in navigation tick update is greater than the time step
   * a number of sub iterations are done. If more iterations are need to reach deltatime
   * they will be discarded.
   * A value of 0 will set to no maximum and update will use as many substeps as needed
   * @param newStepCount the maximum number of iterations
   */
  void setMaximumSubStepCount(unsigned int newStepCount = 10) override;

  /**
   * @brief Get the maximum number of iterations per navigation tick update
   * @returns the maximum number of iterations
   */
  unsigned int getMaximumSubStepCount() const override;

  /**
   * @brief Creates a navigation mesh.
   * @param meshes array of all the geometry used to compute the navigation mesh
   * @param parameters bunch of parameters used to filter geometry
   */
  void createNavMesh(const std::vector<MeshPtr>& meshes,
                     const INavMeshParameters& parameters) override;

  /**
   * @brief Create a navigation mesh debug mesh.
   * @param scene is where the mesh will be added
   * @returns debug display mesh
   */
  MeshPtr createDebugNavMesh(Scene* scene) const override;

  /**
   * @brief Get a navigation mesh constrained position, closest to the parameter position.
   * @param position world position
   * @returns the closest point to position constrained by the navigation mesh
   */
  Vector3 getClosestPoint(const Vector3& position) override;

  /**
   * @brief Get a navigation mesh constrained position, closest to the parameter position.
   * @param position world position
   * @param result output the closest point to position constrained by the navigation mesh
   */
  void getClosestPointToRef(const Vector3& position, Vector3& result) override;

  /**
   * @brief Get a navigation mesh constrained position, within a particular radius.
   * @param position world position
   * @param maxRadius the maximum distance to the constrained world position
   * @returns the closest point to position constrained by the navigation mesh
   */
  Vector3 getRandomPointAround(const Vector3& position, float maxRadius) override;

  /**
   * @brief Get a navigation mesh constrained position, within a particular radius.
   * @param position world position
   * @param maxRadius the maximum distance to the constrained world position
   * @param result output the closest point to position constrained by the navigation mesh
   */
  void getRandomPointAroundToRef(const Vector3& position, float maxRadius,
                                 Vector3& result) override;

  /**
   * @brief Compute the final position from a segment made of destination-position
   * @param position world position
   * @param destination world position
   * @returns the resulting point along the navmesh
   */
  Vector3 moveAlong(const Vector3& position, const Vector3& destination) override;

  /**
   * @brief Compute the final position from a segment made of destination-position.
   * @param position world position
   * @param destination world position
   * @param result output the resulting point along the navmesh
   */
  void moveAlongToRef(const Vector3& position, const Vector3& destination,
                      Vector3& result) override;

  /**
   * @brief Compute a navigation path from start to end. Returns an empty array if no path can be
   * computed.
   * @param start world position
   * @param end world position
   * @returns array containing world position composing the path
   */
  std::vector<Vector3> computePath(const Vector3& start, const Vector3& end) override;

  /**
   * @brief Create a new Crowd so you can add agents.
   * @param maxAgents the maximum agent count in the crowd
   * @param maxAgentRadius the maximum radius an agent can have
   * @param scene to attach the crowd to
   * @returns the crowd you can add agents to
   */
  ICrowdPtr createCrowd(size_t maxAgents, float maxAgentRadius, Scene* scene) override;

  /**
   * @brief Set the Bounding box extent for doing spatial queries (getClosestPoint,
   * getRandomPointAround,
   * ...) The queries will try to find a solution within those bounds default is (1,1,1)
   * @param extent x,y,z value that define the extent around the queries point of reference
   */
  void setDefaultQueryExtent(const Vector3& extent) override;

  /**
   * @brief Get the Bounding box extent specified by setDefaultQueryExtent.
   * @returns the box extent values
   */
  Vector3 getDefaultQueryExtent() const override;

  /**
   * @brief build the navmesh from a previously saved state using getNavmeshData.
   * @param data the Uint8Array returned by getNavmeshData
   */
  void buildFromNavmeshData(const Uint8Array& data) override;

  /**
   * @brief returns the navmesh data that can be used later. The navmesh must be built before
   * retrieving the data.
   * @returns data the Uint8Array that can be saved and reused
   */
  Uint8Array getNavmeshData() override;

  /**
   * @brief Get the Bounding box extent result specified by setDefaultQueryExtent.
   * @param result output the box extent values
   */
  void getDefaultQueryExtentToRef(Vector3& result) override;

  /**
   * @brief Disposes
   */
  void dispose() override;

  /**
   * @brief If this plugin is supported.
   * @returns true if plugin is supported
   */
  bool isSupported() const override;

public:
  /**
   * The first navmesh created. We might extend this to support multiple navmeshes
   */
  std::unique_ptr<NavMesh> navMesh;

private:
  unsigned int _maximumSubStepCount;
  float _timeStep;

}; // end of class RecastJSPlugin

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_RECASTJS_RECASTJS_PLUGIN_H
