#include <babylon/materials/effect_fallbacks.h>

#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

EffectFallbacks::EffectFallbacks() : _currentRank{32}, _maxRank{0}
{
}

EffectFallbacks::~EffectFallbacks()
{
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
  }

  _defines[rank].emplace_back(define);
}

void EffectFallbacks::addCPUSkinningFallback(unsigned int rank,
                                             AbstractMesh* mesh)
{
  _meshRank = rank;
  _mesh     = mesh;

  if (rank > _maxRank) {
    _maxRank = rank;
  }
}

bool EffectFallbacks::isMoreFallbacks() const
{
  return _currentRank <= _maxRank;
}

std::string EffectFallbacks::reduce(const std::string& currentDefines)
{
  std::string currentDefinesCpy(currentDefines);
  const std::vector<std::string>& currentFallbacks = _defines[_currentRank];

  for (unsigned int index = 0; index < currentFallbacks.size(); ++index) {
    String::replaceInPlace(
      currentDefinesCpy, std::string("#define ") + currentFallbacks[index], "");
  }

  if (_mesh && _currentRank == _meshRank) {
    _mesh->computeBonesUsingShaders = false;
    std::string toReplace = std::string("#define NUM_BONE_INFLUENCERS ")
                            + std::to_string(_mesh->numBoneInfluencers);
    String::replaceInPlace(currentDefinesCpy, toReplace,
                           "#define NUM_BONE_INFLUENCERS 0");
    BABYLON_LOGF_DEBUG("EffectFallbacks", "Falling back to CPU skinning for %s",
                       _mesh->name.c_str());
  }

  ++_currentRank;

  return currentDefinesCpy;
}

} // end of namespace BABYLON
