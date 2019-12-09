#ifndef BABYLON_POSTPROCESSES_GLOW_BLUR_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_GLOW_BLUR_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class GlowBlurPostProcess;
using GlowBlurPostProcessPtr = std::shared_ptr<GlowBlurPostProcess>;

/**
 * @brief Special Glow Blur post process only blurring the alpha channel
 * It enforces keeping the most luminous color in the color channel.
 */
class BABYLON_SHARED_EXPORT GlowBlurPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static GlowBlurPostProcessPtr New(Ts&&... args)
  {
    auto postProcess
      = std::shared_ptr<GlowBlurPostProcess>(new GlowBlurPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~GlowBlurPostProcess() override; // = default

protected:
  GlowBlurPostProcess(const std::string& name, const Vector2& direction, float kernel,
                      const std::variant<float, PostProcessOptions>& options,
                      const CameraPtr& camera,
                      const std::optional<unsigned int>& samplingMode = std::nullopt,
                      Engine* engine = nullptr, bool reusable = false);

public:
  Vector2 direction;
  float kernel;

}; // end of struct GlowBlurPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_GLOW_BLUR_POST_PROCESS_H
