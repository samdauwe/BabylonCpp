#ifndef BABYLON_PARTICLES_SOLID_PARTICLE_H
#define BABYLON_PARTICLES_SOLID_PARTICLE_H

#include <babylon/babylon_global.h>

#include <babylon/math/color4.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SolidParticle {

public:
  /**
   * Creates a Solid Particle object.
   * Don't create particles manually, use instead the Solid Particle System
   * internal tools like _addParticle()
   * `particleIndex` (integer) is the particle index in the Solid Particle
   * System pool. It's also the particle identifier.
   * `positionIndex` (integer) is the starting index of the particle vertices in
   * the SPS "positions" array.
   *  `model` (ModelShape) is a reference to the model shape on what the
   * particle is designed.
   * `shapeId` (integer) is the model shape identifier in the SPS.
   * `idxInShape` (integer) is the index of the particle in the current model
   * (ex: the 10th box of addShape(box, 30))
   * `modelBoundingInfo` is the reference to the model BoundingInfo used for
   * intersection computations.
   */
  SolidParticle(unsigned int particleIndex, unsigned int positionIndex,
                ModelShape* model, int shapeId, unsigned int idxInShape,
                SolidParticleSystem* sps,
                BoundingInfo* modelBoundingInfo = nullptr);
  ~SolidParticle();

  /**
   * Returns a boolean. True if the particle intersects another particle or
   * another mesh, else false.
   * The intersection is computed on the particle bounding sphere and Axis
   * Aligned Bounding Box (AABB)
   * `target` is the object (solid particle or mesh) what the intersection is
   * computed against.
   */
  bool intersectsMesh(Mesh* target) const;
  bool intersectsMesh(SolidParticle* target) const;

public:
  // particle global index
  unsigned int idx;
  // color
  Color4* color;
  // position
  Vector3 position;
  // rotation
  Vector3 rotation;
  // quaternion, will overwrite rotation
  Quaternion* rotationQuaternion;
  // scale
  Vector3 scaling;
  // uvs
  Vector4 uvs;
  // velocity
  Vector3 velocity;
  // alive
  bool alive;
  // visibility
  bool isVisible;
  // index of this particle in the global "positions" array
  unsigned int _pos;
  // model shape reference
  ModelShape* _model;
  // model shape id
  int shapeId;
  // index of the particle in its shape id
  unsigned int idxInShape;
  // reference to the shape model BoundingInfo object
  BoundingInfo* _modelBoundingInfo;
  // particle BoundingInfo
  BoundingInfo* _boundingInfo;
  // reference to the SPS what the particle belongs to
  SolidParticleSystem* _sps;
  std::unordered_map<std::string, float> extraFields;

}; // end of class SolidParticle

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SOLID_PARTICLE_H
