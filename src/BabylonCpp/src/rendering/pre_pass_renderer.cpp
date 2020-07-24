#include <babylon/rendering/pre_pass_renderer.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/postprocesses/image_processing_post_process.h>
#include <babylon/postprocesses/post_process_manager.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/postprocesses/sub_surface_scattering_post_process.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

PrePassRenderer::PrePassRenderer(Scene* scene)
    : prePassRT{nullptr}
    , imageProcessingPostProcess{nullptr}
    , subSurfaceConfiguration{nullptr}
    , materialsShouldRenderGeometry{false}
    , materialsShouldRenderIrradiance{false}
    , enabled{this, &PrePassRenderer::get_enabled}
    , samples{this, &PrePassRenderer::get_samples, &PrePassRenderer::set_samples}
    , isSupported{this, &PrePassRenderer::get_isSupported}
    , _isDirty{false}
    , _mrtTypes{
          Constants::TEXTURETYPE_HALF_FLOAT, // Original color
          Constants::TEXTURETYPE_HALF_FLOAT, // Irradiance
          Constants::TEXTURETYPE_HALF_FLOAT, // Depth (world units)
          Constants::TEXTURETYPE_UNSIGNED_INT // Albedo
    }
    , _clearColor{Color4(0.f, 0.f, 0.f, 0.f)}
    , _enabled{false}
{
  _scene  = scene;
  _engine = scene->getEngine();

  // PrePassRenderer::_SceneComponentInitialization(_scene);

  subSurfaceConfiguration = std::make_unique<SubSurfaceConfiguration>(_scene);
}

bool PrePassRenderer::get_enabled() const
{
  return _enabled;
}

unsigned int PrePassRenderer::get_samples() const
{
  return prePassRT->samples();
}

void PrePassRenderer::set_samples(unsigned int n)
{
  if (!imageProcessingPostProcess) {
    _createCompositionEffect();
  }

  prePassRT->samples = n;
}

void PrePassRenderer::_initializeAttachments()
{
  auto& gl = *_engine->_gl;

  _multiRenderAttachments = {};
  _clearAttachments       = {GL::NONE};
  _defaultAttachments     = {GL::COLOR_ATTACHMENT0};

  for (size_t i = 0; i < mrtCount; ++i) {
    const auto iStr = std::to_string(i);
    _multiRenderAttachments.emplace_back(gl["COLOR_ATTACHMENT" + iStr]);

    if (i > 0) {
      _clearAttachments.emplace_back(gl["COLOR_ATTACHMENT" + iStr]);
      _defaultAttachments.emplace_back(GL::NONE);
    }
  }
}

void PrePassRenderer::_createCompositionEffect()
{
  IMultiRenderTargetOptions options;
  options.generateMipMaps      = false;
  options.generateDepthTexture = true;
  options.defaultType          = Constants::TEXTURETYPE_UNSIGNED_INT;
  options.types                = _mrtTypes;
  prePassRT                    = std::make_shared<MultiRenderTarget>("sceneprePassRT",
                                                  RenderTargetSize{
                                                    _engine->getRenderWidth(), // width
                                                    _engine->getRenderHeight() // height
                                                  },
                                                  mrtCount, _scene, options);
  prePassRT->samples           = 1;

  _initializeAttachments();

  imageProcessingPostProcess
    = ImageProcessingPostProcess::New("sceneCompositionPass", 1, nullptr, std::nullopt, _engine);
  imageProcessingPostProcess->autoClear = false;
}

bool PrePassRenderer::get_isSupported() const
{
  return _engine->webGLVersion() > 1.f;
}

void PrePassRenderer::bindAttachmentsForEffect(Effect& effect)
{
  if (enabled()) {
    if (effect._multiTarget) {
      _engine->bindAttachments(_multiRenderAttachments);
    }
    else {
      _engine->bindAttachments(_defaultAttachments);
    }
  }
}

void PrePassRenderer::_beforeCameraDraw()
{
  if (_isDirty) {
    _update();
  }

  _bindFrameBuffer();
}

