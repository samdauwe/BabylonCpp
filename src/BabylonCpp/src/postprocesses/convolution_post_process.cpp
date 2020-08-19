#include <babylon/babylon_api.h>
#include <babylon/materials/effect.h>
#include <babylon/postprocesses/convolution_post_process.h>

namespace BABYLON {

const Float32Array gEdgeDetect0Kernel = {1, 0, -1, 0, 0, 0, -1, 0, 1};
const Float32Array gEdgeDetect1Kernel = {0, 1, 0, 1, -4, 1, 0, 1, 0};
const Float32Array gEdgeDetect2Kernel = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
const Float32Array gSharpenKernel     = {0, -1, 0, -1, 5, -1, 0, -1, 0};
const Float32Array gEmbossKernel      = {-2, -1, 0, -1, 1, 1, 0, 1, 2};
const Float32Array gGaussianKernel    = {0, 1, 0, 1, 1, 1, 0, 1, 0};

const Float32Array ConvolutionPostProcess::EdgeDetect0Kernel()
{
  return gEdgeDetect0Kernel;
}
const Float32Array ConvolutionPostProcess::EdgeDetect1Kernel()
{
  return gEdgeDetect1Kernel;
}
const Float32Array ConvolutionPostProcess::EdgeDetect2Kernel()
{
  return gEdgeDetect2Kernel;
}
const Float32Array ConvolutionPostProcess::SharpenKernel()
{
  return gSharpenKernel;
}
const Float32Array ConvolutionPostProcess::EmbossKernel()
{
  return gEmbossKernel;
}
const Float32Array ConvolutionPostProcess::GaussianKernel()
{
  return gGaussianKernel;
}

ConvolutionPostProcess::ConvolutionPostProcess(const std::string& iName,
                                               const Float32Array& iKernel, float ratio,
                                               const CameraPtr& camera,
                                               const std::optional<unsigned int>& samplingMode,
                                               Engine* engine, bool reusable,
                                               unsigned int textureType)
    : PostProcess{iName,        "convolution", {"kernel", "screenSize"},
                  {},           ratio,         camera,
                  samplingMode, engine,        reusable,
                  "",           textureType}
{
  kernel = iKernel;

  onApply = [this](Effect* effect, EventState&) {
    effect->setFloat2("screenSize", static_cast<float>(width), static_cast<float>(height));
    effect->setArray("kernel", kernel);
  };
}

ConvolutionPostProcess::~ConvolutionPostProcess() = default;

std::string ConvolutionPostProcess::getClassName() const
{
  return "ConvolutionPostProcess";
}

ConvolutionPostProcessPtr ConvolutionPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                                         const CameraPtr& /*targetCamera*/,
                                                         Scene* /*scene*/,
                                                         const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
