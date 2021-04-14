#ifndef BABYLON_EXTENSIONS_RECASTJS_RECASTJS_CROWD_H
#define BABYLON_EXTENSIONS_RECASTJS_RECASTJS_CROWD_H

#include <babylon/babylon_api.h>
#include <babylon/misc/observer.h>
#include <babylon/navigation/icrowd.h>

namespace BABYLON {
namespace Extensions {

class Crowd;
class RecastJSPlugin;

/**
 * @brief Recast detour crowd implementation.
 */
class BABYLON_SHARED_EXPORT RecastJSCrowd : public ICrowd {

public:
  /**
   * @brief Constructor.
   * @param plugin recastJS plugin
   * @param maxAgents the maximum agent count in the crowd
   * @param maxAgentRadius the maximum radius an agent can have
   * @param scene to attach the crowd to
   * @returns the crowd you can add agents to
   */
  RecastJSCrowd(RecastJSPlugin* plugin, size_t maxAgents, float maxAgentRadius, Scene* scene);
  ~RecastJSCrowd() override;

  /**
   * @brief Add a new agent to the crowd with the specified parameter a corresponding transformNode.
   * You can attach anything to that node. The node position is updated in the scene update tick.
   * @param pos world position that will be constrained by the navigation mesh
   * @param parameters agent parameters
   * @param transform hooked to the agent that will be update by the scene
   * @returns agent index
   */
  int addAgent(const Vector3& pos, const IAgentParameters& parameters,
               const TransformNodePtr& transform) override;

  /**
   * @brief Returns the agent position in world space.
   * @param index agent index returned by addAgent
   * @returns world space position
   */
  Vector3 getAgentPosition(int index) override;

  /**
   * @brief Returns the agent position result in world space.
   * @param index agent index returned by addAgent
   * @param result output world space position
   */
  void getAgentPositionToRef(int index, Vector3& result) override;

  /**
   * @brief Returns the agent velocity in world space.
   * @param index agent index returned by addAgent
   * @returns world space velocity
   */
  Vector3 getAgentVelocity(int index) override;

  /**
   * @brief Returns the agent velocity result in world space.
   * @param index agent index returned by addAgent
   * @param result output world space velocity
   */
  void getAgentVelocityToRef(int index, Vector3& result) override;

  /**
   * @brief Returns the agent next target point on the path
   * @param index agent index returned by addAgent
   * @returns world space position
   */
  Vector3 getAgentNextTargetPath(int index) override;

  /**
   * @brief Returns the agent next target point on the path
   * @param index agent index returned by addAgent
   * @param result output world space position
   */
  void getAgentNextTargetPathToRef(int index, Vector3& result) override;

  /**
   * @brief Gets the agent state
   * @param index agent index returned by addAgent
   * @returns agent state
   */
  int getAgentState(int index) override;

  /**
   * @brief Returns true if the agent in over an off mesh link connection
   * @param index agent index returned by addAgent
   * @returns true if over an off mesh link connection
   */
  bool overOffmeshConnection(int index) override;

  /**
   * @brief Asks a particular agent to go to a destination. That destination is constrained by the
   * navigation mesh.
   * @param index agent index returned by addAgent
   * @param destination targeted world position
   */
  void agentGoto(int index, const Vector3& destination) override;

  /**
   * @brief Teleport the agent to a new position.
   * @param index agent index returned by addAgent
   * @param destination targeted world position
   */
  void agentTeleport(int index, const Vector3& destination) override;

  /**
   * @brief Update agent parameters.
   * @param index agent index returned by addAgent
   * @param parameters agent parameters
   */
  void updateAgentParameters(int index, const IAgentParameters& parameters) override;

  /**
   * @brief Remove a particular agent previously created.
   * @param index agent index returned by addAgent
   */
  void removeAgent(int index) override;

  /**
   * @brief Get the list of all agents attached to this crowd.
   * @returns list of agent indices
   */
  std::vector<int>& getAgents() override;

  /**
   * @brief Tick update done by the Scene. Agent position/velocity/acceleration is updated by this
   * function.
   * @param deltaTime in seconds
   */
  void update(float deltaTime) override;

  /**
   * @brief Set the Bounding box extent for doing spatial queries (getClosestPoint,
   * getRandomPointAround, ...) The queries will try to find a solution within those bounds default
   * is (1,1,1)
   * @param extent x,y,z value that define the extent around the queries point of reference
   */
  void setDefaultQueryExtent(const Vector3& extent) override;

  /**
   * @brief Get the Bounding box extent specified by setDefaultQueryExtent.
   * @returns the box extent values
   */
  Vector3 getDefaultQueryExtent() const override;

  /**
   * @brief Get the Bounding box extent result specified by setDefaultQueryExtent.
   * @param result output the box extent values
   */
  void getDefaultQueryExtentToRef(Vector3& result) override;

  /**
   * @brief Get the next corner points composing the path (max 4 points).
   * @param index agent index returned by addAgent
   * @returns array containing world position composing the path
   */
  std::vector<Vector3> getCorners(int index) override;

  /**
   * @brief Release all resources.
   */
  void dispose() override;

public:
  /**
   * Recast/detour plugin
   */
  RecastJSPlugin* bjsRECASTPlugin;

  /**
   * Link to the detour crowd
   */
  std::unique_ptr<Crowd> recastCrowd;

  /**
   * One transform per agent
   */
  std::vector<TransformNodePtr> transforms;

  /**
   * All agents created
   */
  std::vector<int> agents;

private:
  /**
   * Link to the scene is kept to unregister the crowd from the scene
   */
  Scene* _scene;

  /**
   * Observer for crowd updates
   */
  Observer<Scene>::Ptr _onBeforeAnimationsObserver;

}; // end of class RecastJSCrowd

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_RECASTJS_RECASTJS_CROWD_H
