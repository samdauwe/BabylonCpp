#include <babylon/postprocess/bloom_effect.h>

#include <babylon/engine/scene.h>
#include <babylon/postprocess/bloom_merge_post_process.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/extract_highlights_post_process.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

BloomEffect::BloomEffect(const string_t& name, Scene* scene, float bloomScale,
                         float bloomWeight, float bloomKernel,
                         unsigned int pipelineTextureType,
                         bool blockCompilation)
    : PostProcessRenderEffect{scene->getEngine(), "bloom",
                              [&]() { return _effects; }, true}
    , threshold{this, &BloomEffect::get_threshold, &BloomEffect::set_threshold}
    , weight{this, &BloomEffect::get_weight, &BloomEffect::set_weight}
    , kernel{this, &BloomEffect::get_kernel, &BloomEffect::set_kernel}
{
  _name      = name;
  _downscale = new ExtractHighlightsPostProcess(
    "highlights", ToVariant<float, PostProcessOptions>(1.f), nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    pipelineTextureType, blockCompilation);

  _blurX = new BlurPostProcess("horizontal blur", Vector2(1.f, 0.f), 10.0,
                               ToVariant<float, PostProcessOptions>(bloomScale),
                               nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
                               scene->getEngine(), false, pipelineTextureType,
                               "", blockCompilation);
  _blurX->alwaysForcePOT = true;
  _blurX->autoClear      = false;

  _blurY = new BlurPostProcess("vertical blur", Vector2(0.f, 1.f), 10.0,
                               ToVariant<float, PostProcessOptions>(bloomScale),
                               nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
                               scene->getEngine(), false, pipelineTextureType,
                               "", blockCompilation);
  _blurY->alwaysForcePOT = true;
  _blurY->autoClear      = false;

  kernel = bloomKernel;

  _effects = {_downscale, _blurX, _blurY};

  _merge = new BloomMergePostProcess(
    "bloomMerge", _downscale, _blurY, bloomWeight,
    ToVariant<float, PostProcessOptions>(bloomScale), nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    pipelineTextureType, blockCompilation);
  _merge->autoClear = false;
  _effects.emplace_back(_merge);
}

BloomEffect::~BloomEffect()
{
}

float BloomEffect::get_threshold() const
{
  return _downscale->threshold;
}

void BloomEffect::set_threshold(float value)
{
  _downscale->threshold = value;
}

float BloomEffect::get_weight() const
{
  return _merge->weight;
}

void BloomEffect::set_weight(float value)
{
  _merge->weight = value;
}

float BloomEffect::get_kernel() const
{
  return _blurX->kernel / bloomScale;
}

void BloomEffect::set_kernel(float value)
{
  _blurX->kernel = value * bloomScale;
  _blurY->kernel = value * bloomScale;
}

void BloomEffect::disposeEffects(Camera* camera)
{
  for (const auto& effect : _effects) {
    effect->dispose(camera);
  }
}

void BloomEffect::_updateEffects()
{
  for (const auto& effect : _effects) {
    effect->updateEffect();
  }
}

bool BloomEffect::_isReady() const
{
  for (const auto& effect : _effects) {
    if (!effect->isReady()) {
      return false;
    }
  }
  return true;
}

} // end of namespace BABYLON
