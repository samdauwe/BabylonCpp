#ifndef BABYLON_LAYER_GLOW_BLUR_POST_PROCESS_H
#define BABYLON_LAYER_GLOW_BLUR_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief Special Glow Blur post process only blurring the alpha channel
 * It enforces keeping the most luminous color in the color channel.
 */
struct BABYLON_SHARED_EXPORT GlowBlurPostProcess : public PostProcess {

  GlowBlurPostProcess(const std::string& name, const Vector2& direction,
                      float blurWidth, float options, Camera* camera,
                      unsigned int samplingMode
                      = TextureConstants::BILINEAR_SAMPLINGMODE,
                      Engine* engine = nullptr, bool reusable = false);
  GlowBlurPostProcess(const std::string& name, const Vector2& direction,
                      float blurWidth, const PostProcessOptions& options,
                      Camera* camera, unsigned int samplingMode
                                      = TextureConstants::BILINEAR_SAMPLINGMODE,
                      Engine* engine = nullptr, bool reusable = false);
  ~GlowBlurPostProcess();

  Vector2 direction;
  float blurWidth;

}; // end of struct GlowBlurPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_GLOW_BLUR_POST_PROCESS_H
