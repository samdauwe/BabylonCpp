#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

#include <babylon/mesh/vertex_data.h>

namespace BABYLON {

_PrimitiveGeometry::_PrimitiveGeometry(const std::string& _id, Scene* scene,
                                       bool canBeRegenerated, Mesh* mesh)
    : Geometry(_id, scene, nullptr, false, mesh)
    , _canBeRegenerated{canBeRegenerated}
{
  _beingRegenerated = true;
  regenerate();
  _beingRegenerated = false;
}

_PrimitiveGeometry::~_PrimitiveGeometry()
{
}

bool _PrimitiveGeometry::canBeRegenerated() const
{
  return _canBeRegenerated;
}

void _PrimitiveGeometry::regenerate()
{
  if (!_canBeRegenerated) {
    return;
  }
  _beingRegenerated = true;
  setAllVerticesData(_regenerateVertexData().get(), false);
  _beingRegenerated = false;
}

GeometryPtr _PrimitiveGeometry::asNewGeometry(const std::string& iId)
{
  return Geometry::copy(iId);
}

void _PrimitiveGeometry::setAllVerticesData(VertexData* vertexData,
                                            bool /*updatable*/)
{
  if (!_beingRegenerated) {
    return;
  }
  Geometry::setAllVerticesData(vertexData, false);
}

AbstractMesh*
_PrimitiveGeometry::setVerticesData(unsigned int kind, const Float32Array& data,
                                    bool /*updatable*/,
                                    const std::optional<size_t>& /*stride*/)
{
  if (!_beingRegenerated) {
    return nullptr;
  }

  return Geometry::setVerticesData(kind, data, false);
}

json _PrimitiveGeometry::serialize() const
{
  auto serializationObject = Geometry::serialize();

  serializationObject["canBeRegenerated"] = canBeRegenerated();

  return serializationObject;
}

} // end of namespace BABYLON
