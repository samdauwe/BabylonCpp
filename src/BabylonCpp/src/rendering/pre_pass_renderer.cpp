#include <babylon/rendering/pre_pass_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/pre_pass_render_target.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/postprocesses/image_processing_post_process.h>
#include <babylon/postprocesses/post_process_manager.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/postprocesses/sub_surface_scattering_post_process.h>
#include <babylon/rendering/geometry_buffer_renderer.h>
#include <babylon/rendering/pre_pass_effect_configuration.h>
#include <babylon/rendering/pre_pass_renderer_scene_component.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

std::vector<TextureFormatMapping> PrePassRenderer::_textureFormats = {
  {
    Constants::PREPASS_IRRADIANCE_TEXTURE_TYPE, // type
    Constants::TEXTURETYPE_HALF_FLOAT,          // format
  },
  {
    Constants::PREPASS_POSITION_TEXTURE_TYPE, // type
    Constants::TEXTURETYPE_HALF_FLOAT,        // format
  },
  {
    Constants::PREPASS_VELOCITY_TEXTURE_TYPE, // type
    Constants::TEXTURETYPE_UNSIGNED_INT,      // format
  },
  {
    Constants::PREPASS_REFLECTIVITY_TEXTURE_TYPE, // type
    Constants::TEXTURETYPE_UNSIGNED_INT,          // format
  },
  {
    Constants::PREPASS_COLOR_TEXTURE_TYPE, // type
    Constants::TEXTURETYPE_HALF_FLOAT,     // format
  },
  {
    Constants::PREPASS_DEPTH_TEXTURE_TYPE, // type
    Constants::TEXTURETYPE_HALF_FLOAT,     // format
  },
  {
    Constants::PREPASS_NORMAL_TEXTURE_TYPE, // type
    Constants::TEXTURETYPE_HALF_FLOAT,      // format
  },
  {
    Constants::PREPASS_ALBEDO_TEXTURE_TYPE, // type
    Constants::TEXTURETYPE_UNSIGNED_INT,    // format
  },
};

PrePassRenderer::PrePassRenderer(Scene* scene)
    : mrtCount{0}
    , samples{this, &PrePassRenderer::get_samples, &PrePassRenderer::set_samples}
    , defaultRT{nullptr}
    , currentRTisSceneRT{this, &PrePassRenderer::get_currentRTisSceneRT}
    , doNotUseGeometryRendererFallback{false}
    , enabled{this, &PrePassRenderer::get_enabled}
    , disableGammaTransform{false}
    , isSupported{this, &PrePassRenderer::get_isSupported}
    , _isDirty{false}
    , _geometryBuffer{nullptr}
    , _currentTarget{nullptr}
    , _clearColor{Color4(0.f, 0.f, 0.f, 0.f)}
    , _enabled{false}
    , _needsCompositionForThisPass{false}
{
  _scene  = scene;
  _engine = scene->getEngine();

  // Register the G Buffer component to the scene.
  auto component = std::static_pointer_cast<PrePassRendererSceneComponent>(
    scene->_getComponent(SceneComponentConstants::NAME_PREPASSRENDERER));
  if (!component) {
    component = PrePassRendererSceneComponent::New(scene);
    scene->_addComponent(component);
  }
  defaultRT = _createRenderTarget("sceneprePassRT", nullptr);
  _setRenderTarget(nullptr);
}

int PrePassRenderer::getIndex(unsigned int type)
{
  return stl_util::contains(_textureIndices, type) ? _textureIndices[type] : -1;
}

unsigned int PrePassRenderer::get_samples() const
{
  return defaultRT->samples();
}

void PrePassRenderer::set_samples(unsigned int n)
{
  defaultRT->samples = n;
}

PrePassRenderTargetPtr& PrePassRenderer::getRenderTarget()
{
  return _currentTarget;
}

void PrePassRenderer::_setRenderTarget(const PrePassRenderTargetPtr& prePassRenderTarget)
{
  if (prePassRenderTarget) {
    _currentTarget = prePassRenderTarget;
  }
  else {
    _currentTarget = defaultRT;
  }
}

