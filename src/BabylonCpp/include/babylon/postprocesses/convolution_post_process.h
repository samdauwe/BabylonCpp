#ifndef BABYLON_POSTPROCESSES_CONVOLUTION_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_CONVOLUTION_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

FWD_CLASS_SPTR(ConvolutionPostProcess)

/**
 * @brief The ConvolutionPostProcess applies a 3x3 kernel to every pixel of the
 * input texture to perform effects such as edge detection or sharpening
 * See http://en.wikipedia.org/wiki/Kernel_(image_processing)
 */
class BABYLON_SHARED_EXPORT ConvolutionPostProcess : public PostProcess {

public:
  // Statics
  /**
   * Edge detection 0 see https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array EdgeDetect0Kernel();
  /**
   * Edge detection 1 see https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array EdgeDetect1Kernel();
  /**
   * Edge detection 2 see https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array EdgeDetect2Kernel();
  /**
   * Kernel to sharpen an image see https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array SharpenKernel();
  /**
   * Kernel to emboss an image see https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array EmbossKernel();
  /**
   * Kernel to blur an image see https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array GaussianKernel();

public:
  template <typename... Ts>
  static ConvolutionPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<ConvolutionPostProcess>(
      new ConvolutionPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~ConvolutionPostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "ConvolutionPostProcess" string
   */
  std::string getClassName() const override;

  /**
   * @brief Hidden
   */
  static ConvolutionPostProcessPtr _Parse(const json& parsedPostProcess,
                                          const CameraPtr& targetCamera, Scene* scene,
                                          const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new instance ConvolutionPostProcess.
   * @param name The name of the effect.
   * @param kernel Array of 9 values corresponding to the 3x3 kernel to be applied
   * @param options The required width/height ratio to downsize to before computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   * @param textureType Type of textures used when performing the post process. (default: 0)
   */
  ConvolutionPostProcess(const std::string& name, const Float32Array& kernel, float ratio,
                         const CameraPtr& camera,
                         const std::optional<unsigned int>& samplingMode = std::nullopt,
                         Engine* engine = nullptr, bool reusable = false,
                         unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT);

public:
  /**
   * Array of 9 values corresponding to the 3x3 kernel to be applied
   */
  Float32Array kernel;

}; // end of class ConvolutionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_CONVOLUTION_POST_PROCESS_H
