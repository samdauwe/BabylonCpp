#include <babylon/morph/morph_target.h>

#include <babylon/core/json.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

MorphTarget::MorphTarget(const std::string& name, float influence)
    : _name{name}, _influence{std::numeric_limits<float>::max()}
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
  if (std_util::almost_equal(_influence, influence)) {
    return;
  }

  float previous = _influence;
  _influence     = influence;

  if (onInfluenceChanged.hasObservers()) {
    auto value
      = std_util::make_unique<bool>(previous == 0.f || influence == 0.f);
    onInfluenceChanged.notifyObservers(value.get());
  }
}

bool MorphTarget::hasNormals() const
{
  return !_normals.empty();
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

Json::object MorphTarget::serialize() const
{
  return Json::object();
}

std::unique_ptr<MorphTarget>
MorphTarget::Parse(const Json::value& serializationObject)
{
  auto result = std_util::make_unique<MorphTarget>(
    Json::GetString(serializationObject, "name"),
    Json::GetNumber<float>(serializationObject, "influence", 0.f));

  result->setPositions(Json::ToArray<float>(serializationObject, "positions"));

  if (serializationObject.contains("normals")) {
    result->setNormals(Json::ToArray<float>(serializationObject, "normals"));
  }

  return result;
}

std::unique_ptr<MorphTarget> MorphTarget::FromMesh(AbstractMesh* mesh,
                                                   const std::string& name,
                                                   float influence)
{
  auto morphTargetName = name;
  if (morphTargetName.empty()) {
    morphTargetName = mesh->name;
  }

  auto result = std_util::make_unique<MorphTarget>(morphTargetName, influence);

  result->setPositions(mesh->getVerticesData(VertexBuffer::PositionKind));

  if (mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
    result->setNormals(mesh->getVerticesData(VertexBuffer::NormalKind));
  }

  return result;
}

} // end of namespace BABYLON