bool PrePassRenderer::get_currentRTisSceneRT() const
{
  return _currentTarget == defaultRT;
}

void PrePassRenderer::_refreshGeometryBufferRendererLink()
{
  if (!doNotUseGeometryRendererFallback) {
    _geometryBuffer = _scene->enableGeometryBufferRenderer();

    if (!_geometryBuffer) {
      // Not supported
      doNotUseGeometryRendererFallback = true;
      return;
    }

    _geometryBuffer->_linkPrePassRenderer(shared_from_this());
  }
  else {
    if (_geometryBuffer) {
      _geometryBuffer->_unlinkPrePassRenderer();
    }
    _geometryBuffer = nullptr;
    _scene->disableGeometryBufferRenderer();
  }
}

bool PrePassRenderer::get_enabled() const
{
  return _enabled;
}

PrePassRenderTargetPtr
PrePassRenderer::_createRenderTarget(const std::string& name,
                                     const RenderTargetTexturePtr& renderTargetTexture)
{
  IMultiRenderTargetOptions options;
  options.generateMipMaps                    = false;
  options.generateStencilBuffer              = _engine->isStencilEnable();
  options.defaultType                        = Constants::TEXTURETYPE_UNSIGNED_INT;
  options.types                              = {};
  options.drawOnlyOnFirstAttachmentByDefault = true;

  const auto rt = std::make_shared<PrePassRenderTarget>(name, renderTargetTexture,
                                                        RenderTargetSize{
                                                          _engine->getRenderWidth(), // width
                                                          _engine->getRenderHeight() // height
                                                        },
                                                        0ull, _scene, options);

  renderTargets.emplace_back(rt);

  return rt;
}

bool PrePassRenderer::get_isSupported() const
{
  return _scene->getEngine()->getCaps().drawBuffersExtension;
}

void PrePassRenderer::bindAttachmentsForEffect(Effect& effect, SubMesh* subMesh)
{
  if (enabled() && _currentTarget->enabled) {
    if (effect._multiTarget) {
      _engine->bindAttachments(_multiRenderAttachments);
    }
    else {
      _engine->bindAttachments(_defaultAttachments);

      if (_geometryBuffer && currentRTisSceneRT()) {
        const auto material = subMesh->getMaterial();
        if (material && !material->isPrePassCapable()
            && stl_util::index_of(excludedMaterials, material) == -1) {
          _geometryBuffer->renderList().emplace_back(subMesh->getRenderingMesh().get());
        }
      }
    }
  }
}

void PrePassRenderer::_reinitializeAttachments()
{
  std::vector<bool> multiRenderLayout = {};
  std::vector<bool> clearLayout       = {false};
  std::vector<bool> defaultLayout     = {true};

  for (size_t i = 0; i < mrtCount; ++i) {
    multiRenderLayout.emplace_back(true);

    if (i > 0) {
      clearLayout.emplace_back(true);
      defaultLayout.emplace_back(false);
    }
  }

  _multiRenderAttachments = _engine->buildTextureLayout(multiRenderLayout);
  _clearAttachments       = _engine->buildTextureLayout(clearLayout);
  _defaultAttachments     = _engine->buildTextureLayout(defaultLayout);
}

void PrePassRenderer::_resetLayout()
{
  _textureIndices.resize(PrePassRenderer::_textureFormats.back().type + 1);

  for (const auto& textureFormat : PrePassRenderer::_textureFormats) {
    _textureIndices[textureFormat.type] = -1;
  }

  _textureIndices[Constants::PREPASS_COLOR_TEXTURE_TYPE] = 0;
  _mrtLayout                                             = {Constants::PREPASS_COLOR_TEXTURE_TYPE};
  _mrtFormats                                            = {Constants::TEXTURETYPE_HALF_FLOAT};
  mrtCount                                               = 1;
}

