#include <babylon/postprocess/renderpipeline/pipelines/default_rendering_pipeline.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/fxaa_post_process.h>
#include <babylon/postprocess/highlights_post_process.h>
#include <babylon/postprocess/image_processing_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>

namespace BABYLON {

std::string DefaultRenderingPipeline::PassPostProcessId
  = "PassPostProcessEffect";
std::string DefaultRenderingPipeline::HighLightsPostProcessId
  = "HighLightsPostProcessEffect";
std::string DefaultRenderingPipeline::BlurXPostProcessId
  = "BlurXPostProcessEffect";
std::string DefaultRenderingPipeline::BlurYPostProcessId
  = "BlurYPostProcessEffect";
std::string DefaultRenderingPipeline::CopyBackPostProcessId
  = "CopyBackPostProcessEffect";
std::string DefaultRenderingPipeline::ImageProcessingPostProcessId
  = "ImageProcessingPostProcessEffect";
std::string DefaultRenderingPipeline::FxaaPostProcessId
  = "FxaaPostProcessEffect";
std::string DefaultRenderingPipeline::FinalMergePostProcessId
  = "FinalMergePostProcessEffect";

DefaultRenderingPipeline::DefaultRenderingPipeline(
  const std::string& iName, bool hdr, Scene* scene,
  const std::unordered_map<std::string, Camera*>& cameras)
    : PostProcessRenderPipeline(scene->getEngine(), iName)
    , pass{nullptr}
    , highlights{nullptr}
    , blurX{nullptr}
    , blurY{nullptr}
    , copyBack{nullptr}
    , fxaa{nullptr}
    , imageProcessing{nullptr}
    , finalMerge{nullptr}
    , bloomKernel{64.f}
    , _bloomEnabled{false}
    , _fxaaEnabled{false}
    , _imageProcessingEnabled{true}
    , _bloomScale{0.6f}
    , _bloomWeight{0.15f}
    , _hdr{hdr}
    , _scene{scene}
{
  _cameras = cameras;

  // Misc
  _defaultPipelineTextureType
    = scene->getEngine()->getCaps().textureFloatRender ?
        EngineConstants::TEXTURETYPE_FLOAT :
        EngineConstants::TEXTURETYPE_HALF_FLOAT;

  // Attach
  scene->postProcessRenderPipelineManager()->addPipeline(this);

  _buildPipeline();
}

DefaultRenderingPipeline::~DefaultRenderingPipeline()
{
}

void DefaultRenderingPipeline::setBloomWeight(float value)
{
  if (stl_util::almost_equal(_bloomWeight, value)) {
    return;
  }
  _bloomWeight = value;

  if (_hdr && copyBack) {
    copyBack->alphaConstants = Color4(value, value, value, value);
  }
}

float DefaultRenderingPipeline::bloomWeight() const
{
  return _bloomWeight;
}

void DefaultRenderingPipeline::setBloomScale(float value)
{
  if (stl_util::almost_equal(_bloomScale, value)) {
    return;
  }
  _bloomScale = value;

  _buildPipeline();
}

float DefaultRenderingPipeline::bloomScale() const
{
  return _bloomScale;
}

void DefaultRenderingPipeline::setBloomEnabled(bool enabled)
{
  if (_bloomEnabled == enabled) {
    return;
  }
  _bloomEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::bloomEnabled() const
{
  return _bloomEnabled;
}

void DefaultRenderingPipeline::setFxaaEnabled(bool enabled)
{
  if (_fxaaEnabled == enabled) {
    return;
  }
  _fxaaEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::fxaaEnabled() const
{
  return _fxaaEnabled;
}

void DefaultRenderingPipeline::setImageProcessingEnabled(bool enabled)
{
  if (_imageProcessingEnabled == enabled) {
    return;
  }
  _imageProcessingEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::imageProcessingEnabled() const
{
  return _imageProcessingEnabled;
}

void DefaultRenderingPipeline::_buildPipeline()
{
}

void DefaultRenderingPipeline::_disposePostProcesses()
{
  for (auto& item : _cameras) {
    auto camera = item.second;

    if (pass) {
      pass->dispose(camera);
      pass = nullptr;
    }

    if (highlights) {
      highlights->dispose(camera);
      highlights = nullptr;
    }

    if (blurX) {
      blurX->dispose(camera);
      blurX = nullptr;
    }

    if (blurY) {
      blurY->dispose(camera);
      blurY = nullptr;
    }

    if (copyBack) {
      copyBack->dispose(camera);
      copyBack = nullptr;
    }

    if (imageProcessing) {
      imageProcessing->dispose(camera);
      imageProcessing = nullptr;
    }

    if (fxaa) {
      fxaa->dispose(camera);
      fxaa = nullptr;
    }

    if (finalMerge) {
      finalMerge->dispose(camera);
      finalMerge = nullptr;
    }
  }
}

void DefaultRenderingPipeline::dispose(bool doNotRecurse)
{
  _disposePostProcesses();

  std::vector<Camera*> cameras;
  for (auto& item : _cameras) {
    cameras.emplace_back(item.second);
  }
  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
    _name, cameras);

  PostProcessRenderPipeline::dispose(doNotRecurse);
}

Json::object DefaultRenderingPipeline::serialize() const
{
  return Json::object();
}

std::unique_ptr<DefaultRenderingPipeline>
DefaultRenderingPipeline::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
