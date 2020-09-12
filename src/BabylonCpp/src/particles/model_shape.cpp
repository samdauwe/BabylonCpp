#include <babylon/particles/model_shape.h>

#include <babylon/maths/vector3.h>
#include <babylon/particles/solid_particle_vertex.h>

namespace BABYLON {

ModelShape::ModelShape(
  int id, const std::vector<Vector3>& shape, const IndicesArray& indices,
  const Float32Array& normals, const Float32Array& colors, const Float32Array& shapeUV,
  const std::function<void(SolidParticle* particle, size_t i, size_t s)>& posFunction,
  const std::function<void(SolidParticle* particle,
                           const std::variant<Vector3, SolidParticleVertex>& vertex,
                           unsigned int i)>& vtxFunction,
  const MaterialPtr& material)
{
  shapeID           = id;
  _shape            = shape;
  _indices          = indices;
  _indicesLength    = indices.size();
  _shapeUV          = shapeUV;
  _shapeColors      = colors;
  _normals          = normals;
  _positionFunction = posFunction;
  _vertexFunction   = vtxFunction;
  _material         = material;
}

ModelShape::~ModelShape() = default;

} // end of namespace BABYLON
