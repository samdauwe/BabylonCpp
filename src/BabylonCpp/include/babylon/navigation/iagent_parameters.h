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
  float radius = -1.f;

  /**
   * Agent height. [Limit: > 0]
   */
  float height = 0.f;

  /**
   *  Maximum allowed acceleration. [Limit: >= 0]
   */
  float maxAcceleration = -1.f;

  /**
   * Maximum allowed speed. [Limit: >= 0]
   */
  float maxSpeed = -1.f;

  /**
   * Defines how close a collision element must be before it is considered for steering behaviors.
   * [Limits: > 0]
   */
  float collisionQueryRange = 0.f;

  /**
   * The path visibility optimization range. [Limit: > 0]
   */
  float pathOptimizationRange = 0.f;

  /**
   * How aggresive the agent manager should be at avoiding collisions with this agent. [Limit: >= 0]
   */
  float separationWeight = -1.f;
}; // end of struct IAgentParameters

} // end of namespace BABYLON

#endif // end of BABYLON_NAVIGATION_IAGENT_PARAMETERS_H
