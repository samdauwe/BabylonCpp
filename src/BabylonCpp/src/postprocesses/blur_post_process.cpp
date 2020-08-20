#include <babylon/postprocesses/blur_post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

BlurPostProcess::BlurPostProcess(const std::string& iName, const Vector2& iDrection, float iKernel,
                                 const std::variant<float, PostProcessOptions>& options,
                                 const CameraPtr& camera,
                                 const std::optional<unsigned int>& samplingMode, Engine* engine,
                                 bool reusable, unsigned int textureType,
                                 const std::string& defines, bool iBlockCompilation)
    : PostProcess{iName,
                  "kernelBlur",
                  {"delta", "direction", "cameraMinMaxZ"},
                  {"circleOfConfusionSampler"},
                  options,
                  camera,
                  samplingMode.value_or(TextureConstants::BILINEAR_SAMPLINGMODE),
                  engine,
                  reusable,
                  "",
                  textureType,
                  "kernelBlur",
                  {{"varyingCount", 0}, {"depCount", 0}},
                  true}
    , kernel{this, &BlurPostProcess::get_kernel, &BlurPostProcess::set_kernel}
    , packedFloat{this, &BlurPostProcess::get_packedFloat, &BlurPostProcess::set_packedFloat}
    , _kernel{0.f}
    , _idealKernel{0.f}
    , _packedFloat{false}
    , blockCompilation{iBlockCompilation}
{
  _staticDefines = defines;
  direction      = iDrection;

  onApplyObservable.add([this](Effect* effect, EventState&) {
    if (_outputTexture) {
      effect->setFloat2("delta", (1.f / static_cast<float>(_outputTexture->width)) * direction.x,
                        (1.f / static_cast<float>(_outputTexture->height)) * direction.y);
    }
    else {
      effect->setFloat2("delta", (1.f / static_cast<float>(width)) * direction.x,
                        (1.f / static_cast<float>(height)) * direction.y);
    }
  });

  kernel = iKernel;
}

BlurPostProcess::~BlurPostProcess() = default;

void BlurPostProcess::set_kernel(float v)
{
  if (stl_util::almost_equal(_idealKernel, v)) {
    return;
  }

  v            = std::max(v, 1.f);
  _idealKernel = v;
  _kernel      = _nearestBestKernel(v);
  if (!blockCompilation) {
    _updateParameters();
  }
}

float BlurPostProcess::get_kernel() const
{
  return _idealKernel;
}

void BlurPostProcess::set_packedFloat(bool v)
{
  if (_packedFloat == v) {
    return;
  }
  _packedFloat = v;
  if (!blockCompilation) {
    _updateParameters();
  }
}

bool BlurPostProcess::get_packedFloat() const
{
  return _packedFloat;
}

std::string BlurPostProcess::getClassName() const
{
  return "BlurPostProcess";
}

void BlurPostProcess::updateEffect(
  const std::string& /*defines*/, const std::vector<std::string>& /*uniforms*/,
  const std::vector<std::string>& /*samplers*/,
  const std::unordered_map<std::string, unsigned int>& /*indexParameters*/,
  const std::function<void(Effect* effect)>& onCompiled,
  const std::function<void(Effect* effect, const std::string& errors)>& onError,
  const std::string& /*vertexUrl*/, const std::string& /*fragmentUrl*/)
{
  _updateParameters(onCompiled, onError);
}

