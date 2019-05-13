#include <babylon/postprocesses/depth_of_field_effect.h>

#include <cmath>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/scene.h>
#include <babylon/postprocesses/blur_post_process.h>
#include <babylon/postprocesses/circle_of_confusion_post_process.h>
#include <babylon/postprocesses/depth_of_field_blur_post_process.h>
#include <babylon/postprocesses/depth_of_field_merge_post_process.h>
#include <babylon/postprocesses/pass_post_process.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

DepthOfFieldEffect::DepthOfFieldEffect(Scene* scene,
                                       RenderTargetTexture* iDepthTexture,
                                       DepthOfFieldEffectBlurLevel blurLevel,
                                       unsigned int pipelineTextureType,
                                       bool blockCompilation)
    : PostProcessRenderEffect{scene->getEngine(), "depth of field",
                              [&]() -> std::vector<PostProcessPtr> {
                                return _effects;
                              },
                              true}
    , focalLength{this, &DepthOfFieldEffect::get_focalLength,
                  &DepthOfFieldEffect::set_focalLength}
    , fStop{this, &DepthOfFieldEffect::get_fStop,
            &DepthOfFieldEffect::set_fStop}
    , focusDistance{this, &DepthOfFieldEffect::get_focusDistance,
                    &DepthOfFieldEffect::set_focusDistance}
    , lensSize{this, &DepthOfFieldEffect::get_lensSize,
               &DepthOfFieldEffect::set_lensSize}
    , depthTexture{this, &DepthOfFieldEffect::set_depthTexture}
{
  // Circle of confusion value for each pixel is used to determine how
  // much to blur that pixel
  _circleOfConfusion = std::make_unique<CircleOfConfusionPostProcess>(
    "circleOfConfusion", iDepthTexture, 1.f, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    pipelineTextureType, blockCompilation);

  // Create a pyramid of blurred images (eg. fullSize 1/4 blur, half
  // size 1/2 blur, quarter size 3/4 blur, eith size 4/4 blur) Blur the
  // image but do not blur on sharp far to near distance changes to
  // avoid bleeding artifacts See section 2.6.2
  // http://fileadmin.cs.lth.se/cs/education/edan35/lectures/12dof.pdf
  _depthOfFieldBlurY.clear();
  _depthOfFieldBlurX.clear();
  auto blurCount  = 1;
  auto kernelSize = 15;
  switch (blurLevel) {
    case DepthOfFieldEffectBlurLevel::High: {
      blurCount  = 3;
      kernelSize = 51;
      break;
    }
    case DepthOfFieldEffectBlurLevel::Medium: {
      blurCount  = 2;
      kernelSize = 31;
      break;
    }
    default: {
      kernelSize = 15;
      blurCount  = 1;
      break;
    }
  }
  auto adjustedKernelSize
    = static_cast<float>(kernelSize / std::pow(2, blurCount - 1));
  auto ratio = 1.f;
  for (int i = 0; i < blurCount; ++i) {
    auto blurY = DepthOfFieldBlurPostProcess::New(
      "verticle blur", scene, Vector2(0.f, 1.f), adjustedKernelSize, ratio,
      nullptr, _depthOfFieldPass.get(),
      i == 0 ? _circleOfConfusion.get() : nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      pipelineTextureType, blockCompilation);
    blurY->autoClear = false;
    ratio            = 0.75f / std::pow(2.f, static_cast<float>(i));
    auto blurX       = DepthOfFieldBlurPostProcess::New(
      "horizontal blur", scene, Vector2(1.f, 0.f), adjustedKernelSize, ratio,
      nullptr, _depthOfFieldPass.get(), nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      pipelineTextureType, blockCompilation);
    blurX->autoClear = false;
    _depthOfFieldBlurY.emplace_back(blurY);
    _depthOfFieldBlurX.emplace_back(blurX);
  }

  std::vector<PostProcessPtr> blurSteps;
  for (auto& depthOfFieldBlurX : _depthOfFieldBlurX) {
    blurSteps.emplace_back(
      std::dynamic_pointer_cast<PostProcess>(depthOfFieldBlurX));
  }

  // Set all post processes on the effect.
  _effects = {_circleOfConfusion};
  for (size_t i = 0; i < _depthOfFieldBlurX.size(); i++) {
    _effects.emplace_back(_depthOfFieldBlurY[i]);
    _effects.emplace_back(_depthOfFieldBlurX[i]);
  }

  // Cast DepthOfFieldBlurPostProcess to PostProcess
  std::vector<PostProcessPtr> castedDepthOfFieldBlurX;
  for (const auto& depthOfFieldBlurX : _depthOfFieldBlurX) {
    castedDepthOfFieldBlurX.emplace_back(
      std::static_pointer_cast<PostProcess>(depthOfFieldBlurX));
  }

  // Merge blurred images with original image based on circleOfConfusion
  _dofMerge = DepthOfFieldMergePostProcess::New(
    "dofMerge", _circleOfConfusion, _circleOfConfusion, castedDepthOfFieldBlurX,
    ratio, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(),
    false, pipelineTextureType, blockCompilation);
  _dofMerge->autoClear = false;
  _effects.emplace_back(_dofMerge);
}

DepthOfFieldEffect::~DepthOfFieldEffect()
{
}

void DepthOfFieldEffect::set_focalLength(float value)
{
  _circleOfConfusion->focalLength = value;
}

float DepthOfFieldEffect::get_focalLength() const
{
  return _circleOfConfusion->focalLength;
}

void DepthOfFieldEffect::set_fStop(float value)
{
  _circleOfConfusion->fStop = value;
}

float DepthOfFieldEffect::get_fStop() const
{
  return _circleOfConfusion->fStop;
}

void DepthOfFieldEffect::set_focusDistance(float value)
{
  _circleOfConfusion->focusDistance = value;
}

float DepthOfFieldEffect::get_focusDistance() const
{
  return _circleOfConfusion->focusDistance;
}

void DepthOfFieldEffect::set_lensSize(float value)
{
  _circleOfConfusion->lensSize = value;
}

float DepthOfFieldEffect::get_lensSize() const
{
  return _circleOfConfusion->lensSize;
}

std::string DepthOfFieldEffect::getClassName() const
{
  return "DepthOfFieldEffect";
}

void DepthOfFieldEffect::set_depthTexture(const RenderTargetTexturePtr& value)
{
  _circleOfConfusion->depthTexture = value;
}

void DepthOfFieldEffect::disposeEffects(Camera* camera)
{
  for (auto& effect : _effects) {
    effect->dispose(camera);
  }
}

void DepthOfFieldEffect::_updateEffects()
{
  for (auto& effect : _effects) {
    effect->updateEffect();
  }
}

bool DepthOfFieldEffect::_isReady() const
{
  for (auto& effect : _effects) {
    if (!effect->isReady()) {
      return false;
    }
  }
  return true;
}

} // end of namespace BABYLON