void PrePassRenderer::_afterCameraDraw()
{
  if (_enabled) {
    const PostProcessPtr firstCameraPP
      = _scene->activeCamera() ? _scene->activeCamera()->_getFirstPostProcess() : nullptr;
    if (firstCameraPP) {
      _scene->postProcessManager->_prepareFrame();
    }
    _scene->postProcessManager->directRender(
      _postProcesses, firstCameraPP ? firstCameraPP->inputTexture() : nullptr);
  }
}

void PrePassRenderer::_checkRTSize()
{
  const auto requiredWidth  = _engine->getRenderWidth(true);
  const auto requiredHeight = _engine->getRenderHeight(true);
  const auto width          = prePassRT->getRenderWidth();
  const auto height         = prePassRT->getRenderHeight();

  if (width != requiredWidth || height != requiredHeight) {
    prePassRT->resize(Size{
      requiredWidth,  // width
      requiredHeight, // height
    });

    _bindPostProcessChain();
  }
}

void PrePassRenderer::_bindFrameBuffer()
{
  if (_enabled) {
    _checkRTSize();
    const auto internalTexture = prePassRT->getInternalTexture();
    if (internalTexture) {
      _engine->bindFramebuffer(internalTexture);
    }
  }
}

void PrePassRenderer::clear()
{
  if (_enabled) {
    _bindFrameBuffer();

    // Regular clear color with the scene clear color of the 1st attachment
    _engine->clear(_scene->clearColor,
                   _scene->autoClear || _scene->forceWireframe() || _scene->forcePointsCloud(),
                   _scene->autoClearDepthAndStencil, _scene->autoClearDepthAndStencil);

    // Clearing other attachment with 0 on all other attachments
    _engine->bindAttachments(_clearAttachments);
    _engine->clear(_clearColor, true, false, false);
    _engine->bindAttachments(_multiRenderAttachments);
  }
}

void PrePassRenderer::_setState(bool enabled)
{
  _enabled        = enabled;
  _scene->prePass = enabled;

  if (imageProcessingPostProcess) {
    imageProcessingPostProcess->imageProcessingConfiguration()->applyByPostProcess = enabled;
  }
}

void PrePassRenderer::_enable()
{
  _resetPostProcessChain();

  if (subSurfaceConfiguration->enabled) {
    if (!subSurfaceConfiguration->postProcess) {
      subSurfaceConfiguration->createPostProcess();
    }

    _postProcesses.emplace_back(subSurfaceConfiguration->postProcess);
  }

  if (!imageProcessingPostProcess) {
    _createCompositionEffect();
  }

  _postProcesses.emplace_back(imageProcessingPostProcess);
  _bindPostProcessChain();
  _setState(true);
}

void PrePassRenderer::_disable()
{
  _setState(false);
  subSurfaceConfiguration->enabled = false;
  materialsShouldRenderGeometry    = false;
  materialsShouldRenderIrradiance  = false;
}

void PrePassRenderer::_resetPostProcessChain()
{
  _postProcesses = {};
  if (imageProcessingPostProcess) {
    imageProcessingPostProcess->restoreDefaultInputTexture();
  }

  if (subSurfaceConfiguration->postProcess) {
    subSurfaceConfiguration->postProcess->restoreDefaultInputTexture();
  }
}

void PrePassRenderer::_bindPostProcessChain()
{
  _postProcesses[0]->inputTexture = prePassRT->getInternalTexture();
}

void PrePassRenderer::markAsDirty()
{
  _isDirty = true;
}

void PrePassRenderer::_update()
{
  _disable();
  auto enablePrePass = false;

  // Subsurface scattering
  for (const auto& material : _scene->materials) {
    if (material->setPrePassRenderer(shared_from_this())) {
      enablePrePass = true;
    }
  }

  const auto pipelines = _scene->postProcessRenderPipelineManager()->supportedPipelines();
  for (const auto& pipeline : pipelines) {
    if (pipeline->setPrePassRenderer(shared_from_this())) {
      enablePrePass = true;
    }
  }

  _isDirty = false;

  if (enablePrePass) {
    _enable();
  }

  if (!enabled()) {
    _engine->bindAttachments(_defaultAttachments);
  }
}

void PrePassRenderer::dispose()
{
  imageProcessingPostProcess->dispose();
  subSurfaceConfiguration->dispose();
  prePassRT->dispose();
}

} // end of namespace BABYLON
