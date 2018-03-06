#include <babylon/morph/morph_target_manager.h>

#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

MorphTargetManager::MorphTargetManager(Scene* scene)
    : _supportsNormals{false}
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
  unique_ptr_t<MorphTargetManager>&& newMorphTargetManager)
{
  _scene->morphTargetManagers.emplace_back(::std::move(newMorphTargetManager));
}

size_t MorphTargetManager::uniqueId() const
{
  return _uniqueId;
}

size_t MorphTargetManager::vertexCount() const
{
  return _vertexCount;
}

bool MorphTargetManager::supportsNormals() const
{
  return _supportsNormals;
}

bool MorphTargetManager::supportsTangents() const
{
  return _supportsTangents;
}

std::size_t MorphTargetManager::numTargets() const
{
  return _targets.size();
}

size_t MorphTargetManager::numInfluencers() const
{
  return _activeTargets.size();
}

Float32Array& MorphTargetManager::influences()
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

void MorphTargetManager::addTarget(unique_ptr_t<MorphTarget>&& target)
{
  _targets.emplace_back(::std::move(target));
  _targetObservable.emplace_back(_targets.back()->onInfluenceChanged.add(
    [this](bool needUpdate, EventState&) { _syncActiveTargets(needUpdate); }));
  _syncActiveTargets(true);
}

void MorphTargetManager::removeTarget(MorphTarget* target)
{
  auto it
    = ::std::find_if(_targets.begin(), _targets.end(),
                     [target](const shared_ptr_t<MorphTarget>& morphTarget) {
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

  if (needUpdate && _scene) {
    // Flag meshes as dirty to resync with the active targets
    for (auto& abstractMesh : _scene->meshes) {
      auto mesh = static_cast<Mesh*>(abstractMesh.get());
      if (mesh && (mesh->morphTargetManager() == this)) {
        mesh->_syncGeometryWithMorphTargetManager();
      }
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
