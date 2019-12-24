#include <babylon/materials/effect_fallbacks.h>

#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

EffectFallbacks::EffectFallbacks() : _currentRank{32}, _maxRank{0}, _mesh{nullptr}
{
}

EffectFallbacks::EffectFallbacks(const EffectFallbacks& other) = default;

EffectFallbacks::EffectFallbacks(EffectFallbacks&& other) = default;

EffectFallbacks& EffectFallbacks::operator=(const EffectFallbacks& other) = default;

EffectFallbacks& EffectFallbacks::operator=(EffectFallbacks&& other) = default;

EffectFallbacks::~EffectFallbacks() = default;

bool EffectFallbacks::operator==(const EffectFallbacks& other) const
{
  return (_defines == other._defines) && (_currentRank == other._currentRank)
         && (_maxRank == other._maxRank) && (_mesh == other._mesh)
         && (_meshRank == other._meshRank);
}

bool EffectFallbacks::operator!=(const EffectFallbacks& other) const
{
  return !(operator==(other));
}

void EffectFallbacks::unBindMesh()
{
  _mesh = nullptr;
}

void EffectFallbacks::addFallback(unsigned int rank, const std::string& define)
{
  if (_defines.find(rank) == _defines.end()) {
    if (rank < _currentRank) {
      _currentRank = rank;
    }

    if (rank > _maxRank) {
      _maxRank = rank;
    }

    _defines[rank].clear();
  }

  _defines[rank].emplace_back(define);
}

void EffectFallbacks::addCPUSkinningFallback(unsigned int rank, AbstractMesh* mesh)
{
  _mesh = mesh;

  if (rank < _currentRank) {
    _currentRank = rank;
  }
  if (rank > _maxRank) {
    _maxRank = rank;
  }
}

bool EffectFallbacks::hasMoreFallbacks() const
{
  return _currentRank <= _maxRank;
}

std::string EffectFallbacks::reduce(std::string currentDefines, Effect* effect)
{
  // First we try to switch to CPU skinning
  if (_mesh && _mesh->computeBonesUsingShaders() && _mesh->numBoneInfluencers() > 0) {
    _mesh->computeBonesUsingShaders = false;
    const std::string toReplace
      = std::string("#define NUM_BONE_INFLUENCERS ") + std::to_string(_mesh->numBoneInfluencers());
    String::replaceInPlace(currentDefines, toReplace, "#define NUM_BONE_INFLUENCERS 0");
    effect->_bonesComputationForcedToCPU = true;

    auto scene = _mesh->getScene();
    for (const auto& otherMesh : scene->meshes) {
      if (!otherMesh->material()) {
        if (!_mesh->material() && otherMesh->computeBonesUsingShaders()
            && otherMesh->numBoneInfluencers() > 0) {
          otherMesh->computeBonesUsingShaders = false;
        }
        continue;
      }

      if (!otherMesh->computeBonesUsingShaders() || otherMesh->numBoneInfluencers() == 0) {
        continue;
      }

      if (otherMesh->material()->getEffect().get() == effect) {
        otherMesh->computeBonesUsingShaders = false;
      }
      else if (!otherMesh->subMeshes.empty()) {
        for (const auto& subMesh : otherMesh->subMeshes) {
          auto subMeshEffect = subMesh->effect();

          if (subMeshEffect.get() == effect) {
            otherMesh->computeBonesUsingShaders = false;
            break;
          }
        }
      }
    }
  }
  else {
    if (_defines.find(_currentRank) != _defines.end() && !_defines[_currentRank].empty()) {
      for (const auto& currentFallback : _defines[_currentRank]) {
        String::replaceInPlace(currentDefines, "#define " + currentFallback, "");
      }
    }
    ++_currentRank;
  }

  return currentDefines;
}

} // end of namespace BABYLON
