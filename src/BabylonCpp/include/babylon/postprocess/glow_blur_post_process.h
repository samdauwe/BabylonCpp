#ifndef BABYLON_POSTPROCESS_GLOW_BLUR_POST_PROCESS_H
#define BABYLON_POSTPROCESS_GLOW_BLUR_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief Special Glow Blur post process only blurring the alpha channel
 * It enforces keeping the most luminous color in the color channel.
 */
class BABYLON_SHARED_EXPORT GlowBlurPostProcess : public PostProcess {

public:
  GlowBlurPostProcess(const string_t& name, const Vector2& direction,
                      float kernel,
                      const Variant<float, PostProcessOptions>& options,
                      Camera* camera,
                      unsigned int samplingMode
                      = TextureConstants::BILINEAR_SAMPLINGMODE,
                      Engine* engine = nullptr, bool reusable = false);
  ~GlowBlurPostProcess();

public:
  Vector2 direction;
  float kernel;

}; // end of struct GlowBlurPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_GLOW_BLUR_POST_PROCESS_H
