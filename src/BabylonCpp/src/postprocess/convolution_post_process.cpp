#include <babylon/postprocess/convolution_post_process.h>

#include <babylon/core/variant.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

const Float32Array ConvolutionPostProcess::EdgeDetect0Kernel
  = {1, 0, -1, 0, 0, 0, -1, 0, 1};
const Float32Array ConvolutionPostProcess::EdgeDetect1Kernel
  = {0, 1, 0, 1, -4, 1, 0, 1, 0};
const Float32Array ConvolutionPostProcess::EdgeDetect2Kernel
  = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
const Float32Array ConvolutionPostProcess::SharpenKernel
  = {0, -1, 0, -1, 5, -1, 0, -1, 0};
const Float32Array ConvolutionPostProcess::EmbossKernel
  = {-2, -1, 0, -1, 1, 1, 0, 1, 2};
const Float32Array ConvolutionPostProcess::GaussianKernel
  = {0, 1, 0, 1, 1, 1, 0, 1, 0};

ConvolutionPostProcess::ConvolutionPostProcess(
  const string_t& iName, const Float32Array& iKernel, float ratio,
  const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
  bool reusable, unsigned int textureType)
    : PostProcess{iName,
                  "convolution",
                  {"kernel", "screenSize"},
                  {},
                  ToVariant<float, PostProcessOptions>(ratio),
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "",
                  textureType}
    , kernel{iKernel}
{
  setOnApply([&](Effect* effect, EventState&) {
    effect->setFloat2("screenSize", static_cast<float>(width),
                      static_cast<float>(height));
    effect->setArray("kernel", kernel);
  });
}

ConvolutionPostProcess::~ConvolutionPostProcess()
{
}

} // end of namespace BABYLON
