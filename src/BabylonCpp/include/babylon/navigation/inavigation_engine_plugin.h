#ifndef BABYLON_NAVIGATION_INAVIGATION_ENGINE_PLUGIN_H
#define BABYLON_NAVIGATION_INAVIGATION_ENGINE_PLUGIN_H

#include <memory>
#include <vector>

namespace BABYLON {

struct ICrowd;
struct INavMeshParameters;
class Mesh;
class Scene;
class Vector3;
using ICrowdPtr = std::shared_ptr<ICrowd>;
using MeshPtr   = std::shared_ptr<Mesh>;

/**
 * @brief Navigation plugin interface to add navigation constrained by a navigation mesh.
 */
struct INavigationEnginePlugin {

  /**
   * @brief Constructor
   */
  INavigationEnginePlugin() = default;
  virtual ~INavigationEnginePlugin()= default;

  /**
   * plugin name
   */
  std::string name;

  /**
   * @brief Creates a navigation mesh.
   * @param meshes array of all the geometry used to compute the navigatio mesh
   * @param parameters bunch of parameters used to filter geometry
   */
  virtual void createMavMesh(const std::vector<MeshPtr>& meshes,
                             const INavMeshParameters& parameters)
    = 0;

  /**
   * @brief Create a navigation mesh debug mesh.
   * @param scene is where the mesh will be added
   * @returns debug display mesh
   */
  virtual MeshPtr createDebugNavMesh(Scene* scene) const = 0;

  /**
   * @brief Get a navigation mesh constrained position, closest to the parameter position.
   * @param position world position
   * @returns the closest point to position constrained by the navigation mesh
   */
  virtual Vector3 getClosestPoint(const Vector3& position) = 0;

  /**
   * @brief Get a navigation mesh constrained position, within a particular radius.
   * @param position world position
   * @param maxRadius the maximum distance to the constrained world position
   * @returns the closest point to position constrained by the navigation mesh
   */
  virtual Vector3 getRandomPointAround(const Vector3& position, float maxRadius) = 0;

  /**
   * @brief Compute the final position from a segment made of destination-position.
   * @param position world position
   * @param destination world position
   * @returns the resulting point along the navmesh
   */
  virtual Vector3 moveAlong(const Vector3& position, const Vector3& destination) = 0;

  /**
   * @brief Compute a navigation path from start to end. Returns an empty array if no path can be
   * computed.
   * @param start world position
   * @param end world position
   * @returns array containing world position composing the path
   */
  virtual std::vector<Vector3> computePath(const Vector3& start, const Vector3& end) = 0;

  /**
   * @brief If this plugin is supported.
   * @returns true if plugin is supported
   */
  virtual bool isSupported() const = 0;

  /**
   * @brief Create a new Crowd so you can add agents.
   * @param maxAgents the maximum agent count in the crowd
   * @param maxAgentRadius the maximum radius an agent can have
   * @param scene to attach the crowd to
   * @returns the crowd you can add agents to
   */
  virtual ICrowdPtr createCrowd(size_t maxAgents, float maxAgentRadius, Scene* scene) = 0;

  /**
   * @brief Set the Bounding box extent for doing spatial queries (getClosestPoint,
   * getRandomPointAround,
   * ...) The queries will try to find a solution within those bounds default is (1,1,1).
   * @param extent x,y,z value that define the extent around the queries point of reference
   */
  virtual void setDefaultQueryExtent(const Vector3& extent) = 0;

  /**
   * @brief Get the Bounding box extent specified by setDefaultQueryExtent.
   * @returns the box extent values
   */
  virtual Vector3 getDefaultQueryExtent() const = 0;

  /**
   * @brief Release all resources.
   */
  virtual void dispose() = 0;
}; // end of struct INavigationEnginePlugin

} // end of namespace BABYLON

#endif // end of BABYLON_NAVIGATION_INAVIGATION_ENGINE_PLUGIN_H
