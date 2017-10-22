#ifndef BABYLON_MATERIALS_EFFECT_FALLBACKS_H
#define BABYLON_MATERIALS_EFFECT_FALLBACKS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT EffectFallbacks {

public:
  EffectFallbacks();
  EffectFallbacks(const EffectFallbacks& other);
  EffectFallbacks(EffectFallbacks&& other);
  EffectFallbacks& operator=(const EffectFallbacks& other);
  EffectFallbacks& operator=(EffectFallbacks&& other);
  ~EffectFallbacks();

  bool operator==(const EffectFallbacks& other) const;
  bool operator!=(const EffectFallbacks& other) const;

  void unBindMesh();
  void addFallback(unsigned int rank, const string_t& define);
  void addCPUSkinningFallback(unsigned int rank, AbstractMesh* mesh);
  bool isMoreFallbacks() const;
  string_t reduce(const string_t& currentDefines);

private:
  unordered_map_t<unsigned int, vector_t<string_t>> _defines;
  unsigned int _currentRank;
  unsigned int _maxRank;

  AbstractMesh* _mesh;
  unsigned int _meshRank;

}; // end of class EffectFallbacks

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_FALLBACKS_H