void PrePassRenderer::_updateGeometryBufferLayout()
{
  _refreshGeometryBufferRendererLink();

  if (_geometryBuffer) {
    _geometryBuffer->_resetLayout();

    std::vector<bool> texturesActivated{};

    for (size_t i = 0; i < _mrtLayout.size(); ++i) {
      texturesActivated.emplace_back(false);
    }

    _geometryBuffer->_linkInternalTexture(defaultRT->getInternalTexture());

    struct ReplaceTextureFormatMapping {
      unsigned int prePassConstant        = 0;
      unsigned int geometryBufferConstant = 0;
    }; // end of struct ReplaceTextureFormatMapping

    static const std::vector<ReplaceTextureFormatMapping> matches = {
      {
        Constants::PREPASS_DEPTH_TEXTURE_TYPE,      // prePassConstant
        GeometryBufferRenderer::DEPTH_TEXTURE_TYPE, // geometryBufferConstant
      },
      {
        Constants::PREPASS_NORMAL_TEXTURE_TYPE,      // prePassConstant
        GeometryBufferRenderer::NORMAL_TEXTURE_TYPE, // geometryBufferConstant
      },
      {
        Constants::PREPASS_POSITION_TEXTURE_TYPE,      // prePassConstant
        GeometryBufferRenderer::POSITION_TEXTURE_TYPE, // geometryBufferConstant
      },
      {
        Constants::PREPASS_REFLECTIVITY_TEXTURE_TYPE,      // prePassConstant
        GeometryBufferRenderer::REFLECTIVITY_TEXTURE_TYPE, // geometryBufferConstant
      },
      {
        Constants::PREPASS_VELOCITY_TEXTURE_TYPE,      // prePassConstant
        GeometryBufferRenderer::VELOCITY_TEXTURE_TYPE, // geometryBufferConstant
      },
    };

    // replace textures in the geometryBuffer RT
    for (const auto& match : matches) {
      const auto index = stl_util::index_of(_mrtLayout, match.prePassConstant);
      if (index != -1) {
        _geometryBuffer->_forceTextureType(match.geometryBufferConstant, index);
        texturesActivated[index] = true;
      }
    }

    _geometryBuffer->_setAttachments(_engine->buildTextureLayout(texturesActivated));
  }
}

void PrePassRenderer::restoreAttachments()
{
  if (enabled() && _currentTarget->enabled && !_defaultAttachments.empty()) {
    _engine->bindAttachments(_defaultAttachments);
  }
}

void PrePassRenderer::_beforeDraw(Camera* camera, int /*faceIndex*/, int /*layer*/)
{
  // const previousEnabled = _enabled && _currentTarget.enabled;

  if (_isDirty) {
    _update();
  }

  if (!_enabled || !_currentTarget->enabled) {
    return;
  }

  if (_geometryBuffer) {
    _geometryBuffer->renderList = {};
  }

  _setupOutputForThisPass(_currentTarget, camera);
}

void PrePassRenderer::_prepareFrame(const PrePassRenderTargetPtr& prePassRenderTarget,
                                    int faceIndex, int layer)
{
  if (prePassRenderTarget->renderTargetTexture) {
    prePassRenderTarget->renderTargetTexture->_prepareFrame(
      _scene, faceIndex, layer,
      prePassRenderTarget->renderTargetTexture->useCameraPostProcesses.value_or(false));
  }
  else if (!_postProcessesSourceForThisPass.empty()) {
    _scene->postProcessManager->_prepareFrame();
  }
  else {
    _engine->restoreDefaultFramebuffer();
  }
}

void PrePassRenderer::_renderPostProcesses(const PrePassRenderTargetPtr& prePassRenderTarget,
                                           int faceIndex)
{
  const auto firstPP
    = !_postProcessesSourceForThisPass.empty() ? _postProcessesSourceForThisPass[0] : nullptr;
  auto outputTexture = firstPP ? firstPP->inputTexture() :
                       prePassRenderTarget->renderTargetTexture ?
                                 prePassRenderTarget->renderTargetTexture->getInternalTexture() :
                                 nullptr;

  // Build post process chain for this prepass post draw
  auto& postProcessChain = _currentTarget->_beforeCompositionPostProcesses;

  if (_needsCompositionForThisPass) {
    postProcessChain
      = stl_util::concat(postProcessChain, {_currentTarget->imageProcessingPostProcess});
  }

  // Activates and renders the chain
  if (!postProcessChain.empty()) {
    _scene->postProcessManager->_prepareFrame(_currentTarget->getInternalTexture(),
                                              postProcessChain);
    _scene->postProcessManager->directRender(postProcessChain, outputTexture, false, faceIndex);
  }
}

