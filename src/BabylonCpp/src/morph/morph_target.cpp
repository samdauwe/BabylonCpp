#include <babylon/morph/morph_target.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

MorphTarget::MorphTarget(const std::string& name, float iInfluence, Scene* scene)
    : influence{this, &MorphTarget::get_influence, &MorphTarget::set_influence}
    , uniqueId{this, &MorphTarget::get_uniqueId}
    , hasPositions{this, &MorphTarget::get_hasPositions}
    , hasNormals{this, &MorphTarget::get_hasNormals}
    , hasTangents{this, &MorphTarget::get_hasTangents}
    , hasUVs{this, &MorphTarget::get_hasUVs}
    , _name{name}
    , _scene{scene ? scene : Engine::LastCreatedScene()}
    , _influence{-1.f} // -1  means Undefined
    , _uniqueId{0}
    , _animationPropertiesOverride{nullptr}
{
  influence = iInfluence;

  if (_scene) {
    _uniqueId = _scene->getUniqueId();
  }
}

MorphTarget::~MorphTarget() = default;

Type MorphTarget::type() const
{
  return Type::MORPHTARGET;
}

AnimationValue MorphTarget::getProperty(const std::vector<std::string>& targetPropertyPath)
{
  if (targetPropertyPath.size() == 1) {
    const auto& target = targetPropertyPath[0];
    if (target == "influence") {
      return influence();
    }
  }

  return AnimationValue();
}

void MorphTarget::setProperty(const std::vector<std::string>& targetPropertyPath,
                              const AnimationValue& value)
{
  const auto animationType = value.animationType();
  if (animationType.has_value()) {
    if (targetPropertyPath.size() == 1) {
      const auto& target = targetPropertyPath[0];
      if (*animationType == Animation::ANIMATIONTYPE_FLOAT()) {
        auto floatValue = value.get<float>();
        if (target == "influence") {
          influence = floatValue;
        }
      }
    }
  }
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

AnimationPropertiesOverridePtr& MorphTarget::get_animationPropertiesOverride()
{
  if (!_animationPropertiesOverride && _scene) {
    return _scene->animationPropertiesOverride;
  }
  return _animationPropertiesOverride;
}

void MorphTarget::set_animationPropertiesOverride(const AnimationPropertiesOverridePtr& value)
{
  _animationPropertiesOverride = value;
}

size_t MorphTarget::get_uniqueId() const
{
  return _uniqueId;
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

bool MorphTarget::get_hasUVs() const
{
  return !_uvs.empty();
}

std::vector<AnimationPtr> MorphTarget::getAnimations()
{
  return animations;
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

void MorphTarget::setUVs(const Float32Array& data)
{
  const auto hadUVs = hasUVs();

  _uvs = data;

  if (hadUVs != hasUVs) {
    _onDataLayoutChanged.notifyObservers(nullptr);
  }
}

const Float32Array& MorphTarget::getUVs() const
{
  return _uvs;
}

MorphTargetPtr MorphTarget::clone()
{
  return nullptr;
}

json MorphTarget::serialize() const
{
  return nullptr;
}

std::string MorphTarget::getClassName() const
{
  return "MorphTarget";
}

std::unique_ptr<MorphTarget> MorphTarget::Parse(const json& serializationObject)
{
  auto result = std::make_unique<MorphTarget>(
    json_util::get_string(serializationObject, "name"),
    json_util::get_number<float>(serializationObject, "influence", 0.f));

  result->setPositions(json_util::get_array<float>(serializationObject, "positions"));

  if (json_util::has_valid_key_value(serializationObject, "id")) {
    result->id = json_util::get_string(serializationObject, "id");
  }

  if (json_util::has_key(serializationObject, "normals")) {
    result->setNormals(json_util::get_array<float>(serializationObject, "normals"));
  }

  if (json_util::has_key(serializationObject, "tangents")) {
    result->setNormals(json_util::get_array<float>(serializationObject, "tangents"));
  }

  if (json_util::has_key(serializationObject, "uvs")) {
    result->setUVs(json_util::get_array<float>(serializationObject, "uvs"));
  }

  // Animations
  if (json_util::has_key(serializationObject, "animations")) {
    for (auto parsedAnimation : json_util::get_array<json>(serializationObject, "animations")) {
      result->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
  }

  return result;
}

MorphTargetPtr MorphTarget::FromMesh(const AbstractMeshPtr& mesh, std::string name, float influence)
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

  if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
    result->setUVs(mesh->getVerticesData(VertexBuffer::UVKind));
  }

  return result;
}

} // end of namespace BABYLON
