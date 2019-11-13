#ifndef BABYLON_PARTICLES_SOLID_PARTICLE_H
#define BABYLON_PARTICLES_SOLID_PARTICLE_H

#include <babylon/babylon_api.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/math/color4.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

class Mesh;
class ModelShape;
class SolidParticleSystem;

/**
 * @brief Represents one particle of a solid particle system.
 */
class BABYLON_SHARED_EXPORT SolidParticle {

public:
  /**
   * @brief Creates a Solid Particle object.
   * Don't create particles manually, use instead the Solid Particle System
   * internal tools like _addParticle()
   * @param particleIndex (integer) is the particle index in the Solid Particle
   * System pool. It's also the particle identifier.
   * @param positionIndex (integer) is the starting index of the particle
   * vertices in the SPS "positions" array.
   * @param indiceIndex (integer) is the starting index of the particle indices
   * in the SPS "indices" array.
   * @param model (ModelShape) is a reference to the model shape on what the
   * particle is designed.
   * @param shapeId (integer) is the model shape identifier in the SPS.
   * @param idxInShape (integer) is the index of the particle in the current
   * model (ex: the 10th box of addShape(box, 30))
   * @param sps defines the sps it is associated to
   * @param modelBoundingInfo is the reference to the model BoundingInfo used
   * for intersection computations.
   */
  SolidParticle(unsigned int particleIndex, unsigned int positionIndex,
                unsigned int indiceIndex, ModelShape* model, int shapeId,
                unsigned int idxInShape, SolidParticleSystem* sps,
                const std::optional<BoundingInfo>& modelBoundingInfo
                = std::nullopt);
  ~SolidParticle(); // = default

  /**
   * @brief Returns a boolean. True if the particle intersects another particle
   * or another mesh, else false. The intersection is computed on the particle
   * bounding sphere and Axis Aligned Bounding Box (AABB)
   * @param target is the object (solid particle or mesh) what the intersection
   * is computed against.
   * @returns true if it intersects
   */
  bool intersectsMesh(Mesh* target) const;
  bool intersectsMesh(SolidParticle* target) const;

  /**
   * @brief Returns `true` if the solid particle is within the frustum defined
   * by the passed array of planes. A particle is in the frustum if its bounding
   * box intersects the frustum
   * @param frustumPlanes defines the frustum to test
   * @returns true if the particle is in the frustum planes
   */
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes);

  /**
   * @brief Get the rotation matrix of the particle.
   * @hidden
   */
  void getRotationMatrix(Matrix& m) const;

public:
  /**
   * particle global index
   */
  unsigned int idx;
  /**
   * The color of the particle
   */
  std::optional<Color4> color;
  /**
   * The world space position of the particle.
   */
  Vector3 position;
  /**
   * The world space rotation of the particle. (Not use if rotationQuaternion is
   * set)
   */
  Vector3 rotation;
  /**
   * The world space rotation quaternion of the particle.
   */
  std::unique_ptr<Quaternion> rotationQuaternion;
  /**
   * The scaling of the particle.
   */
  Vector3 scaling;
  /**
   * The uvs of the particle.
   */
  Vector4 uvs;
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
   * In this case, the pivot point is set at the origin of the particle local
   * space and the particle is translated. Default : false
   */
  bool translateFromPivot;
  /**
   * Is the particle active or not ?
   */
  bool alive;
  /**
   * Is the particle visible or not ?
   */
  bool isVisible;
  /**
   * Index of this particle in the global "positions" array (Internal use)
   * Hidden
   */
  unsigned int _pos;
  /**
   * Index of this particle in the global "indices" array (Internal use)
   */
  unsigned int _ind;
  /**
   * ModelShape of this particle (Internal use)
   * Hidden
   */
  ModelShape* _model;
  /**
   * ModelShape id of this particle
   */
  int shapeId;
  /**
   * Index of the particle in its shape id
   */
  unsigned int idxInShape;
  /**
   * Reference to the shape model BoundingInfo object (Internal use)
   * Hidden
   */
  std::unique_ptr<BoundingInfo> _modelBoundingInfo;
  /**
   * Particle BoundingInfo object (Internal use)
   * Hidden
   */
  std::unique_ptr<BoundingInfo> _boundingInfo;
  /**
   * Reference to the SPS what the particle belongs to (Internal use)
   * Hidden
   */
  SolidParticleSystem* _sps;
  /**
   * Still set as invisible in order to skip useless computations (Internal use)
   * Hidden
   */
  bool _stillInvisible;
  /**
   * Last computed particle rotation matrix
   * Hidden
   */
  Float32Array _rotationMatrix;
  /**
   * Parent particle Id, if any.
   * Default null.
   */
  std::optional<unsigned int> parentId;
  /**
   * The culling strategy to use to check whether the solid particle must be
   * culled or not when using isInFrustum(). The possible values are :
   * - AbstractMesh.CULLINGSTRATEGY_STANDARD
   * - AbstractMesh.CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY
   * - AbstractMesh.CULLINGSTRATEGY_OPTIMISTIC_INCLUSION
   * - AbstractMesh.CULLINGSTRATEGY_OPTIMISTIC_INCLUSION_THEN_BSPHERE_ONLY
   * The default value for solid particles is
   * AbstractMesh.CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY Please read each static
   * variable documentation in the class AbstractMesh to get details about the
   * culling process.
   * */
  unsigned int cullingStrategy;
  /**
   * Internal global position in the SPS.
   * Hidden
   */
  Vector3 _globalPosition;

  std::unordered_map<std::string, float> extraFields;

}; // end of class SolidParticle

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SOLID_PARTICLE_H
