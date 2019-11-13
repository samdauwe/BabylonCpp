#include <babylon/particles/model_shape.h>

#include <babylon/math/vector3.h>

namespace BABYLON {

ModelShape::ModelShape(
  int id, const std::vector<Vector3>& shape, size_t indicesLength,
  const Float32Array& shapeUV,
  const std::function<void(SolidParticle* particle, unsigned int i,
                           unsigned int s)>& posFunction,
  const std::function<void(SolidParticle* particle, const Vector3& vertex,
                           unsigned int i)>& vtxFunction)
    : shapeID{id}
    , _shape{shape}
    , _shapeUV{shapeUV}
    , _indicesLength{indicesLength}
    , _positionFunction{posFunction}
    , _vertexFunction{vtxFunction}
{
}

ModelShape::~ModelShape() = default;

} // end of namespace BABYLON
