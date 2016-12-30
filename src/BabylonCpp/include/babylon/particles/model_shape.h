#ifndef BABYLON_PARTICLES_MODEL_SHAPE_H
#define BABYLON_PARTICLES_MODEL_SHAPE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ModelShape {

public:
  /**
   * Creates a ModelShape object. This is an internal simplified reference to a
   * mesh used as for a model to replicate particles from by the SPS.
   * SPS internal tool, don't use it manually.
   */
  ModelShape(
    int id, const std::vector<Vector3>& shape, const Float32Array& shapeUV,
    const std::function<void(SolidParticle* particle, unsigned int i,
                             unsigned int s)>& posFunction,
    const std::function<void(SolidParticle* particle, const Vector3& vertex,
                             unsigned int i)>& vtxFunction);
  ~ModelShape();

public:
  int shapeID;
  std::vector<Vector3> _shape;
  Float32Array _shapeUV;
  std::function<void(SolidParticle* particle, unsigned int i, unsigned int s)>
    _positionFunction;
  std::function<void(SolidParticle* particle, const Vector3& vertex,
                     unsigned int i)>
    _vertexFunction;

}; // end of class ModelShape

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_MODEL_SHAPE_H
