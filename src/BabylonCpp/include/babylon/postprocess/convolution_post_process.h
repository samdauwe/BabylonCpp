#ifndef BABYLON_POSTPROCESS_CONVOLUTION_POST_PROCESS_H
#define BABYLON_POSTPROCESS_CONVOLUTION_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief The ConvolutionPostProcess applies a 3x3 kernel to every pixel of the
 * input texture to perform effects such as edge detection or sharpening
 * See http://en.wikipedia.org/wiki/Kernel_(image_processing)
 */
class BABYLON_SHARED_EXPORT ConvolutionPostProcess : public PostProcess {

public:
  // Statics
  /**
   * Edge detection 0 see
   * https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array EdgeDetect0Kernel;
  /**
   * Edge detection 1 see
   * https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array EdgeDetect1Kernel;
  /**
   * Edge detection 2 see
   * https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array EdgeDetect2Kernel;
  /**
   * Kernel to sharpen an image see
   * https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array SharpenKernel;
  /**
   * Kernel to emboss an image see
   * https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array EmbossKernel;
  /**
   * Kernel to blur an image see
   * https://en.wikipedia.org/wiki/Kernel_(image_processing)
   */
  static const Float32Array GaussianKernel;

public:
  /**
   * @brief Creates a new instance ConvolutionPostProcess.
   * @param name The name of the effect.
   * @param kernel Array of 9 values corrisponding to the 3x3 kernel to be
   * applied
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
   */
  ConvolutionPostProcess(const string_t& name, const Float32Array& kernel,
                         float ratio, Camera* camera, unsigned int samplingMode,
                         Engine* engine, bool reusable = false,
                         unsigned int textureType
                         = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  ~ConvolutionPostProcess();

public:
  /**
   * Array of 9 values corrisponding to the 3x3 kernel to be applied
   */
  Float32Array kernel;

}; // end of class ConvolutionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_CONVOLUTION_POST_PROCESS_H
