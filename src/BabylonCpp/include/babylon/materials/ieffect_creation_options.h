#ifndef BABYLON_MATERIALS_IEFFECT_CREATE_OPTIONS_H
#define BABYLON_MATERIALS_IEFFECT_CREATE_OPTIONS_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/materials/effect_fallbacks.h>

namespace BABYLON {

class Effect;
struct MaterialDefines;
using EffectPtr = std::shared_ptr<Effect>;

/**
 * @brief Options to be used when creating an effect.
 */
struct BABYLON_SHARED_EXPORT IEffectCreationOptions {
  /**
   * Atrributes that will be used in the shader.
   */
  std::vector<std::string> attributes{};
  /**
   * Uniform varible names that will be set in the shader.
   */
  std::vector<std::string> uniformsNames{};
  /**
   * Uniform buffer varible names that will be set in the shader.
   */
  std::vector<std::string> uniformBuffersNames{};
  /**
   * Sampler texture variable names that will be set in the shader.
   */
  std::vector<std::string> samplers{};
  /**
   * MaterialDefines pointer.
   */
  MaterialDefines* materialDefines{nullptr};
  /**
   * Define statements that will be set in the shader.
   */
  std::string defines{""};
  /**
   * Possible fallbacks for this effect to improve performance when needed.
   */
  std::unique_ptr<EffectFallbacks> fallbacks{nullptr};
  /**
   * Callback that will be called when the shader is compiled.
   */
  std::function<void(Effect* effect)> onCompiled{nullptr};
  /**
   * Callback that will be called if an error occurs during shader compilation.
   */
  std::function<void(Effect* effect, const std::string& errors)> onError{nullptr};
  /**
   * Parameters to be used with Babylons include syntax to iterate over an array
   * (eg. {lights: 10})
   */
  std::unordered_map<std::string, unsigned int> indexParameters{};
  /**
   * Number.
   */
  unsigned int number{0};
  /**
   * Max number of lights that can be used in the shader.
   */
  unsigned int maxSimultaneousLights{4};
  /**
   * See
   * https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/transformFeedbackVaryings
   */
  std::vector<std::string> transformFeedbackVaryings{};
}; // end of class IEffectCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IEFFECT_CREATE_OPTIONS_H
