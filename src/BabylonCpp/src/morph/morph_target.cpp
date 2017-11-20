#include <babylon/morph/morph_target.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

MorphTarget::MorphTarget(const string_t& name, float influence)
    : _name{name}, _influence{numeric_limits_t<float>::max()}
{
  setInfluence(influence);
}

MorphTarget::~MorphTarget()
{
}

float MorphTarget::influence() const
{
  return _influence;
}

void MorphTarget::setInfluence(float influence)
{
  if (stl_util::almost_equal(_influence, influence)) {
    return;
  }

  float previous = _influence;
  _influence     = influence;

  if (onInfluenceChanged.hasObservers()) {
    auto value = ::std::make_unique<bool>(previous == 0.f || influence == 0.f);
    onInfluenceChanged.notifyObservers(value.get());
  }
}

bool MorphTarget::hasNormals() const
{
  return !_normals.empty();
}

bool MorphTarget::hasTangents() const
{
  return !_tangents.empty();
}

vector_t<Animation*>& MorphTarget::animations()
{
  return _animations;
}

const vector_t<Animation*>& MorphTarget::animations() const
{
  return _animations;
}

void MorphTarget::setPositions(const Float32Array& data)
{
  _positions = data;
}

Float32Array& MorphTarget::getPositions()
{
  return _positions;
}

const Float32Array& MorphTarget::getPositions() const
{
  return _positions;
}

void MorphTarget::setNormals(const Float32Array& data)
{
  _normals = data;
}

Float32Array& MorphTarget::getNormals()
{
  return _normals;
}

const Float32Array& MorphTarget::getNormals() const
{
  return _normals;
}

void MorphTarget::setTangents(const Float32Array& data)
{
  _tangents = data;
}

Float32Array& MorphTarget::getTangents()
{
  return _tangents;
}

const Float32Array& MorphTarget::getTangents() const
{
  return _tangents;
}

Json::object MorphTarget::serialize() const
{
  return Json::object();
}

unique_ptr_t<MorphTarget>
MorphTarget::Parse(const Json::value& serializationObject)
{
  auto result = ::std::make_unique<MorphTarget>(
    Json::GetString(serializationObject, "name"),
    Json::GetNumber<float>(serializationObject, "influence", 0.f));

  result->setPositions(Json::ToArray<float>(serializationObject, "positions"));

  if (serializationObject.contains("normals")) {
    result->setNormals(Json::ToArray<float>(serializationObject, "normals"));
  }

  if (serializationObject.contains("tangents")) {
    result->setNormals(Json::ToArray<float>(serializationObject, "tangents"));
  }

  return result;
}

unique_ptr_t<MorphTarget>
MorphTarget::FromMesh(AbstractMesh* mesh, const string_t& name, float influence)
{
  auto morphTargetName = name;
  if (morphTargetName.empty()) {
    morphTargetName = mesh->name;
  }

  auto result = ::std::make_unique<MorphTarget>(morphTargetName, influence);

  result->setPositions(mesh->getVerticesData(VertexBuffer::PositionKind));

  if (mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
    result->setNormals(mesh->getVerticesData(VertexBuffer::NormalKind));
  }

  if (mesh->isVerticesDataPresent(VertexBuffer::TangentKind)) {
    result->setTangents(mesh->getVerticesData(VertexBuffer::TangentKind));
  }

  return result;
}

} // end of namespace BABYLON
