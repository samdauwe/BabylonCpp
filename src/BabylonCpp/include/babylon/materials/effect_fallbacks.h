#ifndef BABYLON_MATERIALS_EFFECT_FALLBACKS_H
#define BABYLON_MATERIALS_EFFECT_FALLBACKS_H

#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class AbstractMesh;
class Effect;

/**
 * @brief EffectFallbacks can be used to add fallbacks (properties to disable)
 * to certain properties when desired to improve performance. (Eg. Start at high
 * quality with reflection and fog, if fps is low, remove reflection, if still
 * low remove fog)
 */
class BABYLON_SHARED_EXPORT EffectFallbacks {

public:
  EffectFallbacks();
  EffectFallbacks(const EffectFallbacks& other);
  EffectFallbacks(EffectFallbacks&& other);
  EffectFallbacks& operator=(const EffectFallbacks& other);
  EffectFallbacks& operator=(EffectFallbacks&& other);
  ~EffectFallbacks(); // = default

  bool operator==(const EffectFallbacks& other) const;
  bool operator!=(const EffectFallbacks& other) const;

  /**
   * @brief Removes the fallback from the bound mesh.
   */
  void unBindMesh();

  /**
   * @brief Adds a fallback on the specified property.
   * @param rank The rank of the fallback (Lower ranks will be fallbacked to
   * first)
   * @param define The name of the define in the shader
   */
  void addFallback(unsigned int rank, const std::string& define);

  /**
   * @brief Sets the mesh to use CPU skinning when needing to fallback.
   * @param rank The rank of the fallback (Lower ranks will be fallbacked to
   * first)
   * @param mesh The mesh to use the fallbacks.
   */
  void addCPUSkinningFallback(unsigned int rank, AbstractMesh* mesh);

  /**
   * @brief Checks to see if more fallbacks are still availible.
   */
  [[nodiscard]] bool isMoreFallbacks() const;

  /**
   * @brief Removes the defines that shoould be removed when falling back.
   * @param currentDefines defines the current define statements for the shader.
   * @param effect defines the current effect we try to compile
   * @returns The resulting defines with defines of the current rank removed.
   */
  std::string reduce(std::string currentDefines, Effect* effect);

private:
  std::unordered_map<unsigned int, std::vector<std::string>> _defines;
  unsigned int _currentRank;
  unsigned int _maxRank;

  AbstractMesh* _mesh;
  unsigned int _meshRank;

}; // end of class EffectFallbacks

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_FALLBACKS_H
