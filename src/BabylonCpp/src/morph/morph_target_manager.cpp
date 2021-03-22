#include <babylon/morph/morph_target_manager.h>

#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/raw_texture_2d_array.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

MorphTargetManager::MorphTargetManager(Scene* scene)
    : _targetStoreTexture{nullptr}
    , optimizeInfluencers{true}
    , enableNormalMorphing{true}
    , enableTangentMorphing{true}
    , enableUVMorphing{true}
    , uniqueId{this, &MorphTargetManager::get_uniqueId}
    , vertexCount{this, &MorphTargetManager::get_vertexCount}
    , supportsNormals{this, &MorphTargetManager::get_supportsNormals}
    , supportsTangents{this, &MorphTargetManager::get_supportsTangents}
    , supportsUVs{this, &MorphTargetManager::get_supportsUVs}
    , numTargets{this, &MorphTargetManager::get_numTargets}
    , numInfluencers{this, &MorphTargetManager::get_numInfluencers}
    , influences{this, &MorphTargetManager::get_influences}
    , useTextureToStoreTargets{this, &MorphTargetManager::get_useTextureToStoreTargets,
                               &MorphTargetManager::set_useTextureToStoreTargets}
    , isUsingTextureForTargets{this, &MorphTargetManager::get_isUsingTextureForTargets}
    , _supportsNormals{false}
    , _supportsTangents{false}
    , _supportsUVs{false}
    , _vertexCount{0}
    , _textureVertexStride{0}
    , _textureWidth{0}
    , _textureHeight{1}
    , _uniqueId{0}
    , _canUseTextureForTargets{false}
    , _useTextureToStoreTargets{true}
{
  _scene = scene ? scene : Engine::LastCreatedScene();
}

MorphTargetManager::~MorphTargetManager() = default;

