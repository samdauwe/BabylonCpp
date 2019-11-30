#ifndef BABYLON_NAVIGATION_IAGENT_PARAMETERS_H
#define BABYLON_NAVIGATION_IAGENT_PARAMETERS_H

namespace BABYLON {

/**
 * @brief Configures an agent.
 */
struct IAgentParameters {
  /**
   *  Agent radius. [Limit: >= 0]
   */
  float radius;

  /**
   * Agent height. [Limit: > 0]
   */
  float height;

  /**
   *  Maximum allowed acceleration. [Limit: >= 0]
   */
  float maxAcceleration;

  /**
   * Maximum allowed speed. [Limit: >= 0]
   */
  float maxSpeed;

  /**
   * Defines how close a collision element must be before it is considered for steering behaviors.
   * [Limits: > 0]
   */
  float collisionQueryRange;

  /**
   * The path visibility optimization range. [Limit: > 0]
   */
  float pathOptimizationRange;

  /**
   * How aggresive the agent manager should be at avoiding collisions with this agent. [Limit: >= 0]
   */
  float separationWeight;
}; // end of struct IAgentParameters

} // end of namespace BABYLON

#endif // end of BABYLON_NAVIGATION_IAGENT_PARAMETERS_H
