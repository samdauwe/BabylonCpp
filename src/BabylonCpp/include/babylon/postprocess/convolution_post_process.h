#ifndef BABYLON_POSTPROCESS_CONVOLUTION_POST_PROCESS_H
#define BABYLON_POSTPROCESS_CONVOLUTION_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT ConvolutionPostProcess : public PostProcess {

public:
  // Statics
  // Based on http://en.wikipedia.org/wiki/Kernel_(image_processing)
  static const Float32Array EdgeDetect0Kernel;
  static const Float32Array EdgeDetect1Kernel;
  static const Float32Array EdgeDetect2Kernel;
  static const Float32Array SharpenKernel;
  static const Float32Array EmbossKernel;
  static const Float32Array GaussianKernel;

public:
  ConvolutionPostProcess(const std::string& name, const Float32Array& kernel,
                         float ratio, Camera* camera, unsigned int samplingMode,
                         Engine* engine, bool reusable = false);
  ~ConvolutionPostProcess();

public:
  Float32Array kernel;

}; // end of class ConvolutionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_CONVOLUTION_POST_PROCESS_H
