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
    Constants::TEXTURETYPE_HALF_FLOAT,        // format
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
    , prePassRT{nullptr}
    , imageProcessingPostProcess{nullptr}
    , enabled{this, &PrePassRenderer::get_enabled}
    , samples{this, &PrePassRenderer::get_samples, &PrePassRenderer::set_samples}
    , useGeometryBufferFallback{this, &PrePassRenderer::get_useGeometryBufferFallback,
                                &PrePassRenderer::set_useGeometryBufferFallback}
    , disableGammaTransform{false}
    , isSupported{this, &PrePassRenderer::get_isSupported}
    , _isDirty{false}
    , _clearColor{Color4(0.f, 0.f, 0.f, 0.f)}
    , _enabled{false}
    , _geometryBuffer{nullptr}
    , _useGeometryBufferFallback{false}
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

  _resetLayout();
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

bool PrePassRenderer::get_useGeometryBufferFallback() const
{
  return _useGeometryBufferFallback;
}

void PrePassRenderer::set_useGeometryBufferFallback(bool value)
{
  _useGeometryBufferFallback = value;

  if (value) {
    _geometryBuffer = _scene->enableGeometryBufferRenderer();

    if (!_geometryBuffer) {
      // Not supported
      _useGeometryBufferFallback = false;
      return;
    }

    _geometryBuffer->renderList = {};
    _geometryBuffer->_linkPrePassRenderer(shared_from_this());
    _updateGeometryBufferLayout();
  }
  else {
    if (_geometryBuffer) {
      _geometryBuffer->_unlinkPrePassRenderer();
    }
    _geometryBuffer = nullptr;
    _scene->disableGeometryBufferRenderer();
  }
}

MultiRenderTargetPtr& PrePassRenderer::getRenderTarget()
{
  return prePassRT;
}

