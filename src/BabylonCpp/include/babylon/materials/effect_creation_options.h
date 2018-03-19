#ifndef BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H
#define BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H

#include <babylon/babylon_global.h>
#include <babylon/materials/effect_fallbacks.h>

namespace BABYLON {

/**
 * @brief Options to be used when creating an effect.
 */
struct BABYLON_SHARED_EXPORT EffectCreationOptions {
  /**
   * Atrributes that will be used in the shader.
   */
  vector_t<string_t> attributes{};
  /**
   * Uniform varible names that will be set in the shader.
   */
  vector_t<string_t> uniformsNames{};
  /**
   * Uniform buffer varible names that will be set in the shader.
   */
  vector_t<string_t> uniformBuffersNames{};
  /**
   * Sampler texture variable names that will be set in the shader.
   */
  vector_t<string_t> samplers{};
  /**
   * See
   * https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/transformFeedbackVaryings
   */
  vector_t<string_t> transformFeedbackVaryings{};
  /**
   * MaterialDefines pointer.
   */
  MaterialDefines* materialDefines{nullptr};
  /**
   * Define statements that will be set in the shader.
   */
  string_t defines{""};
  /**
   * Possible fallbacks for this effect to improve performance when needed.
   */
  unique_ptr_t<EffectFallbacks> fallbacks{nullptr};
  /**
   * Callback that will be called when the shader is compiled.
   */
  ::std::function<void(Effect* effect)> onCompiled{nullptr};
  /**
   * Callback that will be called if an error occurs during shader compilation.
   */
  ::std::function<void(Effect* effect, const string_t& errors)> onError{
    nullptr};
  /**
   * Parameters to be used with Babylons include syntax to iterate over an array
   * (eg. {lights: 10})
   */
  unordered_map_t<string_t, unsigned int> indexParameters{};
  /**
   * Number.
   */
  unsigned int number{0};
  /**
   * Max number of lights that can be used in the shader.
   */
  unsigned int maxSimultaneousLights{4};
}; // end of class EffectCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_CREATE_OPTIONS_H
