#ifndef BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_BLUR_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_BLUR_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/engine/constants.h>
#include <babylon/postprocesses/blur_post_process.h>

namespace BABYLON {

class DepthOfFieldBlurPostProcess;
using DepthOfFieldBlurPostProcessPtr
  = std::shared_ptr<DepthOfFieldBlurPostProcess>;

/**
 * @brief The DepthOfFieldBlurPostProcess applied a blur in a give direction.
 * This blur differs from the standard BlurPostProcess as it attempts to avoid
 * blurring pixels based on samples that have a large difference in distance
 * than the center pixel. See section 2.6.2
 * http://fileadmin.cs.lth.se/cs/education/edan35/lectures/12dof.pdf
 */
class BABYLON_SHARED_EXPORT DepthOfFieldBlurPostProcess
    : public BlurPostProcess {

public:
  template <typename... Ts>
  static DepthOfFieldBlurPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<DepthOfFieldBlurPostProcess>(
      new DepthOfFieldBlurPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~DepthOfFieldBlurPostProcess();

protected:
  /**
   * @brief Creates a new instance CircleOfConfusionPostProcess.
   * @param name The name of the effect.
   * @param scene The scene the effect belongs to.
   * @param direction The direction the blur should be applied.
   * @param kernel The size of the kernel used to blur.
   * @param options The required width/height ratio to downsize to before
   * computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param circleOfConfusion The circle of confusion + depth map to be used to
   * avoid blurring accross edges
   * @param imageToBlur The image to apply the blur to (default: Current
   * rendered frame)
   * @param samplingMode The sampling mode to be used when computing the pass.
   * (default: 0)
   * @param engine The engine which the post process will be applied. (default:
   * current engine)
   * @param reusable If the post process can be reused on the same frame.
   * (default: false)
   * @param textureType Type of textures used when performing the post process.
   * (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in
   * the constructor. The updateEffect method can be used to compile the shader
   * at a later time. (default: false)
   */
  DepthOfFieldBlurPostProcess(
    const std::string& name, Scene* scene, const Vector2& direction,
    float kernel, const std::variant<float, PostProcessOptions>& options,
    const CameraPtr& camera, PostProcess* circleOfConfusion,
    PostProcess* imageToBlur  = nullptr,
    unsigned int samplingMode = Constants::TEXTURE_BILINEAR_SAMPLINGMODE,
    Engine* engine = nullptr, bool reusable = false,
    unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
    bool blockCompilation    = false);

}; // end of class DepthOfFieldBlurPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_BLUR_POST_PROCESS_H
