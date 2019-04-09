#include <babylon/morph/morph_target.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

MorphTarget::MorphTarget(const std::string& name, float iInfluence,
                         Scene* scene)
    : influence{this, &MorphTarget::get_influence, &MorphTarget::set_influence}
    , animationPropertiesOverride{this,
                                  &MorphTarget::get_animationPropertiesOverride,
                                  &MorphTarget::set_animationPropertiesOverride}
    , hasPositions{this, &MorphTarget::get_hasPositions}
    , hasNormals{this, &MorphTarget::get_hasNormals}
    , hasTangents{this, &MorphTarget::get_hasTangents}
    , _name{name}
    , _scene{scene ? scene : Engine::LastCreatedScene()}
    , _influence{iInfluence}
    , _animationPropertiesOverride{nullptr}
{
}

MorphTarget::~MorphTarget()
{
}

Type MorphTarget::type() const
{
  return Type::MORPHTARGET;
}

float MorphTarget::get_influence() const
{
  return _influence;
}

void MorphTarget::set_influence(float iInfluence)
{
  if (stl_util::almost_equal(_influence, iInfluence)) {
    return;
  }

  auto previous = _influence;
  _influence    = iInfluence;

  if (onInfluenceChanged.hasObservers()) {
    auto value = (previous == 0.f || iInfluence == 0.f);
    onInfluenceChanged.notifyObservers(&value);
  }
}

AnimationPropertiesOverride*& MorphTarget::get_animationPropertiesOverride()
{
  if (!_animationPropertiesOverride && _scene) {
    return _scene->animationPropertiesOverride;
  }
  return _animationPropertiesOverride;
}

void MorphTarget::set_animationPropertiesOverride(
  AnimationPropertiesOverride* const& value)
{
  _animationPropertiesOverride = value;
}

bool MorphTarget::get_hasPositions() const
{
  return !_positions.empty();
}

bool MorphTarget::get_hasNormals() const
{
  return !_normals.empty();
}

bool MorphTarget::get_hasTangents() const
{
  return !_tangents.empty();
}

void MorphTarget::setPositions(const Float32Array& data)
{
  const auto hadPositions = hasPositions();

  _positions = data;

  if (hadPositions != hasPositions) {
    _onDataLayoutChanged.notifyObservers(nullptr);
  }
}

Float32Array& MorphTarget::getPositions()
{
  return _positions;
}

void MorphTarget::setNormals(const Float32Array& data)
{
  const auto hadNormals = hasNormals();

  _normals = data;

  if (hadNormals != hasNormals) {
    _onDataLayoutChanged.notifyObservers(nullptr);
  }
}

const Float32Array& MorphTarget::getPositions() const
{
  return _positions;
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
  const auto hadTangents = hasTangents();

  _tangents = data;

  if (hadTangents != hasTangents) {
    _onDataLayoutChanged.notifyObservers(nullptr);
  }
}

Float32Array& MorphTarget::getTangents()
{
  return _tangents;
}

const Float32Array& MorphTarget::getTangents() const
{
  return _tangents;
}

json MorphTarget::serialize() const
{
  return nullptr;
}

std::unique_ptr<MorphTarget> MorphTarget::Parse(const json& serializationObject)
{
  auto result = std::make_unique<MorphTarget>(
    json_util::get_string(serializationObject, "name"),
    json_util::get_number<float>(serializationObject, "influence", 0.f));

  result->setPositions(
    json_util::get_array<float>(serializationObject, "positions"));

  if (json_util::has_key(serializationObject, "normals")) {
    result->setNormals(
      json_util::get_array<float>(serializationObject, "normals"));
  }

  if (json_util::has_key(serializationObject, "tangents")) {
    result->setNormals(
      json_util::get_array<float>(serializationObject, "tangents"));
  }

  // Animations
  if (json_util::has_key(serializationObject, "animations")) {
    for (auto parsedAnimation :
         json_util::get_array<json>(serializationObject, "animations")) {
      result->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
  }

  return result;
}

MorphTargetPtr MorphTarget::FromMesh(const AbstractMeshPtr& mesh,
                                     std::string name, float influence)
{
  if (name.empty()) {
    name = mesh->name;
  }

  auto result = MorphTarget::New(name, influence, mesh->getScene());

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
