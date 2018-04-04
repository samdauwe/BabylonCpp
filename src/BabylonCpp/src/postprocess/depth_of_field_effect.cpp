#include <babylon/postprocess/depth_of_field_effect.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/variant.h>
#include <babylon/engine/scene.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/circle_of_confusion_post_process.h>
#include <babylon/postprocess/depth_of_field_blur_post_process.h>
#include <babylon/postprocess/depth_of_field_merge_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

DepthOfFieldEffect::DepthOfFieldEffect(Scene* scene,
                                       RenderTargetTexture* depthTexture,
                                       DepthOfFieldEffectBlurLevel blurLevel,
                                       unsigned int pipelineTextureType,
                                       bool blockCompilation)
    : PostProcessRenderEffect{
        scene->getEngine(), "depth of field",
        [&]() -> vector_t<PostProcess*> { return _effects; }, true}
{
  // Circle of confusion value for each pixel is used to determine how
  // much to blur that pixel
  _circleOfConfusion = ::std::make_unique<CircleOfConfusionPostProcess>(
    "circleOfConfusion", depthTexture,
    ToVariant<float, PostProcessOptions>(1.f), nullptr,
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
  auto adjustedKernelSize = kernelSize / ::std::pow(2, blurCount - 1);
  for (int i = 0; i < blurCount; i++) {
    auto blurY = ::std::make_unique<DepthOfFieldBlurPostProcess>(
      "verticle blur", scene, Vector2(0.f, 1.f), adjustedKernelSize,
      ToVariant<float, PostProcessOptions>(
        1.f / static_cast<float>(::std::pow(2.f, i))),
      nullptr, _depthOfFieldPass.get(),
      i == 0 ? _circleOfConfusion.get() : nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      pipelineTextureType, blockCompilation);
    blurY->autoClear = false;
    auto blurX       = ::std::make_unique<DepthOfFieldBlurPostProcess>(
      "horizontal blur", scene, Vector2(1.f, 0.f), adjustedKernelSize,
      ToVariant<float, PostProcessOptions>(
        1.f / static_cast<float>(::std::pow(2.f, i))),
      nullptr, _depthOfFieldPass.get(), nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      pipelineTextureType, blockCompilation);
    blurX->autoClear = false;
    _depthOfFieldBlurY.emplace_back(::std::move(blurY));
    _depthOfFieldBlurX.emplace_back(::std::move(blurX));
  }

  vector_t<PostProcess*> blurSteps;
  for (auto& depthOfFieldBlurX : _depthOfFieldBlurX) {
    blurSteps.emplace_back(static_cast<PostProcess*>(depthOfFieldBlurX.get()));
  }

  // Merge blurred images with original image based on circleOfConfusion
  _depthOfFieldMerge = ::std::make_unique<DepthOfFieldMergePostProcess>(
    "depthOfFieldMerge", _circleOfConfusion.get(), _circleOfConfusion.get(),
    blurSteps, ToVariant<float, PostProcessOptions>(1.f), nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    pipelineTextureType, blockCompilation);
  _depthOfFieldMerge->autoClear = false;

  // Set all post processes on the effect.
  _effects = {_circleOfConfusion.get()};
  for (size_t i = 0; i < _depthOfFieldBlurX.size(); i++) {
    _effects.emplace_back(_depthOfFieldBlurY[i].get());
    _effects.emplace_back(_depthOfFieldBlurX[i].get());
  }
  _effects.emplace_back(_depthOfFieldMerge.get());
}

DepthOfFieldEffect::~DepthOfFieldEffect()
{
}

void DepthOfFieldEffect::setFocalLength(float value)
{
  _circleOfConfusion->focalLength = value;
}

float DepthOfFieldEffect::focalLength() const
{
  return _circleOfConfusion->focalLength;
}

void DepthOfFieldEffect::setFStop(float value)
{
  _circleOfConfusion->fStop = value;
}

float DepthOfFieldEffect::fStop() const
{
  return _circleOfConfusion->fStop;
}

void DepthOfFieldEffect::setFocusDistance(float value)
{
  _circleOfConfusion->focusDistance = value;
}

float DepthOfFieldEffect::focusDistance() const
{
  return _circleOfConfusion->focusDistance;
}

void DepthOfFieldEffect::setLensSize(float value)
{
  _circleOfConfusion->lensSize = value;
}

float DepthOfFieldEffect::lensSize() const
{
  return _circleOfConfusion->lensSize;
}

void DepthOfFieldEffect::setDepthTexture(RenderTargetTexture* value)
{
  _circleOfConfusion->setDepthTexture(value);
}

void DepthOfFieldEffect::disposeEffects(Camera* camera)
{
  _depthOfFieldPass->dispose(camera);
  _circleOfConfusion->dispose(camera);
  for (auto& element : _depthOfFieldBlurX) {
    element->dispose(camera);
  };
  for (auto& element : _depthOfFieldBlurY) {
    element->dispose(camera);
  };
  _depthOfFieldMerge->dispose(camera);
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