void MorphTargetManager::addToScene(const MorphTargetManagerPtr& newMorphTargetManager)
{
  if (_scene) {
    _scene->morphTargetManagers.emplace_back(newMorphTargetManager);

    _uniqueId = _scene->getUniqueId();

    const auto engineCaps    = _scene->getEngine()->getCaps();
    _canUseTextureForTargets = engineCaps.canUseGLVertexID && engineCaps.textureFloat
                               && engineCaps.maxVertexTextureImageUnits > 0;
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
  return _supportsNormals && enableNormalMorphing;
}

bool MorphTargetManager::get_supportsTangents() const
{
  return _supportsTangents && enableTangentMorphing;
}

bool MorphTargetManager::get_supportsUVs() const
{
  return _supportsUVs && enableUVMorphing;
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

bool MorphTargetManager::get_useTextureToStoreTargets() const
{
  return _useTextureToStoreTargets;
}

void MorphTargetManager::set_useTextureToStoreTargets(bool value)
{
  _useTextureToStoreTargets = value;
}

bool MorphTargetManager::get_isUsingTextureForTargets() const
{
  return useTextureToStoreTargets() && _canUseTextureForTargets;
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
  _targetInfluenceChangedObservers.emplace_back(_targets.back()->onInfluenceChanged.add(
    [this](const bool* needUpdate, EventState&) -> void { _syncActiveTargets(*needUpdate); }));
  _targetDataLayoutChangedObservers.emplace_back(_targets.back()->_onDataLayoutChanged.add(
    [this](void*, EventState&) -> void { _syncActiveTargets(true); }));
  _syncActiveTargets(true);
}

void MorphTargetManager::removeTarget(MorphTarget* target)
{
  auto it
    = std::find_if(_targets.begin(), _targets.end(), [target](const MorphTargetPtr& morphTarget) {
        return target == morphTarget.get();
      });
  if (it != _targets.end()) {
    _targets.erase(it);

    size_t index = static_cast<size_t>(it - _targets.begin());
    target->onInfluenceChanged.remove(_targetInfluenceChangedObservers[index]);
    target->_onDataLayoutChanged.remove(_targetDataLayoutChangedObservers[index]);
    _syncActiveTargets(true);
  }
}

void MorphTargetManager::_bind(const EffectPtr& effect)
{
  effect->setFloat3("morphTargetTextureInfo", static_cast<float>(_textureVertexStride),
                    static_cast<float>(_textureWidth), static_cast<float>(_textureHeight));
  effect->setFloatArray("morphTargetTextureIndices", _morphTargetTextureIndices);
  effect->setTexture("morphTargets", _targetStoreTexture);
}

MorphTargetManagerPtr MorphTargetManager::clone() const
{
  return nullptr;
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
  _supportsUVs      = true;
  _vertexCount      = 0;

  if (_morphTargetTextureIndices.empty() || _morphTargetTextureIndices.size() != _targets.size()) {
    _morphTargetTextureIndices = Float32Array(_targets.size(), 0.f);
  }

  auto targetIndex = -1;
  for (const auto& target : _targets) {
    ++targetIndex;
    if (target->influence == 0.f && optimizeInfluencers) {
      continue;
    }

    _activeTargets.emplace_back(target);
    _morphTargetTextureIndices[influenceCount] = targetIndex;
    _tempInfluences.emplace_back(target->influence());
    ++influenceCount;

    _supportsNormals  = _supportsNormals && target->hasNormals();
    _supportsTangents = _supportsTangents && target->hasTangents();
    _supportsUVs      = _supportsUVs && target->hasUVs();

    const auto& positions = target->getPositions();
    if (!positions.empty()) {
      const auto iVertexCount = positions.size() / 3;
      if (_vertexCount == 0) {
        _vertexCount = iVertexCount;
      }
      else if (_vertexCount != iVertexCount) {
        BABYLON_LOG_ERROR("MorphTargetManager",
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

  if (isUsingTextureForTargets && _vertexCount) {
    _textureVertexStride = 1;

    if (_supportsNormals) {
      _textureVertexStride++;
    }

    if (_supportsTangents) {
      _textureVertexStride++;
    }

    if (_supportsUVs) {
      _textureVertexStride++;
    }

    _textureWidth  = _vertexCount * _textureVertexStride;
    _textureHeight = 1;

    const auto maxTextureSize = _scene->getEngine()->getCaps().maxTextureSize;
    if (_textureWidth > maxTextureSize) {
      _textureHeight = std::ceil(_textureWidth / maxTextureSize);
      _textureWidth  = maxTextureSize;
    }

    auto mustUpdateTexture = true;
    if (_targetStoreTexture) {
      auto textureSize = _targetStoreTexture->getSize();
      if (textureSize.width == _textureWidth && textureSize.height == _textureHeight
          && _targetStoreTexture->depth == static_cast<int>(_targets.size())) {
        mustUpdateTexture = false;
      }
    }

    if (mustUpdateTexture) {
      if (_targetStoreTexture) {
        _targetStoreTexture->dispose();
      }

      auto targetCount = _targets.size();
      Float32Array data(targetCount * _textureWidth * _textureHeight * 4, 0.f);

      auto offset = 0;
      for (size_t index = 0; index < targetCount; index++) {
        const auto& target = _targets[index];

        const auto positions = target->getPositions();
        const auto normals   = target->getNormals();
        const auto uvs       = target->getUVs();
        const auto tangents  = target->getTangents();

        if (positions.empty()) {
          if (index == 0) {
            BABYLON_LOG_ERROR("MorphTargetManager",
                              "Invalid morph target. Target must have positions.");
          }
          return;
        }

        offset = index * _textureWidth * _textureHeight * 4;
        for (size_t vertex = 0; vertex < _vertexCount; vertex++) {
          data[offset]     = positions[vertex * 3];
          data[offset + 1] = positions[vertex * 3 + 1];
          data[offset + 2] = positions[vertex * 3 + 2];

          offset += 4;

          if (!normals.empty()) {
            data[offset]     = normals[vertex * 3];
            data[offset + 1] = normals[vertex * 3 + 1];
            data[offset + 2] = normals[vertex * 3 + 2];
            offset += 4;
          }

          if (!uvs.empty()) {
            data[offset]     = uvs[vertex * 2];
            data[offset + 1] = uvs[vertex * 2 + 1];
            offset += 4;
          }

          if (!tangents.empty()) {
            data[offset]     = tangents[vertex * 3];
            data[offset + 1] = tangents[vertex * 3 + 1];
            data[offset + 2] = tangents[vertex * 3 + 2];
            offset += 4;
          }
        }
      }

      _targetStoreTexture = RawTexture2DArray::CreateRGBATexture(
        data, _textureWidth, _textureHeight, targetCount, _scene, false, false,
        Constants::TEXTURE_NEAREST_SAMPLINGMODE, Constants::TEXTURETYPE_FLOAT);
    }
  }

  // Flag meshes as dirty to resync with the active targets
  for (auto& abstractMesh : _scene->meshes) {
    auto mesh = std::static_pointer_cast<Mesh>(abstractMesh);
    if (mesh && (mesh->morphTargetManager().get() == this)) {
      mesh->_syncGeometryWithMorphTargetManager();
    }
  }
}

void MorphTargetManager::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  if (_targetStoreTexture) {
    _targetStoreTexture->dispose();
  }

  _targetStoreTexture = nullptr;
}

MorphTargetManagerPtr MorphTargetManager::Parse(const json& serializationObject, Scene* scene)
{
  auto result = MorphTargetManager::New(scene);

  result->_uniqueId = json_util::get_number<unsigned>(serializationObject, "id", 0);

  if (json_util::has_key(serializationObject, "targets")
      && (serializationObject["targets"].is_array())) {
    for (auto& targetData : json_util::get_array<json>(serializationObject, "targets")) {
      result->addTarget(MorphTarget::Parse(targetData));
    }
  }

  return result;
}

} // end of namespace BABYLON
