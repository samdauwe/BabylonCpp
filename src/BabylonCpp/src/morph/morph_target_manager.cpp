#include <babylon/morph/morph_target_manager.h>

#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

MorphTargetManager::MorphTargetManager(Scene* scene)
    : uniqueId{this, &MorphTargetManager::get_uniqueId}
    , vertexCount{this, &MorphTargetManager::get_vertexCount}
    , supportsNormals{this, &MorphTargetManager::get_supportsNormals}
    , supportsTangents{this, &MorphTargetManager::get_supportsTangents}
    , numTargets{this, &MorphTargetManager::get_numTargets}
    , numInfluencers{this, &MorphTargetManager::get_numInfluencers}
    , influences{this, &MorphTargetManager::get_influences}
    , _supportsNormals{false}
    , _supportsTangents{false}
    , _vertexCount{0}
    , _uniqueId{0}
{
  _scene = scene ? scene : Engine::LastCreatedScene();
}

MorphTargetManager::~MorphTargetManager()
{
}

void MorphTargetManager::addToScene(
  const MorphTargetManagerPtr& newMorphTargetManager)
{
  if (_scene) {
    _scene->morphTargetManagers.emplace_back(newMorphTargetManager);

    _uniqueId = _scene->getUniqueId();
  }
}

size_t MorphTargetManager::get_uniqueId() const
{
  return _uniqueId;
}

size_t MorphTargetManager::get_vertexCount() const
{
  return _vertexCount;
}

bool MorphTargetManager::get_supportsNormals() const
{
  return _supportsNormals;
}

bool MorphTargetManager::get_supportsTangents() const
{
  return _supportsTangents;
}

std::size_t MorphTargetManager::get_numTargets() const
{
  return _targets.size();
}

size_t MorphTargetManager::get_numInfluencers() const
{
  return _activeTargets.size();
}

Float32Array& MorphTargetManager::get_influences()
{
  return _influences;
}

MorphTargetPtr MorphTargetManager::getActiveTarget(size_t index)
{
  if (index < _activeTargets.size()) {
    return _activeTargets[index];
  }

  return nullptr;
}

MorphTargetPtr MorphTargetManager::getTarget(std::size_t index)
{
  if (index < _targets.size()) {
    return _targets[index];
  }

  return nullptr;
}

void MorphTargetManager::addTarget(const MorphTargetPtr& target)
{
  _targets.emplace_back(target);
  _targetInfluenceChangedObservers.emplace_back(
    _targets.back()->onInfluenceChanged.add(
      [this](bool* needUpdate, EventState&) {
        _syncActiveTargets(*needUpdate);
      }));
  _targetDataLayoutChangedObservers.emplace_back(
    _targets.back()->_onDataLayoutChanged.add(
      [this](void*, EventState&) { _syncActiveTargets(true); }));
  _syncActiveTargets(true);
}

void MorphTargetManager::removeTarget(MorphTarget* target)
{
  auto it = std::find_if(_targets.begin(), _targets.end(),
                         [target](const MorphTargetPtr& morphTarget) {
                           return target == morphTarget.get();
                         });
  if (it != _targets.end()) {
    _targets.erase(it);

    size_t index = static_cast<size_t>(it - _targets.begin());
    target->onInfluenceChanged.remove(_targetInfluenceChangedObservers[index]);
    target->_onDataLayoutChanged.remove(
      _targetDataLayoutChangedObservers[index]);
    _syncActiveTargets(true);
  }
}

json MorphTargetManager::serialize()
{
  return nullptr;
}

void MorphTargetManager::_syncActiveTargets(bool needUpdate)
{
  size_t influenceCount = 0;
  _activeTargets.clear();
  _tempInfluences.clear();
  _influences.clear();
  _supportsNormals  = true;
  _supportsTangents = true;
  _vertexCount      = 0;

  for (auto& target : _targets) {
    if (target->influence == 0.f) {
      continue;
    }

    _activeTargets.emplace_back(target);
    _tempInfluences.emplace_back(target->influence());
    ++influenceCount;

    _supportsNormals  = _supportsNormals && target->hasNormals();
    _supportsTangents = _supportsTangents && target->hasTangents();

    const auto& positions = target->getPositions();
    if (!positions.empty()) {
      const auto iVertexCount = positions.size() / 3;
      if (_vertexCount == 0) {
        _vertexCount = iVertexCount;
      }
      else if (_vertexCount != iVertexCount) {
        BABYLON_LOG_ERROR(
          "MorphTargetManager",
          "Incompatible target. Targets must all have the same vertices "
          "count.")
        return;
      }
    }
  }

  if (_influences.size() != influenceCount) {
    _influences = _tempInfluences;
  }

  if (needUpdate) {
    synchronize();
  }
}

void MorphTargetManager::synchronize()
{
  if (!_scene) {
    return;
  }
  // Flag meshes as dirty to resync with the active targets
  for (auto& abstractMesh : _scene->meshes) {
    auto mesh = std::static_pointer_cast<Mesh>(abstractMesh);
    if (mesh && (mesh->morphTargetManager().get() == this)) {
      mesh->_syncGeometryWithMorphTargetManager();
    }
  }
}

MorphTargetManagerPtr MorphTargetManager::Parse(const json& serializationObject,
                                                Scene* scene)
{
  auto result = MorphTargetManager::New(scene);

  result->_uniqueId
    = json_util::get_number<unsigned>(serializationObject, "id", 0);

  if (json_util::has_key(serializationObject, "targets")
      && (serializationObject["targets"].is_array())) {
    for (auto& targetData :
         json_util::get_array<json>(serializationObject, "targets")) {
      result->addTarget(MorphTarget::Parse(targetData));
    }
  }

  return result;
}

} // end of namespace BABYLON
