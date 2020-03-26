#ifndef BABYLON_PARTICLES_CLOUD_POINT_H
#define BABYLON_PARTICLES_CLOUD_POINT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class BoundingInfo;
class Color4;
class Mesh;
class PointsCloudSystem;
class PointsGroup;
class Vector2;
class Vector3;
using MeshPtr        = std::shared_ptr<Mesh>;
using PointsGroupPtr = std::shared_ptr<PointsGroup>;

/**
 * @brief Represents one particle of a points cloud system.
 */
class BABYLON_SHARED_EXPORT CloudPoint {

public:
  /**
   * @brief Creates a Point Cloud object.
   * Don't create particles manually, use instead the PCS internal tools like _addParticle()
   * @param particleIndex (integer) is the particle index in the PCS pool. It's also the particle
   * identifier.
   * @param group (PointsGroup) is the group the particle belongs to
   * @param groupId (integer) is the group identifier in the PCS.
   * @param idxInGroup (integer) is the index of the particle in the current point group (ex: the
   * 10th point of addPoints(30))
   * @param pcs defines the PCS it is associated to
   */
  CloudPoint(size_t particleIndex, const PointsGroupPtr& group, size_t groupId, size_t idxInGroup,
             PointsCloudSystem* pcs);
  ~CloudPoint(); // default

  /**
   * @brief  Returns a boolean. True if the particle intersects a mesh, else false
   * The intersection is computed on the particle position and Axis Aligned Bounding Box (AABB) or
   * Sphere
   * @param target is the object (point or mesh) what the intersection is computed against
   * @param isSphere is boolean flag when false (default) bounding box of mesh is used, when true
   * the bouding sphere is used
   * @returns true if it intersects
   */
  bool intersectsMesh(const MeshPtr& target, bool isSphere) const;

  /**
   * @brief Gets the rotation matrix of the particle.
   * @hidden
   */
  void getRotationMatrix(Matrix& m);

protected:
  /**
   * @brief Get point size.
   */
  Vector3& get_size();

  /**
   * @brief Set point size.
   */
  void set_size(const Vector3& scale);

  /**
   * @brief Legacy support, changed quaternion to rotationQuaternion.
   */
  std::unique_ptr<Quaternion>& get_quaternion();

  /**
   * @brief Legacy support, changed quaternion to rotationQuaternion.
   */
  void set_quaternion(const std::unique_ptr<Quaternion>& q);

public:
  /**
   * particle global index
   */
  size_t idx;

  /**
   * The color of the particle
   */
  std::unique_ptr<Color4> color;

  /**
   * The world space position of the particle.
   */
  Vector3 position;

  /**
   * The world space rotation of the particle. (Not use if rotationQuaternion is set)
   */
  Vector3 rotation;

  /**
   * The world space rotation quaternion of the particle.
   */
  std::unique_ptr<Quaternion> rotationQuaternion;

  /**
   * The uv of the particle.
   */
  std::unique_ptr<Vector2> uv;

  /**
   * The current speed of the particle.
   */
  Vector3 velocity;

  /**
   * The pivot point in the particle local space.
   */
  Vector3 pivot;

  /**
   * Must the particle be translated from its pivot point in its local space ?
   * In this case, the pivot point is set at the origin of the particle local space and the particle
   * is translated. Default : false
   */
  bool translateFromPivot;

  /**
   * Index of this particle in the global "positions" array (Internal use)
   * @hidden
   */
  size_t _pos;

  /**
   * @hidden Index of this particle in the global "indices" array (Internal use)
   */
  size_t _ind;

  /**
   * Group this particle belongs to
   */
  PointsGroupPtr _group;

  /**
   * Group id of this particle
   */
  size_t groupId;

  /**
   * Index of the particle in its group id (Internal use)
   */
  size_t idxInGroup;

  /**
   * @hidden Particle BoundingInfo object (Internal use)
   */
  std::unique_ptr<BoundingInfo> _boundingInfo;

  /**
   * @hidden Reference to the PCS that the particle belongs to (Internal use)
   */
  PointsCloudSystem* _pcs;

  /**
   * @hidden Still set as invisible in order to skip useless computations (Internal use)
   */
  bool _stillInvisible;

  /**
   * @hidden Last computed particle rotation matrix
   */
  Float32Array _rotationMatrix;

  /**
   * Parent particle Id, if any.
   * Default null.
   */
  std::optional<size_t> parentId;

  /**
   * @hidden Internal global position in the PCS.
   */
  Vector3 _globalPosition;

  /**
   * Gets or sets the point size.
   */
  Property<CloudPoint, Vector3> size;

  /**
   * Gets or sets the world space rotation quaternion of the particle.
   */
  Property<CloudPoint, std::unique_ptr<Quaternion>> quaternion;

}; // end of class CloudPoint

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_CLOUD_POINT_H
