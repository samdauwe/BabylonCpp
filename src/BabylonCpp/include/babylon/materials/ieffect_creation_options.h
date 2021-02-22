#ifndef BABYLON_MATERIALS_IEFFECT_CREATE_OPTIONS_H
#define BABYLON_MATERIALS_IEFFECT_CREATE_OPTIONS_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/effect_fallbacks.h>

namespace BABYLON {

struct MaterialDefines;
FWD_CLASS_SPTR(Effect)

/**
 * @brief Options to be used when creating an effect.
 */
struct BABYLON_SHARED_EXPORT IEffectCreationOptions {
  /**
   * Attributes that will be used in the shader.
   */
  std::vector<std::string> attributes{};
  /**
   * Uniform variable names that will be set in the shader.
   */
  std::vector<std::string> uniformsNames{};
  /**
   * Uniform buffer variable names that will be set in the shader.
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
  std::unique_ptr<IEffectFallbacks> fallbacks{nullptr};
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
  /**
   * If provided, will be called two times with the vertex and fragment code so that this code can
   * be updated before it is compiled by the GPU
   */
  std::function<std::string(const std::string& shaderType, const std::string& code)>
    processFinalCode = nullptr;
  /**
   * Is this effect rendering to several color attachments ?
   */
  std::optional<bool> multiTarget{std::nullopt};
}; // end of class IEffectCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IEFFECT_CREATE_OPTIONS_H