void PrePassRenderer::_afterDraw(int faceIndex, int layer)
{
  if (_enabled && _currentTarget->enabled) {
    _prepareFrame(_currentTarget, faceIndex, layer);
    _renderPostProcesses(_currentTarget, faceIndex);
  }
}

void PrePassRenderer::_clear()
{
  if (_enabled && _currentTarget->enabled) {
    _bindFrameBuffer(_currentTarget);

    // Clearing other attachment with 0 on all other attachments
    _engine->bindAttachments(_clearAttachments);
    _engine->clear(_clearColor, true, false, false);
    // Regular clear color with the scene clear color of the 1st attachment
    _engine->bindAttachments(_defaultAttachments);
  }
}

void PrePassRenderer::_bindFrameBuffer(const PrePassRenderTargetPtr& /*prePassRenderTarget*/)
{
  if (_enabled && _currentTarget->enabled) {
    _currentTarget->_checkSize();
    const auto internalTexture = _currentTarget->getInternalTexture();
    if (internalTexture) {
      _engine->bindFramebuffer(internalTexture);
    }
  }
}

void PrePassRenderer::_setEnabled(bool iEnabled)
{
  _enabled = iEnabled;
}

void PrePassRenderer::_setRenderTargetEnabled(const PrePassRenderTargetPtr& prePassRenderTarget,
                                              bool iEnabled)
{
  prePassRenderTarget->enabled = iEnabled;
}

PrePassEffectConfigurationPtr
PrePassRenderer::addEffectConfiguration(const PrePassEffectConfigurationPtr& cfg)
{
  // Do not add twice
  auto it = std::find_if(
    _effectConfigurations.begin(), _effectConfigurations.end(),
    [&cfg](const PrePassEffectConfigurationPtr& config) { return config->name() == cfg->name(); });
  if (it != _effectConfigurations.end()) {
    return *it;
  }

  _effectConfigurations.emplace_back(cfg);
  return cfg;
}

void PrePassRenderer::_enable()
{
  const auto previousMrtCount = mrtCount;

  for (const auto& effectConfiguration : _effectConfigurations) {
    if (effectConfiguration->enabled) {
      _enableTextures(effectConfiguration->texturesRequired());
    }
  }

  for (const auto& renderTarget : renderTargets) {
    if (mrtCount != previousMrtCount) {
      IMultiRenderTargetOptions options;
      options.types = _mrtFormats;
      renderTarget->updateCount(mrtCount, options);
    }

    renderTarget->_resetPostProcessChain();

    for (const auto& effectConfiguration : _effectConfigurations) {
      if (effectConfiguration->enabled) {
        // TODO : subsurface scattering has 1 scene-wide effect configuration
        // solution : do not stock postProcess on effectConfiguration, but in the
        // prepassRenderTarget (hashmap configuration => postProcess) And call createPostProcess
        // whenever the post process does not exist in the RT
        if (!effectConfiguration->postProcess) {
          effectConfiguration->createPostProcess();
        }

        if (effectConfiguration->postProcess) {
          renderTarget->_beforeCompositionPostProcesses.emplace_back(
            effectConfiguration->postProcess);
        }
      }
    }
  }

  _reinitializeAttachments();
  _setEnabled(true);
  _updateGeometryBufferLayout();
}

void PrePassRenderer::_disable()
{
  _setEnabled(false);

  for (const auto& renderTarget : renderTargets) {
    _setRenderTargetEnabled(renderTarget, false);
  }

  _resetLayout();

  for (const auto& effectConfiguration : _effectConfigurations) {
    effectConfiguration->enabled = false;
  }
}

