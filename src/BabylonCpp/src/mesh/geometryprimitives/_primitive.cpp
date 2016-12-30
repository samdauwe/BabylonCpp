#include <babylon/mesh/geometryprimitives/_primitive.h>

#include <babylon/mesh/vertex_data.h>

namespace BABYLON {
namespace GeometryPrimitives {

_Primitive::_Primitive(const std::string& _id, Scene* scene,
                       bool canBeRegenerated, Mesh* mesh)
    : Geometry(_id, scene, nullptr, false, mesh)
    , _canBeRegenerated{canBeRegenerated}
{
  _beingRegenerated = true;
  regenerate();
  _beingRegenerated = false;
}

_Primitive::~_Primitive()
{
}

bool _Primitive::canBeRegenerated() const
{
  return _canBeRegenerated;
}

void _Primitive::regenerate()
{
  if (!_canBeRegenerated) {
    return;
  }
  _beingRegenerated          = true;
  auto regeneratedVertexData = _regenerateVertexData();
  setAllVerticesData(regeneratedVertexData.get(), false);
  _beingRegenerated = false;
}

Geometry* _Primitive::asNewGeometry(const std::string& iId)
{
  return Geometry::copy(iId);
}

void _Primitive::setAllVerticesData(VertexData* vertexData, bool /*updatable*/)
{
  if (!_beingRegenerated) {
    return;
  }
  Geometry::setAllVerticesData(vertexData, false);
}

void _Primitive::setVerticesData(unsigned int kind, const Float32Array& data,
                                 bool /*updatable*/, int /*stride*/)
{
  if (!_beingRegenerated) {
    return;
  }

  Geometry::setVerticesData(kind, data, false);
}

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON
