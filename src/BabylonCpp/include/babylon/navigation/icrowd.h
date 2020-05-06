#ifndef BABYLON_NAVIGATION_ICROWD_H
#define BABYLON_NAVIGATION_ICROWD_H

#include <memory>
#include <vector>

namespace BABYLON {

struct IAgentParameters;
class Scene;
class TransformNode;
class Vector3;

using TransformNodePtr = std::shared_ptr<TransformNode>;

/**
 * @brief Crowd Interface. A Crowd is a collection of moving agents constrained by a navigation
 * mesh.
 */
struct ICrowd {

  /**
   * @brief Destructor
   */
  virtual ~ICrowd() = default;

  /**
   * @brief Add a new agent to the crowd with the specified parameter a corresponding transformNode.
   * You can attach anything to that node. The node position is updated in the scene update tick.
   * @param pos world position that will be constrained by the navigation mesh
   * @param parameters agent parameters
   * @param transform hooked to the agent that will be update by the scene
   * @returns agent index
   */
  virtual int addAgent(const Vector3& pos, const IAgentParameters& parameters,
                       const TransformNodePtr& transform)
    = 0;

  /**
   * @brief Returns the agent position in world space.
   * @param index agent index returned by addAgent
   * @returns world space position
   */
  virtual Vector3 getAgentPosition(int index) = 0;

  /**
   * @brief Gets the agent position result in world space.
   * @param index agent index returned by addAgent
   * @param result output world space position
   */
  virtual void getAgentPositionToRef(int index, Vector3& result) = 0;

  /**
   * @brief Gets the agent velocity in world space.
   * @param index agent index returned by addAgent
   * @returns world space velocity
   */
  virtual Vector3 getAgentVelocity(int index) = 0;

  /**
   * @brief Gets the agent velocity result in world space.
   * @param index agent index returned by addAgent
   * @param result output world space velocity
   */
  virtual void getAgentVelocityToRef(int index, Vector3& result) = 0;

  /**
   * @brief remove a particular agent previously created.
   * @param index agent index returned by addAgent
   */
  virtual void removeAgent(int index) = 0;

  /**
   * @brief get the list of all agents attached to this crowd.
   * @returns list of agent indices
   */
  virtual std::vector<int>& getAgents() = 0;

  /**
   * @brief Tick update done by the Scene. Agent position/velocity/acceleration is updated by this
   * function.
   * @param deltaTime in seconds
   */
  virtual void update(float deltaTime) = 0;

  /**
   * @brief Asks a particular agent to go to a destination. That destination is constrained by the
   * navigation mesh.
   * @param index agent index returned by addAgent
   * @param destination targeted world position
   */
  virtual void agentGoto(int index, const Vector3& destination) = 0;

  /**
   * @brief Teleport the agent to a new position.
   * @param index agent index returned by addAgent
   * @param destination targeted world position
   */
  virtual void agentTeleport(int index, const Vector3& destination) = 0;

  /**
   * @brief Update agent parameters.
   * @param index agent index returned by addAgent
   * @param parameters agent parameters
   */
  virtual void updateAgentParameters(int index, const IAgentParameters& parameters) = 0;

  /**
   * @brief Set the Bounding box extent for doing spatial queries (getClosestPoint,
   * getRandomPointAround, ...). The queries will try to find a solution within those bounds default
   * is (1,1,1)
   * @param extent x,y,z value that define the extent around the queries point of reference
   */
  virtual void setDefaultQueryExtent(const Vector3& extent) = 0;

  /**
   * @brief Get the Bounding box extent specified by setDefaultQueryExtent.
   * @returns the box extent values
   */
  virtual Vector3 getDefaultQueryExtent() const = 0;

  /**
   * @brief Get the Bounding box extent result specified by setDefaultQueryExtent.
   * @param result output the box extent values
   */
  virtual void getDefaultQueryExtentToRef(Vector3& result) = 0;

  /**
   * @brief Release all resources.
   */
  virtual void dispose() = 0;

}; // end of struct ICrowd

} // end of namespace BABYLON

#endif // end of BABYLON_NAVIGATION_ICROWD_H