std::vector<PostProcessPtr>
PrePassRenderer::_getPostProcessesSource(const PrePassRenderTargetPtr& prePassRenderTarget,
                                         Camera* camera)
{
  if (camera) {
    return camera->_postProcesses;
  }
  else if (prePassRenderTarget->renderTargetTexture) {
    if (prePassRenderTarget->renderTargetTexture->useCameraPostProcesses) {
      const auto iCamera = prePassRenderTarget->renderTargetTexture->activeCamera ?
                             prePassRenderTarget->renderTargetTexture->activeCamera :
                             _scene->activeCamera();
      return iCamera ? iCamera->_postProcesses : std::vector<PostProcessPtr>();
    }
    else if (!prePassRenderTarget->renderTargetTexture->postProcesses().empty()) {
      return prePassRenderTarget->renderTargetTexture->postProcesses();
    }
    else {
      return std::vector<PostProcessPtr>();
    }
  }
  else {
    return _scene->activeCamera() ? _scene->activeCamera()->_postProcesses :
                                    std::vector<PostProcessPtr>();
  }
}

void PrePassRenderer::_setupOutputForThisPass(const PrePassRenderTargetPtr& prePassRenderTarget,
                                              Camera* camera)
{
  // Order is : draw ===> prePassRenderTarget._postProcesses ==> ipp ==> camera._postProcesses
  const auto secondaryCamera
    = camera && !_scene->activeCameras.empty()
      && ((std::find_if(_scene->activeCameras.begin(), _scene->activeCameras.end(),
                        [camera](const CameraPtr& cam) { return cam.get() == camera; })
           != _scene->activeCameras.end())
          && _scene->activeCameras[0].get() != camera);
  _postProcessesSourceForThisPass = _getPostProcessesSource(prePassRenderTarget, camera);
  stl_util::erase_remove_if(_postProcessesSourceForThisPass,
                            [](const PostProcessPtr& pp) { return pp == nullptr; });
  _scene->autoClear = true;

  const auto cameraHasImageProcessing = _hasImageProcessing(_postProcessesSourceForThisPass);
  _needsCompositionForThisPass        = !cameraHasImageProcessing && !disableGammaTransform
                                 && _needsImageProcessing() && !secondaryCamera;

  const auto firstCameraPP  = _getFirstPostProcess(_postProcessesSourceForThisPass);
  const auto firstPrePassPP = !prePassRenderTarget->_beforeCompositionPostProcesses.empty() ?
                                prePassRenderTarget->_beforeCompositionPostProcesses[0] :
                                nullptr;
  PostProcessPtr firstPP    = nullptr;

  // Setting the scene-wide post process configuration
  _scene->imageProcessingConfiguration()->applyByPostProcess
    = _needsCompositionForThisPass || cameraHasImageProcessing;

  // Create composition effect if needed
  if (_needsCompositionForThisPass && !prePassRenderTarget->imageProcessingPostProcess) {
    prePassRenderTarget->_createCompositionEffect();
  }

  // Setting the prePassRenderTarget as input texture of the first PP
  if (firstPrePassPP) {
    firstPP = firstPrePassPP;
  }
  else if (_needsCompositionForThisPass) {
    firstPP = prePassRenderTarget->imageProcessingPostProcess;
  }
  else if (firstCameraPP) {
    firstPP = firstCameraPP;
  }

  _bindFrameBuffer(prePassRenderTarget);
  _linkInternalTexture(prePassRenderTarget, firstPP);
}

void PrePassRenderer::_linkInternalTexture(const PrePassRenderTargetPtr& prePassRenderTarget,
                                           const PostProcessPtr& postProcess)
{
  if (postProcess) {
    postProcess->autoClear    = false;
    postProcess->inputTexture = prePassRenderTarget->getInternalTexture();
  }

  if (prePassRenderTarget->_outputPostProcess != postProcess) {
    if (prePassRenderTarget->_outputPostProcess) {
      prePassRenderTarget->_outputPostProcess->restoreDefaultInputTexture();
    }
    prePassRenderTarget->_outputPostProcess = postProcess;
  }

  if (prePassRenderTarget->_internalTextureDirty) {
    _updateGeometryBufferLayout();
    prePassRenderTarget->_internalTextureDirty = false;
  }
}

