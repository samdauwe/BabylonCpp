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
                                       DepthOfFieldEffectBlurLevel blurLevel,
                                       unsigned int pipelineTextureType)
    : PostProcessRenderEffect{
        scene->getEngine(), "depth of field",
        [&]() -> vector_t<PostProcess*> {
          // Enable and get current depth map
          auto depthMap = scene->enableDepthRenderer()->getDepthMap();
          // Circle of confusion value for each pixel is used to determine how
          // much to blur that pixel
          _circleOfConfusion = ::std::make_unique<CircleOfConfusionPostProcess>(
            "circleOfConfusion", scene, depthMap,
            ToVariant<float, PostProcessOptions>(1.f), nullptr,
            TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
            pipelineTextureType);
          // Capture circle of confusion texture
          _depthOfFieldPass = ::std::make_unique<PassPostProcess>(
            "depthOfFieldPass", 1.0, nullptr,
            TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
            pipelineTextureType);
          _depthOfFieldPass->autoClear = false;

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
              TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(),
              false, pipelineTextureType);
            blurY->autoClear = false;
            auto blurX       = ::std::make_unique<DepthOfFieldBlurPostProcess>(
              "horizontal blur", scene, Vector2(1.f, 0.f), adjustedKernelSize,
              ToVariant<float, PostProcessOptions>(
                1.f / static_cast<float>(::std::pow(2.f, i))),
              nullptr, _depthOfFieldPass.get(), nullptr,
              TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(),
              false, pipelineTextureType);
            blurX->autoClear = false;
            _depthOfFieldBlurY.emplace_back(::std::move(blurY));
            _depthOfFieldBlurX.emplace_back(::std::move(blurX));
          }

          // Merge blurred images with original image based on circleOfConfusion
          vector_t<PostProcess*> blurSteps;
          blurSteps.reserve(_depthOfFieldBlurY.size());
          for (auto& blurStep : _depthOfFieldBlurY) {
            blurSteps.emplace_back(blurStep.get());
          }
          _depthOfFieldMerge = ::std::make_unique<DepthOfFieldMergePostProcess>(
            "depthOfFieldMerge", _circleOfConfusion.get(),
            _depthOfFieldPass.get(), stl_util::slice(blurSteps, 1),
            ToVariant<float, PostProcessOptions>(1.f), nullptr,
            TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
            pipelineTextureType);
          _depthOfFieldMerge->autoClear = false;

          // Set all post processes on the effect.
          vector_t<PostProcess*> effects{_circleOfConfusion.get(),
                                         _depthOfFieldPass.get()};
          for (size_t i = 0; i < _depthOfFieldBlurX.size(); i++) {
            effects.emplace_back(_depthOfFieldBlurY[i].get());
            effects.emplace_back(_depthOfFieldBlurX[i].get());
          }
          effects.emplace_back(_depthOfFieldMerge.get());
          return effects;
        },
        true}
{
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

} // end of namespace BABYLON
