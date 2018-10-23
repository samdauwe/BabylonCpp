#ifndef BABYLON_POSTPROCESS_SHARPEN_POST_PROCESS_H
#define BABYLON_POSTPROCESS_SHARPEN_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

class SharpenPostProcess;
using SharpenPostProcessPtr = std::shared_ptr<SharpenPostProcess>;

/**
 * @brief The SharpenPostProcess applies a sharpen kernel to every pixel.
 * See http://en.wikipedia.org/wiki/Kernel_(image_processing)
 */
class BABYLON_SHARED_EXPORT SharpenPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static SharpenPostProcessPtr New(Ts&&... args)
  {
    return std::shared_ptr<SharpenPostProcess>(
      new SharpenPostProcess(std::forward<Ts>(args)...));
  }
  virtual ~SharpenPostProcess() override;

protected:
  /**
   * @brief Creates a new instance ConvolutionPostProcess.
   * @param name The name of the effect.
   * @param options The required width/height ratio to downsize to before
   * computing the render pass.
   * @param camera The camera to apply the render pass to.
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
  SharpenPostProcess(const std::string& name,
                     const std::variant<float, PostProcessOptions>& options,
                     const CameraPtr& camera, unsigned int samplingMode,
                     Engine* engine, bool reusable = false,
                     unsigned int textureType
                     = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
                     bool blockCompilation = false);

public:
  /**
   * How much of the original color should be applied. Setting this to 0 will
   * display edge detection. (default: 1)
   */
  float colorAmount;
  /**
   * How much sharpness should be applied (default: 0.3)
   */
  float edgeAmount;

}; // end of class SharpenPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_SHARPEN_POST_PROCESS_H