bool PrePassRenderer::_needsImageProcessing() const
{
  for (const auto& effectConfiguration : _effectConfigurations) {
    if (effectConfiguration->enabled && effectConfiguration->needsImageProcessing.value_or(false)) {
      return true;
    }
  }

  return false;
}

bool PrePassRenderer::_hasImageProcessing(const std::vector<PostProcessPtr>& postProcesses) const
{
  auto isIPPAlreadyPresent = false;
  if (!postProcesses.empty()) {
    for (const auto& postProcess : postProcesses) {
      if (postProcess && postProcess->getClassName() == "ImageProcessingPostProcess") {
        isIPPAlreadyPresent = true;
        break;
      }
    }
  }

  return isIPPAlreadyPresent;
}

PostProcessPtr
PrePassRenderer::_getFirstPostProcess(const std::vector<PostProcessPtr>& postProcesses)
{
  for (const auto& postProcess : postProcesses) {
    if (postProcess != nullptr) {
      return postProcess;
    }
  }
  return nullptr;
}

void PrePassRenderer::markAsDirty()
{
  _isDirty = true;
}

void PrePassRenderer::_enableTextures(const std::vector<unsigned int>& types)
{
  // For velocity : enable storage of previous matrices for instances
  _scene->needsPreviousWorldMatrices = false;

  for (const auto& type : types) {
    if (_textureIndices[type] == -1) {
      _textureIndices[type] = static_cast<int>(_mrtLayout.size());
      _mrtLayout.emplace_back(type);

      _mrtFormats.emplace_back(_textureFormats[type].format);
      ++mrtCount;
    }

    if (type == Constants::PREPASS_VELOCITY_TEXTURE_TYPE) {
      _scene->needsPreviousWorldMatrices = true;
    }
  }
}

void PrePassRenderer::_update()
{
  _disable();
  auto enablePrePass                                         = false;
  _scene->imageProcessingConfiguration()->applyByPostProcess = false;

  for (const auto& material : _scene->materials) {
    if (material->setPrePassRenderer(shared_from_this())) {
      enablePrePass = true;
    }
  }

  if (enablePrePass) {
    _setRenderTargetEnabled(defaultRT, true);
  }

  std::vector<PostProcessPtr> postProcesses;

  for (const auto& renderTarget : renderTargets) {
    if (renderTarget->renderTargetTexture) {
      postProcesses = _getPostProcessesSource(renderTarget);
    }
    else {
      const auto camera = _scene->activeCamera();
      if (!camera) {
        continue;
      }

      postProcesses = camera->_postProcesses;
    }

    if (postProcesses.empty()) {
      continue;
    }

    stl_util::erase_remove_if(postProcesses,
                              [](const PostProcessPtr& pp) { return pp == nullptr; });

    if (!postProcesses.empty()) {
      for (const auto& postProcess : postProcesses) {
        if (postProcess->setPrePassRenderer(shared_from_this())) {
          _setRenderTargetEnabled(renderTarget, true);
          enablePrePass = true;
        }
      }

      if (_hasImageProcessing(postProcesses)) {
        _scene->imageProcessingConfiguration()->applyByPostProcess = true;
      }
    }
  }

  _markAllMaterialsAsPrePassDirty();
  _isDirty = false;

  if (enablePrePass) {
    _enable();
  }
}

void PrePassRenderer::_markAllMaterialsAsPrePassDirty()
{
  const auto& materials = _scene->materials;

  for (const auto& material : materials) {
    material->markAsDirty(Material::PrePassDirtyFlag);
  }
}

void PrePassRenderer::dispose()
{
  for (size_t i = renderTargets.size(); i-- > 0;) {
    renderTargets[i]->dispose();
  }
  renderTargets.clear();

  for (const auto& effectConfiguration : _effectConfigurations) {
    if (effectConfiguration) {
      effectConfiguration->dispose();
    }
  }
  _effectConfigurations.clear();
}

} // end of namespace BABYLON
