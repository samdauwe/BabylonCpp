#include <babylon/particles/model_shape.h>

#include <babylon/math/vector3.h>

namespace BABYLON {

ModelShape::ModelShape(
  int id, const vector_t<Vector3>& shape, const Float32Array& shapeUV,
  const ::std::function<void(SolidParticle* particle, unsigned int i,
                             unsigned int s)>& posFunction,
  const ::std::function<void(SolidParticle* particle, const Vector3& vertex,
                             unsigned int i)>& vtxFunction)
    : shapeID{id}
    , _shape{shape}
    , _shapeUV{shapeUV}
    , _positionFunction{posFunction}
    , _vertexFunction{vtxFunction}
{
}

ModelShape::~ModelShape()
{
}

} // end of namespace BABYLON
