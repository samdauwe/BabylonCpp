#ifndef BABYLON_MATERIALS_EFFECT_FALLBACKS_H
#define BABYLON_MATERIALS_EFFECT_FALLBACKS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT EffectFallbacks {

public:
  EffectFallbacks();
  ~EffectFallbacks();

  void addFallback(unsigned int rank, const std::string& define);
  void addCPUSkinningFallback(unsigned int rank, AbstractMesh* mesh);
  bool isMoreFallbacks() const;
  std::string reduce(const std::string& currentDefines);

private:
  std::unordered_map<unsigned int, std::vector<std::string>> _defines;
  unsigned int _currentRank;
  unsigned int _maxRank;

  AbstractMesh* _mesh;
  unsigned int _meshRank;

}; // end of class EffectFallbacks

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_FALLBACKS_H