void PrePassRenderer::_initializeAttachments()
{
  std::vector<bool> multiRenderLayout;
  std::vector<bool> clearLayout   = {false};
  std::vector<bool> defaultLayout = {true};

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

void PrePassRenderer::_createCompositionEffect()
{
  IMultiRenderTargetOptions options;
  options.generateMipMaps      = false;
  options.generateDepthTexture = true;
  options.defaultType          = Constants::TEXTURETYPE_UNSIGNED_INT;
  options.types                = _mrtFormats;
  prePassRT                    = std::make_shared<MultiRenderTarget>("sceneprePassRT",
                                                  RenderTargetSize{
                                                    _engine->getRenderWidth(), // width
                                                    _engine->getRenderHeight() // height
                                                  },
                                                  mrtCount, _scene, options);
  prePassRT->samples           = 1;

  _initializeAttachments();
  if (_useGeometryBufferFallback && !_geometryBuffer) {
    // Initializes the link with geometry buffer
    useGeometryBufferFallback = true;
  }

  imageProcessingPostProcess
    = ImageProcessingPostProcess::New("sceneCompositionPass", 1.f, nullptr, std::nullopt, _engine);
  imageProcessingPostProcess->autoClear = false;
}

bool PrePassRenderer::get_isSupported() const
{
  return _engine->webGLVersion() > 1.f || _scene->getEngine()->getCaps().drawBuffersExtension;
}

void PrePassRenderer::bindAttachmentsForEffect(Effect& effect, SubMesh* subMesh)
{
  if (enabled()) {
    if (effect._multiTarget) {
      _engine->bindAttachments(_multiRenderAttachments);
    }
    else {
      _engine->bindAttachments(_defaultAttachments);

      if (_geometryBuffer) {
        const auto material = subMesh->getMaterial();
        if (material && stl_util::index_of(excludedMaterials, material) == -1) {
          _geometryBuffer->renderList().emplace_back(subMesh->getRenderingMesh().get());
        }
      }
    }
  }
}

void PrePassRenderer::restoreAttachments()
{
  if (enabled() && !_defaultAttachments.empty()) {
    _engine->bindAttachments(_defaultAttachments);
  }
}

void PrePassRenderer::_beforeCameraDraw()
{
  if (_isDirty) {
    _update();
  }

  if (_geometryBuffer) {
    _geometryBuffer->renderList().clear();
  }

  _bindFrameBuffer();
}

void PrePassRenderer::_afterCameraDraw()
{
  if (_enabled) {
    const PostProcessPtr firstCameraPP
      = _scene->activeCamera() ? _scene->activeCamera()->_getFirstPostProcess() : nullptr;
    if (firstCameraPP && !_postProcesses.empty()) {
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

    _updateGeometryBufferLayout();
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
    _engine->bindAttachments(_defaultAttachments);
  }
}

void PrePassRenderer::_setState(bool iEnabled)
{
  _enabled        = iEnabled;
  _scene->prePass = iEnabled;

  if (imageProcessingPostProcess) {
    imageProcessingPostProcess->imageProcessingConfiguration()->applyByPostProcess = iEnabled;
  }
}

void PrePassRenderer::_updateGeometryBufferLayout()
{
  if (_geometryBuffer) {
    _geometryBuffer->_resetLayout();

    std::vector<bool> texturesActivated;

    for (size_t i = 0; i < _mrtLayout.size(); ++i) {
      texturesActivated.emplace_back(false);
    }

    _geometryBuffer->_linkInternalTexture(prePassRT->getInternalTexture());

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

int PrePassRenderer::getIndex(unsigned int type)
{
  return stl_util::contains(_textureIndices, type) ? _textureIndices[type] : -1;
}

void PrePassRenderer::_enable()
{
  const auto previousMrtCount = mrtCount;

  for (const auto& effectConfiguration : _effectConfigurations) {
    if (effectConfiguration->enabled) {
      _enableTextures(effectConfiguration->texturesRequired());
    }
  }

  if (prePassRT && mrtCount != previousMrtCount) {
    IMultiRenderTargetOptions option;
    option.types = _mrtFormats;
    prePassRT->updateCount(mrtCount, option);
  }

  _updateGeometryBufferLayout();
  _resetPostProcessChain();

  for (const auto& effectConfiguration : _effectConfigurations) {
    if (effectConfiguration->enabled) {
      if (!effectConfiguration->postProcess && effectConfiguration->createPostProcess()) {
        effectConfiguration->createPostProcess();
      }

      if (effectConfiguration->postProcess) {
        _postProcesses.emplace_back(effectConfiguration->postProcess);
      }
    }
  }

  _initializeAttachments();

  if (!imageProcessingPostProcess) {
    _createCompositionEffect();
  }
  auto isIPPAlreadyPresent = false;
  if (_scene->activeCamera() && !_scene->activeCamera()->_postProcesses.empty()) {
    for (const auto& postProcess : _scene->activeCamera()->_postProcesses) {
      if (postProcess && postProcess->getClassName() == "ImageProcessingPostProcess") {
        isIPPAlreadyPresent = true;
      }
    }
  }

  if (!isIPPAlreadyPresent && !disableGammaTransform) {
    _postProcesses.emplace_back(imageProcessingPostProcess);
  }
  _bindPostProcessChain();
  _setState(true);
}

void PrePassRenderer::_disable()
{
  _setState(false);
  _resetLayout();

  for (const auto& effectConfiguration : _effectConfigurations) {
    effectConfiguration->enabled = false;
  }
}

void PrePassRenderer::_resetLayout()
{
  _textureIndices.resize(_textureFormats.back().type + 1);

  for (const auto& textureFormat : _textureFormats) {
    _textureIndices[textureFormat.type] = -1;
  }

  _textureIndices[Constants::PREPASS_COLOR_TEXTURE_TYPE] = 0;
  _mrtLayout                                             = {Constants::PREPASS_COLOR_TEXTURE_TYPE};
  _mrtFormats                                            = {Constants::TEXTURETYPE_HALF_FLOAT};
  mrtCount                                               = 1;
}

void PrePassRenderer::_resetPostProcessChain()
{
  _postProcesses = {};
  if (imageProcessingPostProcess) {
    imageProcessingPostProcess->restoreDefaultInputTexture();
  }

  for (const auto& effectConfiguration : _effectConfigurations) {
    if (effectConfiguration->postProcess) {
      effectConfiguration->postProcess->restoreDefaultInputTexture();
    }
  }
}

void PrePassRenderer::_bindPostProcessChain()
{
  if (!_postProcesses.empty()) {
    _postProcesses[0]->inputTexture = prePassRT->getInternalTexture();
  }
  else {
    const PostProcessPtr pp
      = _scene->activeCamera() ? _scene->activeCamera()->_getFirstPostProcess() : nullptr;
    if (pp && prePassRT->getInternalTexture()) {
      pp->inputTexture = prePassRT->getInternalTexture();
    }
  }
}

void PrePassRenderer::markAsDirty()
{
  _isDirty = true;
}

void PrePassRenderer::_enableTextures(const std::vector<unsigned int>& types)
{
  for (const auto& type : types) {
    if (_textureIndices[type] == -1) {
      _textureIndices[type] = static_cast<int>(_mrtLayout.size());
      _mrtLayout.emplace_back(type);

      _mrtFormats.emplace_back(_textureFormats[type].format);
      ++mrtCount;
    }
  }
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

  const auto camera = _scene->activeCamera();
  if (!camera) {
    return;
  }

  std::vector<PostProcessPtr> postProcesses;
  copy_if(camera->_postProcesses.begin(), camera->_postProcesses.end(),
          back_inserter(postProcesses), [](const PostProcessPtr& pp) { return pp != nullptr; });

  if (!postProcesses.empty()) {
    for (const auto& postProcess : postProcesses) {
      if (postProcess->setPrePassRenderer(shared_from_this())) {
        enablePrePass = true;
      }
    }
  }

  _markAllMaterialsAsPrePassDirty();
  _isDirty = false;

  if (enablePrePass) {
    _enable();
  }

  if (!enabled()) {
    // Prepass disabled, we render only on 1 color attachment
    _engine->restoreDefaultFramebuffer();
    _engine->restoreSingleAttachment();
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
  for (const auto& effectConfiguration : _effectConfigurations) {
    if (effectConfiguration) {
      effectConfiguration->dispose();
    }
  }
  _effectConfigurations.clear();

  imageProcessingPostProcess->dispose();
  prePassRT->dispose();
}

} // end of namespace BABYLON
