#ifndef BABYLON_PARTICLES_MODEL_SHAPE_H
#define BABYLON_PARTICLES_MODEL_SHAPE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents the shape of the model used by one particle of a solid
 * particle system. SPS internal tool, don't use it manually.
 * @see SolidParticleSystem
 */
class BABYLON_SHARED_EXPORT ModelShape {

public:
  /**
   * @brief Creates a ModelShape object. This is an internal simplified
   * reference to a mesh used as for a model to replicate particles from by the
   * SPS. SPS internal tool, don't use it manually.
   */
  ModelShape(
    int id, const vector_t<Vector3>& shape, size_t indicesLength,
    const Float32Array& shapeUV,
    const ::std::function<void(SolidParticle* particle, unsigned int i,
                               unsigned int s)>& posFunction,
    const ::std::function<void(SolidParticle* particle, const Vector3& vertex,
                               unsigned int i)>& vtxFunction);
  ~ModelShape();

public:
  /**
   * The shape id.
   */
  int shapeID;
  /**
   * flat array of model positions (internal use)
   */
  vector_t<Vector3> _shape;
  /**
   * flat array of model UVs (internal use)
   */
  Float32Array _shapeUV;
  /**
   * length of the shape in the model indices array (internal use)
   */
  size_t _indicesLength;
  /**
   * Custom position function (internal use)
   */
  ::std::function<void(SolidParticle* particle, unsigned int i, unsigned int s)>
    _positionFunction;
  /**
   * Custom vertex function (internal use)
   */
  ::std::function<void(SolidParticle* particle, const Vector3& vertex,
                       unsigned int i)>
    _vertexFunction;

}; // end of class ModelShape

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_MODEL_SHAPE_H