void BlurPostProcess::_updateParameters(
  const std::function<void(Effect* effect)>& onCompiled,
  const std::function<void(Effect* effect, const std::string& errors)>& onError)
{
  // Generate sampling offsets and weights
  const auto N           = _kernel;
  const auto centerIndex = static_cast<unsigned>((N - 1) / 2.f);

  // Generate Gaussian sampling weights over kernel
  const auto _N = static_cast<unsigned>(N);
  Int32Array offsets(_N);
  Float32Array weights(_N);
  float totalWeight = 0.f;
  for (unsigned int i = 0; i < _N; i++) {
    const auto u = i / static_cast<float>(N - 1);
    const auto w = _gaussianWeight(u * 2.f - 1.f);
    offsets[i]   = static_cast<int>(i - centerIndex);
    weights[i]   = w;
    totalWeight += w;
  }

  // Normalize weights
  for (float& weight : weights) {
    weight /= totalWeight;
  }

  // Optimize: combine samples to take advantage of hardware linear sampling
  // Walk from left to center, combining pairs (symmetrically)
  Float32Array linearSamplingWeights;
  Int32Array linearSamplingOffsets;

  std::vector<std::pair<std::int32_t, float>> linearSamplingMap;

  for (unsigned int i = 0; i <= centerIndex; i += 2) {
    auto j = std::min(i + 1, static_cast<unsigned>(std::floor(centerIndex)));

    auto singleCenterSample = (i == j);

    if (singleCenterSample) {
      linearSamplingMap.emplace_back(std::make_pair(offsets[i], weights[i]));
    }
    else {
      auto sharedCell = (j == centerIndex);

      auto weightLinear = (weights[i] + weights[j] * (sharedCell ? 0.5f : 1.f));
      auto offsetLinear = static_cast<std::int32_t>(offsets[i] + 1 / (1 + weights[i] / weights[j]));

      if (offsetLinear == 0) {
        linearSamplingMap.emplace_back(std::make_pair(offsets[i], weights[i]));
        linearSamplingMap.emplace_back(std::make_pair(offsets[i + 1], weights[i + 1]));
      }
      else {
        linearSamplingMap.emplace_back(std::make_pair(offsetLinear, weightLinear));
        linearSamplingMap.emplace_back(std::make_pair(-offsetLinear, weightLinear));
      }
    }
  }

  linearSamplingOffsets.resize(linearSamplingMap.size());
  linearSamplingWeights.resize(linearSamplingMap.size());

  for (unsigned int i = 0; i < linearSamplingMap.size(); ++i) {
    linearSamplingOffsets[i] = linearSamplingMap[i].first;
    linearSamplingWeights[i] = linearSamplingMap[i].second;
  }

  // Replace with optimized
  offsets = linearSamplingOffsets;
  weights = linearSamplingWeights;

  // Generate shaders
  int maxVaryingRows  = getEngine()->getCaps().maxVaryingVectors;
  int freeVaryingVec2 = std::max(maxVaryingRows, 0) - 1; // Because of sampleCenter

  int varyingCount = std::min(static_cast<int>(offsets.size()), freeVaryingVec2);

  std::ostringstream defines;
  defines << _staticDefines;

  // The DOF fragment should ignore the center pixel when looping as it is
  // handled manualy in the fragment shader.
  if (StringTools::contains(_staticDefines, "DOF")) {
    defines << "#define CENTER_WEIGHT "
            << _glslFloat(weights[static_cast<unsigned>(varyingCount) - 1]) << "\r\n";
    --varyingCount;
  }

  for (int i = 0; i < varyingCount; ++i) {
    defines << "#define KERNEL_OFFSET" << i << " " << _glslFloat(static_cast<float>(offsets[i]))
            << "\r\n";
    defines << "#define KERNEL_WEIGHT" << i << " " << _glslFloat(static_cast<float>(weights[i]))
            << "\r\n";
  }

  unsigned int depCount = 0;
  if (freeVaryingVec2 >= 0) {
    for (int i = freeVaryingVec2; i < static_cast<int>(offsets.size()); ++i) {
      defines << "#define KERNEL_DEP_OFFSET" << depCount << " "
              << _glslFloat(static_cast<float>(offsets[i])) << "\r\n";
      defines << "#define KERNEL_DEP_WEIGHT" << depCount << " "
              << _glslFloat(static_cast<float>(weights[i])) << "\r\n";
      ++depCount;
    }
  }

  if (packedFloat()) {
    defines << "#define PACKEDFLOAT 1";
  }

  blockCompilation = false;
  PostProcess::updateEffect(
    defines.str(), {}, {},
    {{"varyingCount", static_cast<unsigned>(varyingCount)}, {"depCount", depCount}}, onCompiled,
    onError);
}

float BlurPostProcess::_nearestBestKernel(float idealKernel) const
{
  float v = std::round(idealKernel);
  Float32Array vec{v, v - 1, v + 1, v - 2, v + 2};
  for (auto k : vec) {
    if (!stl_util::almost_equal(std::fmod(k, 2.f), 0.f)
        && (stl_util::almost_equal(std::fmod(std::floor(k / 2.f), 2.f), 0.f)) && k > 0.f) {
      return std::max(k, 3.f);
    }
  }
  return std::max(v, 3.f);
}

float BlurPostProcess::_gaussianWeight(float x) const
{
  // reference: Engines/ImageProcessingBlur.cpp #dcc760
  // We are evaluating the Gaussian (normal) distribution over a kernel parameter space of [-1,1],
  // so we truncate at three standard deviations by setting stddev (sigma) to 1/3.
  // The choice of 3-sigma truncation is common but arbitrary, and means that the signal is
  // truncated at around 1.3% of peak strength.

  // the distribution is scaled to account for the difference between the actual kernel size and the
  // requested kernel size
  const float sigma       = (1.f / 3.f);
  const float denominator = std::sqrt(2.f * Math::PI) * sigma;
  const float exponent    = -((x * x) / (2.f * sigma * sigma));
  const float weight      = (1.f / denominator) * std::exp(exponent);
  return weight;
}

std::string BlurPostProcess::_glslFloat(float x, unsigned int decimalFigures) const
{
  std::ostringstream oss;
  oss.precision(decimalFigures);
  oss << std::fixed << x;
  return oss.str();
}

BlurPostProcessPtr _Parse(const json& /*parsedPostProcess*/, const CameraPtr& /*targetCamera*/,
                          Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
