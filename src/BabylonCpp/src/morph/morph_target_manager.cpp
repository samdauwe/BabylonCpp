#include <babylon/morph/morph_target_manager.h>

#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>

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

  if (_scene) {
    _scene->morphTargetManagers.emplace_back(this);
    _uniqueId = _scene->getUniqueId();
  }
}

MorphTargetManager::~MorphTargetManager()
{
}

void MorphTargetManager::addToScene(
  std::unique_ptr<MorphTargetManager>&& newMorphTargetManager)
{
  _scene->morphTargetManagers.emplace_back(std::move(newMorphTargetManager));
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

MorphTarget* MorphTargetManager::getActiveTarget(size_t index)
{
  if (index < _activeTargets.size()) {
    return _activeTargets[index];
  }

  return nullptr;
}

MorphTarget* MorphTargetManager::getTarget(std::size_t index)
{
  if (index < _targets.size()) {
    return _targets[index].get();
  }

  return nullptr;
}

void MorphTargetManager::addTarget(std::unique_ptr<MorphTarget>&& target)
{
  _targets.emplace_back(std::move(target));
  _targetObservable.emplace_back(_targets.back()->onInfluenceChanged.add(
    [this](bool needUpdate, EventState&) { _syncActiveTargets(needUpdate); }));
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
    target->onInfluenceChanged.remove(_targetObservable[index]);
    _syncActiveTargets(true);
  }
}

Json::object MorphTargetManager::serialize()
{
  return Json::object();
}

void MorphTargetManager::_syncActiveTargets(bool needUpdate)
{
  std::size_t influenceCount = 0;
  _activeTargets.clear();
  _tempInfluences.clear();
  _influences.clear();
  _supportsNormals  = true;
  _supportsTangents = true;
  _vertexCount      = 0;

  for (auto& target : _targets) {
    _activeTargets.emplace_back(target.get());
    _tempInfluences.emplace_back(target->influence());
    ++influenceCount;

    auto& positions = target->getPositions();
    if (!positions.empty()) {
      _supportsNormals  = _supportsNormals && target->hasNormals();
      _supportsTangents = _supportsTangents && target->hasTangents();

      const auto vertexCount = positions.size() / 3;
      if (_vertexCount == 0) {
        _vertexCount = vertexCount;
      }
      else if (_vertexCount != vertexCount) {
        BABYLON_LOG_ERROR(
          "MorphTargetManager",
          "Incompatible target. Targets must all have the same vertices "
          "count.");
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

MorphTargetManager*
MorphTargetManager::Parse(const Json::value& serializationObject, Scene* scene)
{
  auto result = MorphTargetManager::New(scene);

  result->_uniqueId = Json::GetNumber<unsigned>(serializationObject, "id", 0);

  if (serializationObject.contains("targets")
      && (serializationObject.get("targets").is<picojson::array>())) {
    for (auto& targetData :
         serializationObject.get("targets").get<picojson::array>()) {
      result->addTarget(MorphTarget::Parse(targetData));
    }
  }

  return result;
}

} // end of namespace BABYLON
