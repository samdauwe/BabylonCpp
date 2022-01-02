#ifndef BABYLON_PARTICLES_MODEL_SHAPE_H
#define BABYLON_PARTICLES_MODEL_SHAPE_H

#include <functional>
#include <memory>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class SolidParticle;
class SolidParticleVertex;
class Vector3;
FWD_CLASS_SPTR(Material)

/**
 * @brief Represents the shape of the model used by one particle of a solid particle system.
 * SPS internal tool, don't use it manually.
 */
class BABYLON_SHARED_EXPORT ModelShape {

public:
  /**
   * @brief Creates a ModelShape object. This is an internal simplified reference to a mesh used as
   * for a model to replicate particles from by the SPS. SPS internal tool, don't use it manually.
   * @hidden
   */
  ModelShape(int id, const std::vector<Vector3>& shape, const IndicesArray& indices,
             const Float32Array& normals, const Float32Array& colors, const Float32Array& shapeUV,
             const std::function<void(SolidParticle* particle, size_t i, size_t s)>& posFunction,
             const std::function<void(SolidParticle* particle,
                                      const std::variant<Vector3, SolidParticleVertex>& vertex,
                                      unsigned int i)>& vtxFunction,
             const MaterialPtr& material);
  ~ModelShape(); // = default

public:
  /**
   * The shape id.
   * Hidden
   */
  int shapeID;
  /**
   * flat array of model positions (internal use)
   * Hidden
   */
  std::vector<Vector3> _shape;
  /**
   * flat array of model UVs (internal use)
   * Hidden
   */
  Float32Array _shapeUV;
  /**
   * color array of the model
   * @hidden
   */
  Float32Array _shapeColors;
  /**
   * indices array of the model
   * @hidden
   */
  IndicesArray _indices;
  /**
   * normals array of the model
   * @hidden
   */
  Float32Array _normals;
  /**
   * length of the shape in the model indices array (internal use)
   * Hidden
   */
  size_t _indicesLength;
  /**
   * Custom position function (internal use)
   * Hidden
   */
  std::function<void(SolidParticle* particle, size_t i, size_t s)> _positionFunction;
  /**
   * Custom vertex function (internal use)
   * Hidden
   */
  std::function<void(SolidParticle* particle,
                     const std::variant<Vector3, SolidParticleVertex>& vertex, unsigned int i)>
    _vertexFunction;
  /**
   * Model material (internal use)
   * @hidden
   */
  MaterialPtr _material;

}; // end of class ModelShape

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_MODEL_SHAPE_H
