#ifndef BABYLON_MATERIALS_IEFFECT_FALLBACKS_H
#define BABYLON_MATERIALS_IEFFECT_FALLBACKS_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Effect;

/**
 * @brief Interface used to define common properties for effect fallbacks.
 */
struct BABYLON_SHARED_EXPORT IEffectFallbacks {
  virtual ~IEffectFallbacks() = default;

  /**
   * @brief Removes the defines that should be removed when falling back.
   * @param currentDefines defines the current define statements for the shader.
   * @param effect defines the current effect we try to compile
   * @returns The resulting defines with defines of the current rank removed.
   */
  virtual std::string reduce(std::string currentDefines, Effect* effect) = 0;

  /**
   * @brief Removes the fallback from the bound mesh.
   */
  virtual void unBindMesh() = 0;

  /**
   * @brief Checks to see if more fallbacks are still available.
   */
  virtual bool hasMoreFallbacks() const = 0;

}; // end of struct IEffectFallbacks

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IEFFECT_FALLBACKS_H
