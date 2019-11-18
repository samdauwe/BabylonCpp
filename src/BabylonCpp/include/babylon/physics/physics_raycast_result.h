#ifndef BABYLON_PHYSICS_PHYSICS_RAYCAST_RESULT_H
#define BABYLON_PHYSICS_PHYSICS_RAYCAST_RESULT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Interface for the size containing width and height
 */
struct IXYZ {
  /**
   * X
   */
  float x;

  /**
   * Y
   */
  float y;

  /**
   * Z
   */
  float z;
}; // end of struct IXYZ

/**
 * @brief Holds the data for the raycast result.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsRaycastResult {

public:
  /**
   * @brief Creates a raycast result.
   */
  PhysicsRaycastResult();
  PhysicsRaycastResult(const PhysicsRaycastResult& rhs);
  PhysicsRaycastResult(PhysicsRaycastResult&& rhs);
  PhysicsRaycastResult& operator=(const PhysicsRaycastResult& rhs);
  PhysicsRaycastResult& operator=(PhysicsRaycastResult&& rhs);
  ~PhysicsRaycastResult(); // = default

  /**
   * @brief Sets the hit data (normal & point in world space)
   * @param hitNormalWorld defines the normal in world space
   * @param hitPointWorld defines the point in world space
   */
  void setHitData(const IXYZ& hitNormalWorld, const IXYZ& hitPointWorld);

  /**
   * @brief Sets the distance from the start point to the hit point
   * @param distance
   */
  void setHitDistance(float distance);

  /**
   * @brief Calculates the distance manually
   */
  void calculateHitDistance();

  /**
   * @brief Resets all the values to default
   * @param from The from point on world space
   * @param to The to point on world space
   */
  void reset(const Vector3& from = Vector3::Zero(), const Vector3& to = Vector3::Zero());

protected:
  /**
   * @brief Gets if there was a hit
   */
  [[nodiscard]] bool get_hasHit() const;

  /**
   * @brief Gets the distance from the hit
   */
  [[nodiscard]] float get_hitDistance() const;

  /**
   * @brief Gets the hit normal/direction in the world
   */
  Vector3& get_hitNormalWorld();

  /**
   * @brief Gets the hit point in the world
   */
  Vector3& get_hitPointWorld();

  /**
   * @brief Gets the ray "start point" of the ray in the world
   */
  Vector3& get_rayFromWorld();

  /**
   * @brief Gets the ray "end point" of the ray in the world
   */
  Vector3& get_rayToWorld();

public:
  /**
   * Gets if there was a hit
   */
  ReadOnlyProperty<PhysicsRaycastResult, bool> hasHit;

  /**
   * Gets the distance from the hit
   */
  ReadOnlyProperty<PhysicsRaycastResult, float> hitDistance;

  /**
   * Gets the hit normal/direction in the world
   */
  ReadOnlyProperty<PhysicsRaycastResult, Vector3> hitNormalWorld;

  /**
   * Gets the hit point in the world
   */
  ReadOnlyProperty<PhysicsRaycastResult, Vector3> hitPointWorld;

  /**
   * Gets the ray "start point" of the ray in the world
   */
  ReadOnlyProperty<PhysicsRaycastResult, Vector3> rayFromWorld;

  /**
   * Gets the ray "end point" of the ray in the world
   */
  ReadOnlyProperty<PhysicsRaycastResult, Vector3> rayToWorld;

private:
  bool _hasHit;

  float _hitDistance;
  Vector3 _hitNormalWorld;
  Vector3 _hitPointWorld;
  Vector3 _rayFromWorld;
  Vector3 _rayToWorld;

}; // end of struct PhysicsRaycastResult

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_RAYCAST_RESULT_H
