#include <babylon/lights/shadows/shadow_generator.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator_scene_component.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/shadow_depth_wrapper.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/maths/vector2.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/postprocesses/blur_post_process.h>
#include <babylon/postprocesses/pass_post_process.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/post_process_manager.h>
#include <babylon/rendering/rendering_manager.h>

namespace BABYLON {

size_t ShadowGenerator::_Counter = 0;

ShadowGenerator::ShadowGenerator(int mapSize, const IShadowLightPtr& light, bool usefulFloatFirst)
    : ShadowGenerator(ISize{mapSize, mapSize}, light, usefulFloatFirst)
{
}

ShadowGenerator::ShadowGenerator(const ISize& mapSize, const IShadowLightPtr& light,
                                 bool usefulFloatFirst)
    : customShaderOptions{std::nullopt}
    , customAllowRendering{nullptr}
    , bias{this, &ShadowGenerator::get_bias, &ShadowGenerator::set_bias}
    , normalBias{this, &ShadowGenerator::get_normalBias, &ShadowGenerator::set_normalBias}
    , blurBoxOffset{this, &ShadowGenerator::get_blurBoxOffset, &ShadowGenerator::set_blurBoxOffset}
    , blurScale{this, &ShadowGenerator::get_blurScale, &ShadowGenerator::set_blurScale}
    , blurKernel{this, &ShadowGenerator::get_blurKernel, &ShadowGenerator::set_blurKernel}
    , useKernelBlur{this, &ShadowGenerator::get_useKernelBlur, &ShadowGenerator::set_useKernelBlur}
    , depthScale{this, &ShadowGenerator::get_depthScale, &ShadowGenerator::set_depthScale}
    , filter{this, &ShadowGenerator::get_filter, &ShadowGenerator::set_filter}
    , usePoissonSampling{this, &ShadowGenerator::get_usePoissonSampling,
                         &ShadowGenerator::set_usePoissonSampling}
    , useExponentialShadowMap{this, &ShadowGenerator::get_useExponentialShadowMap,
                              &ShadowGenerator::set_useExponentialShadowMap}
    , useBlurExponentialShadowMap{this, &ShadowGenerator::get_useBlurExponentialShadowMap,
                                  &ShadowGenerator::set_useBlurExponentialShadowMap}
    , useCloseExponentialShadowMap{this, &ShadowGenerator::get_useCloseExponentialShadowMap,
                                   &ShadowGenerator::set_useCloseExponentialShadowMap}
    , useBlurCloseExponentialShadowMap{this, &ShadowGenerator::get_useBlurCloseExponentialShadowMap,
                                       &ShadowGenerator::set_useBlurCloseExponentialShadowMap}
    , usePercentageCloserFiltering{this, &ShadowGenerator::get_usePercentageCloserFiltering,
                                   &ShadowGenerator::set_usePercentageCloserFiltering}
    , filteringQuality{this, &ShadowGenerator::get_filteringQuality,
                       &ShadowGenerator::set_filteringQuality}
    , useContactHardeningShadow{this, &ShadowGenerator::get_useContactHardeningShadow,
                                &ShadowGenerator::set_useContactHardeningShadow}
    , contactHardeningLightSizeUVRatio{this, &ShadowGenerator::get_contactHardeningLightSizeUVRatio,
                                       &ShadowGenerator::set_contactHardeningLightSizeUVRatio}
    , darkness{this, &ShadowGenerator::get_darkness, &ShadowGenerator::set_darkness}
    , transparencyShadow{this, &ShadowGenerator::get_transparencyShadow,
                         &ShadowGenerator::set_transparencyShadow}
    , mapSize{this, &ShadowGenerator::get_mapSize, &ShadowGenerator::set_mapSize}
    , enableSoftTransparentShadow{false}
    , frustumEdgeFalloff{0.f}
    , forceBackFacesOnly{false}
    , _bias{0.00005f}
    , _normalBias{0.f}
    , _blurBoxOffset{1}
    , _blurScale{2.f}
    , _blurKernel{1.f}
    , _useKernelBlur{false}
    , _filter{ShadowGenerator::FILTER_NONE}
    , _filteringQuality{ShadowGenerator::QUALITY_HIGH}
    , _contactHardeningLightSizeUVRatio{0.1f}
    , _darkness{0.f}
    , _transparencyShadow{false}
    , _shadowMap{nullptr}
    , _shadowMap2{nullptr}
    , _lightDirection{Vector3::Zero()}
    , _viewMatrix{Matrix::Zero()}
    , _projectionMatrix{Matrix::Zero()}
    , _transformMatrix{Matrix::Zero()}
    , _cacheInitialized{false}
    , _cachedPosition{Vector3{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max()}}
    , _cachedDirection{Vector3{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                               std::numeric_limits<float>::max()}}
    , _boxBlurPostprocess{nullptr}
    , _kernelBlurXPostprocess{nullptr}
    , _kernelBlurYPostprocess{nullptr}
    , _currentFaceIndex{0}
    , _currentFaceIndexCache{0}
    , _useFullFloat{true}
    , _textureType{0}
    , _defaultTextureMatrix{Matrix::Identity()}
    , _storedUniqueId{std::nullopt}
{
  _mapSize = RenderTargetSize{mapSize.width, mapSize.height};
  _light   = light;
  _scene   = light->getScene();
  id       = light->id;

  auto component = _scene->_getComponent(SceneComponentConstants::NAME_SHADOWGENERATOR);
  if (!component) {
    component = ShadowGeneratorSceneComponent::New(_scene);
    _scene->_addComponent(component);
  }

  // Texture type fallback from float to int if not supported.
  const auto& caps = _scene->getEngine()->getCaps();

  if (!usefulFloatFirst) {
    if (caps.textureHalfFloatRender && caps.textureHalfFloatLinearFiltering) {
      _textureType = Constants::TEXTURETYPE_HALF_FLOAT;
    }
    else if (caps.textureFloatRender && caps.textureFloatLinearFiltering) {
      _textureType = Constants::TEXTURETYPE_FLOAT;
    }
    else {
      _textureType = Constants::TEXTURETYPE_UNSIGNED_INT;
    }
  }
  else {
    if (caps.textureFloatRender && caps.textureFloatLinearFiltering) {
      _textureType = Constants::TEXTURETYPE_FLOAT;
    }
    else if (caps.textureHalfFloatRender && caps.textureHalfFloatLinearFiltering) {
      _textureType = Constants::TEXTURETYPE_HALF_FLOAT;
    }
    else {
      _textureType = Constants::TEXTURETYPE_UNSIGNED_INT;
    }
  }

  _initializeGenerator();
  _applyFilterValues();
}

ShadowGenerator::~ShadowGenerator() = default;

void ShadowGenerator::addToLight(const ShadowGeneratorPtr& shadowGenerator)
{
  _light->_shadowGenerator = shadowGenerator;
}

float ShadowGenerator::get_bias() const
{
  return _bias;
}

void ShadowGenerator::set_bias(float iBias)
{
  _bias = iBias;
}

float ShadowGenerator::get_normalBias() const
{
  return _normalBias;
}

void ShadowGenerator::set_normalBias(float iNormalBias)
{
  _normalBias = iNormalBias;
}

int ShadowGenerator::get_blurBoxOffset() const
{
  return _blurBoxOffset;
}

void ShadowGenerator::set_blurBoxOffset(int value)
{
  if (_blurBoxOffset == value) {
    return;
  }

  _blurBoxOffset = value;
  _disposeBlurPostProcesses();
}

float ShadowGenerator::get_blurScale() const
{
  return _blurScale;
}

void ShadowGenerator::set_blurScale(float value)
{
  if (stl_util::almost_equal(_blurScale, value)) {
    return;
  }

  _blurScale = value;
  _disposeBlurPostProcesses();
}

float ShadowGenerator::get_blurKernel() const
{
  return _blurKernel;
}

void ShadowGenerator::set_blurKernel(float value)
{
  if (stl_util::almost_equal(_blurKernel, value)) {
    return;
  }

  _blurKernel = value;
  _disposeBlurPostProcesses();
}

bool ShadowGenerator::get_useKernelBlur() const
{
  return _useKernelBlur;
}

void ShadowGenerator::set_useKernelBlur(bool value)
{
  if (_useKernelBlur == value) {
    return;
  }

  _useKernelBlur = value;
  _disposeBlurPostProcesses();
}

float ShadowGenerator::get_depthScale() const
{
  return _depthScale.value_or(_light->getDepthScale());
}

unsigned int ShadowGenerator::_validateFilter(unsigned int iFilter) const
{
  return iFilter;
}

void ShadowGenerator::set_depthScale(float value)
{
  _depthScale = value;
}

unsigned int ShadowGenerator::get_filter() const
{
  return _filter;
}

void ShadowGenerator::set_filter(unsigned int value)
{
  value = _validateFilter(value);

  // Blurring the cubemap is going to be too expensive. Reverting to unblurred version
  if (_light->needCube()) {
    if (value == ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP) {
      useExponentialShadowMap = true;
      return;
    }
    else if (value == ShadowGenerator::FILTER_BLURCLOSEEXPONENTIALSHADOWMAP) {
      useCloseExponentialShadowMap = true;
      return;
    }
    // PCF on cubemap would also be expensive
    else if (value == ShadowGenerator::FILTER_PCF || value == ShadowGenerator::FILTER_PCSS) {
      usePoissonSampling = true;
      return;
    }
  }

  // WebGL fallback for PCF.
  if (value == ShadowGenerator::FILTER_PCF || value == ShadowGenerator::FILTER_PCSS) {
    if (!_scene->getEngine()->_features.supportShadowSamplers) {
      usePoissonSampling = true;
      return;
    }
  }

  if (_filter == value) {
    return;
  }

  _filter = value;
  _disposeBlurPostProcesses();
  _applyFilterValues();
  _light->_markMeshesAsLightDirty();
}

bool ShadowGenerator::get_usePoissonSampling() const
{
  return filter() == ShadowGenerator::FILTER_POISSONSAMPLING;
}

void ShadowGenerator::set_usePoissonSampling(bool value)
{
  const auto iFilter = _validateFilter(ShadowGenerator::FILTER_POISSONSAMPLING);

  if (!value && filter() != ShadowGenerator::FILTER_POISSONSAMPLING) {
    return;
  }

  filter = value ? iFilter : ShadowGenerator::FILTER_NONE;
}

bool ShadowGenerator::get_useExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_EXPONENTIALSHADOWMAP;
}

void ShadowGenerator::set_useExponentialShadowMap(bool value)
{
  const auto iFilter = _validateFilter(ShadowGenerator::FILTER_EXPONENTIALSHADOWMAP);

  if (!value && filter() != ShadowGenerator::FILTER_EXPONENTIALSHADOWMAP) {
    return;
  }

  filter = value ? iFilter : ShadowGenerator::FILTER_NONE;
}

bool ShadowGenerator::get_useBlurExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP;
}

void ShadowGenerator::set_useBlurExponentialShadowMap(bool value)
{
  const auto iFilter = _validateFilter(ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP);

  if (!value && filter() != ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP) {
    return;
  }

  filter = value ? iFilter : ShadowGenerator::FILTER_NONE;
}

bool ShadowGenerator::get_useCloseExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_CLOSEEXPONENTIALSHADOWMAP;
}

void ShadowGenerator::set_useCloseExponentialShadowMap(bool value)
{
  const auto iFilter = _validateFilter(ShadowGenerator::FILTER_CLOSEEXPONENTIALSHADOWMAP);

  if (!value && filter() != ShadowGenerator::FILTER_CLOSEEXPONENTIALSHADOWMAP) {
    return;
  }

  filter = value ? iFilter : ShadowGenerator::FILTER_NONE;
}

bool ShadowGenerator::get_useBlurCloseExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_BLURCLOSEEXPONENTIALSHADOWMAP;
}

void ShadowGenerator::set_useBlurCloseExponentialShadowMap(bool value)
{
  const auto iFilter = _validateFilter(ShadowGenerator::FILTER_BLURCLOSEEXPONENTIALSHADOWMAP);

  if (!value && filter() != ShadowGenerator::FILTER_BLURCLOSEEXPONENTIALSHADOWMAP) {
    return;
  }

  filter = value ? iFilter : ShadowGenerator::FILTER_NONE;
}

bool ShadowGenerator::get_usePercentageCloserFiltering() const
{
  return filter() == ShadowGenerator::FILTER_PCF;
}

void ShadowGenerator::set_usePercentageCloserFiltering(bool value)
{
  const auto iFilter = _validateFilter(ShadowGenerator::FILTER_PCF);

  if (!value && filter() != ShadowGenerator::FILTER_PCF) {
    return;
  }

  filter = value ? iFilter : ShadowGenerator::FILTER_NONE;
}

unsigned int ShadowGenerator::get_filteringQuality() const
{
  return _filteringQuality;
}

void ShadowGenerator::set_filteringQuality(unsigned int iFilteringQuality)
{
  if (_filteringQuality == iFilteringQuality) {
    return;
  }

  _filteringQuality = iFilteringQuality;

  _disposeBlurPostProcesses();
  _applyFilterValues();
  _light->_markMeshesAsLightDirty();
}

bool ShadowGenerator::get_useContactHardeningShadow() const
{
  return filter() == ShadowGenerator::FILTER_PCSS;
}

void ShadowGenerator::set_useContactHardeningShadow(bool value)
{
  const auto iFilter = _validateFilter(ShadowGenerator::FILTER_PCSS);

  if (!value && filter() != ShadowGenerator::FILTER_PCSS) {
    return;
  }

  filter = value ? iFilter : ShadowGenerator::FILTER_NONE;
}

float ShadowGenerator::get_contactHardeningLightSizeUVRatio() const
{
  return _contactHardeningLightSizeUVRatio;
}

void ShadowGenerator::set_contactHardeningLightSizeUVRatio(float iContactHardeningLightSizeUVRatio)
{
  _contactHardeningLightSizeUVRatio = iContactHardeningLightSizeUVRatio;
}

float ShadowGenerator::get_darkness() const
{
  return _darkness;
}

void ShadowGenerator::set_darkness(float value)
{
  setDarkness(value);
}

float ShadowGenerator::getDarkness() const
{
  return _darkness;
}

bool ShadowGenerator::get_transparencyShadow() const
{
  return _transparencyShadow;
}

void ShadowGenerator::set_transparencyShadow(bool value)
{
  setTransparencyShadow(value);
}

RenderTargetSize& ShadowGenerator::get_mapSize()
{
  return _mapSize;
}

void ShadowGenerator::set_mapSize(const RenderTargetSize& size)
{
  _mapSize = size;
  _light->_markMeshesAsLightDirty();
  recreateShadowMap();
}

ShadowGenerator& ShadowGenerator::setDarkness(float iDarkness)
{
  if (iDarkness >= 1.f) {
    _darkness = 1.f;
  }
  else if (iDarkness <= 0.f) {
    _darkness = 0.f;
  }
  else {
    _darkness = iDarkness;
  }
  return *this;
}

ShadowGenerator& ShadowGenerator::setTransparencyShadow(bool transparent)
{
  _transparencyShadow = transparent;
  return *this;
}

RenderTargetTexturePtr ShadowGenerator::getShadowMap()
{
  return _shadowMap;
}

RenderTargetTexturePtr ShadowGenerator::getShadowMapForRendering()
{
  if (_shadowMap2) {
    return _shadowMap2;
  }

  return _shadowMap;
}

std::string ShadowGenerator::getClassName() const
{
  return ShadowGenerator::CLASSNAME;
}

ShadowGenerator& ShadowGenerator::addShadowCaster(const AbstractMeshPtr& mesh,
                                                  bool includeDescendants)
{
  if (!_shadowMap) {
    return *this;
  }

  _shadowMap->renderList().emplace_back(mesh.get());

  if (includeDescendants) {
    stl_util::concat(_shadowMap->renderList(),
                     stl_util::to_raw_ptr_vector(mesh->getChildMeshes(false)));
  }

  return *this;
}

ShadowGenerator& ShadowGenerator::removeShadowCaster(const AbstractMeshPtr& mesh,
                                                     bool includeDescendants)
{
  if (!_shadowMap || !_shadowMap->renderList().empty()) {
    return *this;
  }

  _shadowMap->renderList().erase(
    std::remove(_shadowMap->renderList().begin(), _shadowMap->renderList().end(), mesh.get()),
    _shadowMap->renderList().end());

  if (includeDescendants) {
    for (auto& child : mesh->getChildren()) {
      if (auto childMesh = std::static_pointer_cast<AbstractMesh>(child)) {
        removeShadowCaster(childMesh);
      }
    }
  }

  return *this;
}

IShadowLightPtr& ShadowGenerator::getLight()
{
  return _light;
}

void ShadowGenerator::_initializeGenerator()
{
  _light->_markMeshesAsLightDirty();
  _initializeShadowMap();
}

void ShadowGenerator::_createTargetRenderTexture()
{
  if (_scene->getEngine()->_features.supportDepthStencilTexture) {
    _shadowMap = RenderTargetTexture::New(_light->name + "_shadowMap", _mapSize, _scene, false,
                                          true, _textureType, _light->needCube(),
                                          TextureConstants::TRILINEAR_SAMPLINGMODE, false, false);
    _shadowMap->createDepthStencilTexture(Constants::LESS, true);
  }
  else {
    _shadowMap = RenderTargetTexture::New(_light->name + "_shadowMap", _mapSize, _scene, false,
                                          true, _textureType, _light->needCube());
  }
}

void ShadowGenerator::_initializeShadowMap()
{
  _createTargetRenderTexture();

  if (_shadowMap == nullptr) {
    return;
  }

  _shadowMap->wrapU                     = TextureConstants::CLAMP_ADDRESSMODE;
  _shadowMap->wrapV                     = TextureConstants::CLAMP_ADDRESSMODE;
  _shadowMap->anisotropicFilteringLevel = 1;
  _shadowMap->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  _shadowMap->renderParticles      = false;
  _shadowMap->ignoreCameraViewport = true;
  if (_storedUniqueId.has_value()) {
    _shadowMap->uniqueId = *_storedUniqueId;
  }

  // Custom render function.
  _shadowMap->customRenderFunction = [this](const std::vector<SubMesh*>& opaqueSubMeshes,
                                            const std::vector<SubMesh*>& alphaTestSubMeshes,
                                            const std::vector<SubMesh*>& transparentSubMeshes,
                                            const std::vector<SubMesh*>& depthOnlySubMeshes,
                                            const std::function<void()>& /*beforeTransparents*/) {
    _renderForShadowMap(opaqueSubMeshes, transparentSubMeshes, alphaTestSubMeshes,
                        depthOnlySubMeshes);
  };

  // Force the mesh is ready function to true as we are double checking it
  // in the custom render function. Also it prevents side effects and useless
  // shader variations in DEPTHPREPASS mode.
  _shadowMap->customIsReadyFunction = [](AbstractMesh* /*m*/, int /*r*/) -> bool { return true; };

  _shadowMap->onBeforeBindObservable.add([this](RenderTargetTexture* /*rt*/, EventState&) -> void {
    _scene->getEngine()->_debugPushGroup(
      StringTools::printf("shadow map generation for %s", _nameForCustomEffect.c_str()), 1);
  });

  // Record Face Index before render.
  _shadowMap->onBeforeRenderObservable.add([this](const int* faceIndex, EventState&) {
    _currentFaceIndex = static_cast<unsigned int>(*faceIndex);
    if (_filter == ShadowGenerator::FILTER_PCF) {
      _scene->getEngine()->setColorWrite(false);
    }
    getTransformMatrix(); // generate the view/projection matrix
    _scene->setTransformMatrix(_viewMatrix, _projectionMatrix);
  });

  // Blur if required after render.
  _shadowMap->onAfterUnbindObservable.add([this](RenderTargetTexture*, EventState&) {
    const auto engine = _scene->getEngine();
    _scene->updateTransformMatrix(); // restore the view/projection matrices of the active camera

    if (_filter == ShadowGenerator::FILTER_PCF) {
      engine->setColorWrite(true);
    }
    if (!useBlurExponentialShadowMap() && !useBlurCloseExponentialShadowMap()) {
      engine->_debugPopGroup(1);
      return;
    }
    auto shadowMap = getShadowMapForRendering();

    if (shadowMap) {
      const auto texture = shadowMap->getInternalTexture();
      _scene->postProcessManager->directRender(_blurPostProcesses, texture, true);
      engine->unBindFramebuffer(texture, true);
      engine->_debugPopGroup(1);
    }
  });

  // Clear according to the chosen filter.
  _shadowMap->onClearObservable.add([this](Engine* engine, EventState&) {
    Color4 clearZero{0.f, 0.f, 0.f, 0.f};
    Color4 clearOne{1.f, 1.f, 1.f, 1.f};
    if (_filter == ShadowGenerator::FILTER_PCF) {
      engine->clear(clearOne, false, true, false);
    }
    else if (useExponentialShadowMap() || useBlurExponentialShadowMap()) {
      engine->clear(clearZero, true, true, false);
    }
    else {
      engine->clear(clearOne, true, true, false);
    }
  });

  // Recreate on resize.
  _shadowMap->onResizeObservable.add([this](RenderTargetTexture* RTT, EventState&) -> void {
    _storedUniqueId = _shadowMap->uniqueId;
    _mapSize        = RTT->getRenderSize();
    _light->_markMeshesAsLightDirty();
    recreateShadowMap();
  });

  // Ensures rendering groupids do not erase the depth buffer or we would lose the shadows
  // information.
  for (auto i = RenderingManager::MIN_RENDERINGGROUPS; i < RenderingManager::MAX_RENDERINGGROUPS;
       ++i) {
    _shadowMap->setRenderingAutoClearDepthStencil(i, false);
  }
}

void ShadowGenerator::_initializeBlurRTTAndPostProcesses()
{
  auto engine     = _scene->getEngine();
  auto targetSize = static_cast<int>(_mapSize.width / blurScale());

  if (!useKernelBlur() || blurScale() != 1.f) {
    _shadowMap2 = RenderTargetTexture::New(
      _light->name + "_shadowMap2", RenderTargetSize{targetSize, targetSize}, _scene, false, true,
      _textureType, false, TextureConstants::TRILINEAR_SAMPLINGMODE, false);
    _shadowMap2->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
    _shadowMap2->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
    _shadowMap2->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  }

  if (useKernelBlur()) {
    _kernelBlurXPostprocess = BlurPostProcess::New(
      _light->name + "KernelBlurX", Vector2(1.f, 0.f), blurKernel(), 1.f, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, engine, false, _textureType);
    _kernelBlurXPostprocess->width  = targetSize;
    _kernelBlurXPostprocess->height = targetSize;
    _kernelBlurXPostprocess->onApplyObservable.add(
      [this](Effect* effect, EventState&) { effect->setTexture("textureSampler", _shadowMap); });

    _kernelBlurYPostprocess = BlurPostProcess::New(
      _light->name + "KernelBlurY", Vector2(0.f, 1.f), blurKernel(), 1.f, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, engine, false, _textureType);

    _kernelBlurXPostprocess->autoClear = false;
    _kernelBlurYPostprocess->autoClear = false;

    if (_textureType == Constants::TEXTURETYPE_UNSIGNED_INT) {
      std::dynamic_pointer_cast<BlurPostProcess>(_kernelBlurXPostprocess)->packedFloat = true;
      std::dynamic_pointer_cast<BlurPostProcess>(_kernelBlurYPostprocess)->packedFloat = true;
    }

    _blurPostProcesses = {_kernelBlurXPostprocess, _kernelBlurYPostprocess};
  }
  else {
    _boxBlurPostprocess
      = PostProcess::New(_light->name + "DepthBoxBlur", "depthBoxBlur", {"screenSize", "boxOffset"},
                         {}, 1.f, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, engine, false,
                         "#define OFFSET " + std::to_string(_blurBoxOffset), _textureType);
    _boxBlurPostprocess->onApplyObservable.add([this](Effect* effect, EventState& /*es*/) {
      auto targetSize = static_cast<float>(_mapSize.width) / blurScale();
      effect->setFloat2("screenSize", targetSize, targetSize);
      effect->setTexture("textureSampler", _shadowMap);
    });

    _boxBlurPostprocess->autoClear = false;

    _blurPostProcesses = {_boxBlurPostprocess};
  }
}

void ShadowGenerator::_renderForShadowMap(const std::vector<SubMesh*>& opaqueSubMeshes,
                                          const std::vector<SubMesh*>& alphaTestSubMeshes,
                                          const std::vector<SubMesh*>& transparentSubMeshes,
                                          const std::vector<SubMesh*>& depthOnlySubMeshes)
{
  auto engine = _scene->getEngine();

  const auto colorWrite = engine->getColorWrite();
  if (!depthOnlySubMeshes.empty()) {
    engine->setColorWrite(false);
    for (const auto& depthOnlySubMesh : depthOnlySubMeshes) {
      _renderSubMeshForShadowMap(depthOnlySubMesh);
    }
    engine->setColorWrite(colorWrite);
  }

  for (const auto& opaqueSubMesh : opaqueSubMeshes) {
    _renderSubMeshForShadowMap(opaqueSubMesh);
  }

  for (const auto& alphaTestSubMesh : alphaTestSubMeshes) {
    _renderSubMeshForShadowMap(alphaTestSubMesh);
  }

  if (_transparencyShadow) {
    for (const auto& transparentSubMesh : transparentSubMeshes) {
      _renderSubMeshForShadowMap(transparentSubMesh, true);
    }
  }
  else {
    for (const auto& transparentSubMesh : transparentSubMeshes) {
      transparentSubMesh->getEffectiveMesh()->_internalAbstractMeshDataInfo._isActiveIntermediate
        = false;
    }
  }
}

void ShadowGenerator::_bindCustomEffectForRenderSubMeshForShadowMap(
  SubMesh* /*subMesh*/, Effect* effect,
  const std::unordered_map<std::string, std::string>& matriceNames, AbstractMesh* mesh)
{
  effect->setMatrix(stl_util::contains(matriceNames, "viewProjection") ?
                      matriceNames.at("viewProjection") :
                      "viewProjection",
                    getTransformMatrix());

  effect->setMatrix(stl_util::contains(matriceNames, "view") ? matriceNames.at("view") : "view",
                    _viewMatrix);

  effect->setMatrix(stl_util::contains(matriceNames, "projection") ? matriceNames.at("projection") :
                                                                     "projection",
                    _projectionMatrix);

  auto world = mesh->getWorldMatrix();

  effect->setMatrix(stl_util::contains(matriceNames, "world") ? matriceNames.at("world") : "world",
                    world);

  world.multiplyToRef(getTransformMatrix(), tmpMatrix);

  effect->setMatrix(stl_util::contains(matriceNames, "worldViewProjection") ?
                      matriceNames.at("worldViewProjection") :
                      "worldViewProjection",
                    tmpMatrix);

  world.multiplyToRef(_viewMatrix, tmpMatrix2);

  effect->setMatrix(stl_util::contains(matriceNames, "worldView") ? matriceNames.at("worldView") :
                                                                    "worldView",
                    tmpMatrix2);
}

void ShadowGenerator::_renderSubMeshForShadowMap(SubMesh* subMesh, bool isTransparent)
{
  auto renderingMesh = subMesh->getRenderingMesh();
  auto effectiveMesh = subMesh->getEffectiveMesh();
  auto scene         = _scene;
  auto engine        = scene->getEngine();
  auto material      = subMesh->getMaterial();

  effectiveMesh->_internalAbstractMeshDataInfo._isActiveIntermediate = false;

  if (!material || subMesh->verticesCount == 0 || subMesh->_renderId == scene->getRenderId()) {
    return;
  }

  // Culling
  engine->setState(material->backFaceCulling());

  // Managing instances
  auto batch = renderingMesh->_getInstancesRenderList(subMesh->_id,
                                                      subMesh->getReplacementMesh() != nullptr);
  if (batch->mustReturn) {
    return;
  }

  auto hardwareInstancedRendering
    = (engine->getCaps().instancedArrays)
      && (stl_util::contains(batch->visibleInstances, subMesh->_id))
      && (!batch->visibleInstances[subMesh->_id].empty() || renderingMesh->hasThinInstances());
  if (customAllowRendering && !customAllowRendering(subMesh)) {
    return;
  }

  if (isReady(subMesh, hardwareInstancedRendering, isTransparent)) {
    subMesh->_renderId = scene->getRenderId();

    const auto shadowDepthWrapper = material->shadowDepthWrapper;

    const auto iEffect = (shadowDepthWrapper && shadowDepthWrapper->getEffect(subMesh, this)) ?
                           subMesh->_getCustomEffect(_nameForCustomEffect, false)->effect :
                           nullptr;
    if (!iEffect) {
      return;
    }

    engine->enableEffect(iEffect);

    renderingMesh->_bind(subMesh, iEffect, material->fillMode());

    getTransformMatrix(); // make sure _cachedDirection et _cachedPosition are up to date

    iEffect->setFloat3("biasAndScaleSM", bias(), normalBias(), depthScale());

    if (getLight()->getTypeID() == Light::LIGHTTYPEID_DIRECTIONALLIGHT) {
      iEffect->setVector3("lightDataSM", _cachedDirection);
    }
    else {
      iEffect->setVector3("lightDataSM", _cachedPosition);
    }

    if (scene->activeCamera()) {
      iEffect->setFloat2("depthValuesSM", getLight()->getDepthMinZ(*scene->activeCamera()),
                         getLight()->getDepthMinZ(*scene->activeCamera())
                           + getLight()->getDepthMaxZ(*scene->activeCamera()));
    }

    if (isTransparent && enableSoftTransparentShadow) {
      iEffect->setFloat("softTransparentShadowSM", effectiveMesh->visibility() * material->alpha());
    }

    if (shadowDepthWrapper) {
      subMesh->_effectOverride = iEffect;
      if (shadowDepthWrapper->standalone()) {
        shadowDepthWrapper->baseMaterial()->bindForSubMesh(effectiveMesh->getWorldMatrix(),
                                                           renderingMesh.get(), subMesh);
      }
      else {
        material->bindForSubMesh(effectiveMesh->getWorldMatrix(), renderingMesh.get(), subMesh);
      }
      subMesh->_effectOverride = nullptr;
    }
    else {
      iEffect->setMatrix("viewProjection", getTransformMatrix());
      // Alpha test
      if (material && material->needAlphaTesting()) {
        auto alphaTexture = material->getAlphaTestTexture();
        if (alphaTexture) {
          iEffect->setTexture("diffuseSampler", alphaTexture);
          iEffect->setMatrix("diffuseMatrix", alphaTexture->getTextureMatrix() ?
                                                *alphaTexture->getTextureMatrix() :
                                                _defaultTextureMatrix);
        }
      }

      // Bones
      if (renderingMesh->useBones() && renderingMesh->computeBonesUsingShaders()
          && renderingMesh->skeleton()) {
        const auto& skeleton = renderingMesh->skeleton();

        if (skeleton->isUsingTextureForMatrices) {
          const auto& boneTexture = skeleton->getTransformMatrixTexture(renderingMesh.get());

          if (!boneTexture) {
            return;
          }

          iEffect->setTexture("boneSampler", boneTexture);
          iEffect->setFloat("boneTextureWidth", 4.f * (skeleton->bones.size() + 1));
        }
        else {
          iEffect->setMatrices("mBones", skeleton->getTransformMatrices((renderingMesh.get())));
        }
      }

      // Morph targets
      MaterialHelper::BindMorphTargetParameters(renderingMesh.get(), iEffect.get());

      if (renderingMesh->morphTargetManager()
          && renderingMesh->morphTargetManager()->isUsingTextureForTargets()) {
        renderingMesh->morphTargetManager()->_bind(iEffect);
      }

      // Clip planes
      MaterialHelper::BindClipPlane(iEffect, scene);
    }

    _bindCustomEffectForRenderSubMeshForShadowMap(subMesh, iEffect.get(),
                                                  shadowDepthWrapper ?
                                                    shadowDepthWrapper->_matriceNames :
                                                    std::unordered_map<std::string, std::string>{},
                                                  effectiveMesh.get());

    if (forceBackFacesOnly) {
      engine->setState(true, 0, false, true);
    }

    // Observables
    onBeforeShadowMapRenderMeshObservable.notifyObservers(renderingMesh.get());
    onBeforeShadowMapRenderObservable.notifyObservers(iEffect.get());

    // Draw
    renderingMesh->_processRendering(
      effectiveMesh.get(), subMesh, iEffect, static_cast<int>(material->fillMode()), batch,
      hardwareInstancedRendering,
      [&](bool /*isInstance*/, const Matrix& world, Material* /*effectiveMaterial*/) {
        iEffect->setMatrix("world", world);
      });

    if (forceBackFacesOnly) {
      engine->setState(true, 0, false, false);
    }

    // Observables
    onAfterShadowMapRenderObservable.notifyObservers(iEffect.get());
    onAfterShadowMapRenderMeshObservable.notifyObservers(renderingMesh.get());
  }
  else {
    // Need to reset refresh rate of the shadowMap
    if (_shadowMap) {
      _shadowMap->resetRefreshCounter();
    }
  }
}

void ShadowGenerator::_applyFilterValues()
{
  if (!_shadowMap) {
    return;
  }

  if (filter() == ShadowGenerator::FILTER_NONE || filter() == ShadowGenerator::FILTER_PCSS) {
    _shadowMap->updateSamplingMode(TextureConstants::NEAREST_SAMPLINGMODE);
  }
  else {
    _shadowMap->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  }
}

void ShadowGenerator::forceCompilationSync(
  const std::function<void(IShadowGenerator* generator)>& onCompiled,
  const ShadowGeneratorCompileOptions& options)
{
  auto shadowMap = getShadowMap();
  if (!shadowMap) {
    if (onCompiled) {
      onCompiled(this);
    }
    return;
  }

  auto& renderList = shadowMap->renderList;
  if (renderList().empty()) {
    if (onCompiled) {
      onCompiled(this);
    }
    return;
  }

  std::vector<SubMesh*> subMeshes;
  for (auto& mesh : renderList()) {
    stl_util::concat(subMeshes, stl_util::to_raw_ptr_vector(mesh->subMeshes));
  }
  if (subMeshes.empty()) {
    if (onCompiled) {
      onCompiled(this);
    }
    return;
  }

  std::size_t currentIndex = 0;

  const auto checkReady = [&]() {
    if (!_scene || !_scene->getEngine()) {
      return;
    }

    while (isReady(subMeshes[currentIndex], options.useInstances,
                   subMeshes[currentIndex]->getMaterial()->needAlphaBlendingForMesh(
                     *subMeshes[currentIndex]->getMesh()) ?
                     subMeshes[currentIndex]->getMaterial()->needAlphaBlendingForMesh(
                       *subMeshes[currentIndex]->getMesh()) :
                     false)) {
      ++currentIndex;
      if (currentIndex >= subMeshes.size()) {
        if (onCompiled) {
          onCompiled(this);
        }
        return;
      }
    }
  };

  checkReady();
}

void ShadowGenerator::_isReadyCustomDefines(std::vector<std::string>& /*defines*/,
                                            SubMesh* /*subMesh*/, bool /*useInstances*/)
{
}

std::vector<std::string>& ShadowGenerator::_prepareShadowDefines(SubMesh* subMesh,
                                                                 bool useInstances,
                                                                 std::vector<std::string>& defines,
                                                                 bool isTransparent)
{
  defines.emplace_back(StringTools::printf(
    "#define SM_FLOAT %s", (_textureType != Constants::TEXTURETYPE_UNSIGNED_INT ? "1" : "0")));

  defines.emplace_back(StringTools::printf(
    "#define SM_ESM %s", (useExponentialShadowMap || useBlurExponentialShadowMap ? "1" : "0")));

  defines.emplace_back(
    StringTools::printf("#define SM_DEPTHTEXTURE %s",
                        (usePercentageCloserFiltering || useContactHardeningShadow ? "1" : "0")));

  const auto mesh = subMesh->getMesh();

  // Normal bias.
  defines.emplace_back(StringTools::printf(
    "#define SM_NORMALBIAS %s",
    (normalBias && mesh->isVerticesDataPresent(VertexBuffer::NormalKind) ? "1" : "0")));
  defines.emplace_back(StringTools::printf(
    "#define SM_DIRECTIONINLIGHTDATA %s",
    (getLight()->getTypeID() == Light::LIGHTTYPEID_DIRECTIONALLIGHT ? "1" : "0")));

  // Point light
  defines.emplace_back(
    StringTools::printf("#define SM_USEDISTANCE %s", (_light->needCube() ? "1" : "0")));

  // Soft transparent shadows
  defines.emplace_back(
    StringTools::printf("#define SM_SOFTTRANSPARENTSHADOW %s",
                        (enableSoftTransparentShadow && isTransparent ? "1" : "0")));

  _isReadyCustomDefines(defines, subMesh, useInstances);

  return defines;
}

bool ShadowGenerator::isReady(SubMesh* subMesh, bool useInstances, bool isTransparent)
{
  const auto material           = subMesh->getMaterial();
  const auto shadowDepthWrapper = material->shadowDepthWrapper;

  std::vector<std::string> defines;

  _prepareShadowDefines(subMesh, useInstances, defines, isTransparent);

  auto subMeshEffect = subMesh->_getCustomEffect(_nameForCustomEffect);

  auto effect        = subMeshEffect->effect;
  auto cachedDefines = subMeshEffect->defines;

  if (shadowDepthWrapper) {
    if (!shadowDepthWrapper->isReadyForSubMesh(subMesh, defines, this, useInstances)) {
      return false;
    }
  }
  else {
    std::vector<std::string> attribs{VertexBuffer::PositionKind};

    auto mesh = subMesh->getMesh();

    // Normal bias.
    if (normalBias() > 0.f && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      attribs.emplace_back(VertexBuffer::NormalKind);
      defines.emplace_back("#define NORMAL");
      if (mesh->nonUniformScaling()) {
        defines.emplace_back("#define NONUNIFORMSCALING");
      }
    }

    // Alpha test
    if (material && material->needAlphaTesting()) {
      auto alphaTexture = material->getAlphaTestTexture();
      if (alphaTexture) {
        if (!alphaTexture->isReady()) {
          return false;
        }
        defines.emplace_back("#define ALPHATEST");
        if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
          attribs.emplace_back(VertexBuffer::UVKind);
          defines.emplace_back("#define UV1");
        }
        if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
          if (alphaTexture->coordinatesIndex == 1) {
            attribs.emplace_back(VertexBuffer::UV2Kind);
            defines.emplace_back("#define UV2");
          }
        }
      }
    }

    // Bones
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (mesh->useBones() && mesh->computeBonesUsingShaders() && mesh->skeleton()) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesKind);
      attribs.emplace_back(VertexBuffer::MatricesWeightsKind);
      if (mesh->numBoneInfluencers() > 4) {
        attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKind);
        attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKind);
      }
      const auto skeleton = mesh->skeleton();
      defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                           + std::to_string(mesh->numBoneInfluencers()));
      if (mesh->numBoneInfluencers > 0) {
        fallbacks->addCPUSkinningFallback(0, mesh.get());
      }

      if (skeleton->isUsingTextureForMatrices()) {
        defines.emplace_back("#define BONETEXTURE");
      }
      else {
        defines.emplace_back(StringTools::concat(
          "#define BonesPerMesh " + std::to_string(mesh->skeleton()->bones.size() + 1)));
      }
    }
    else {
      defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
    }

    // Morph targets
    auto manager          = (std::static_pointer_cast<Mesh>(mesh))->morphTargetManager();
    auto morphInfluencers = 0u;
    if (manager) {
      if (manager->numInfluencers() > 0) {
        defines.emplace_back("#define MORPHTARGETS");
        morphInfluencers = static_cast<unsigned int>(manager->numInfluencers());
        defines.emplace_back("#define NUM_MORPH_INFLUENCERS " + std::to_string(morphInfluencers));
        MaterialDefines iDefines;
        iDefines.intDef["NUM_MORPH_INFLUENCERS"] = morphInfluencers;
        if (manager->isUsingTextureForTargets()) {
          defines.emplace_back("#define MORPHTARGETS_TEXTURE");
        }
        MaterialHelper::PrepareAttributesForMorphTargetsInfluencers(attribs, mesh.get(),
                                                                    morphInfluencers);
      }
    }

    // ClipPlanes
    const auto scene = _scene;
    if (scene->clipPlane) {
      defines.emplace_back("#define CLIPPLANE");
    }
    if (scene->clipPlane2) {
      defines.emplace_back("#define CLIPPLANE2");
    }
    if (scene->clipPlane3) {
      defines.emplace_back("#define CLIPPLANE3");
    }
    if (scene->clipPlane4) {
      defines.emplace_back("#define CLIPPLANE4");
    }
    if (scene->clipPlane5) {
      defines.emplace_back("#define CLIPPLANE5");
    }
    if (scene->clipPlane6) {
      defines.emplace_back("#define CLIPPLANE6");
    }

    // Instances
    if (useInstances) {
      defines.emplace_back("#define INSTANCES");
      MaterialHelper::PushAttributesForInstances(attribs);
      if (subMesh->getRenderingMesh()->hasThinInstances()) {
        defines.emplace_back("#define THIN_INSTANCES");
      }
    }

    if (customShaderOptions) {
      if (!customShaderOptions->defines.empty()) {
        for (const auto& define : customShaderOptions->defines) {
          if (!stl_util::contains(defines, define)) {
            defines.emplace_back(define);
          }
        }
      }
    }

    // Get correct effect
    auto join = StringTools::join(defines, '\n');
    if (cachedDefines != join) {
      cachedDefines = join;

      std::string shaderName = "shadowMap";
      std::vector<std::string> uniforms{"world",
                                        "mBones",
                                        "viewProjection",
                                        "diffuseMatrix",
                                        "lightDataSM",
                                        "depthValuesSM",
                                        "biasAndScaleSM",
                                        "morphTargetInfluences",
                                        "boneTextureWidth",
                                        "vClipPlane",
                                        "vClipPlane2",
                                        "vClipPlane3",
                                        "vClipPlane4",
                                        "vClipPlane5",
                                        "vClipPlane6",
                                        "softTransparentShadowSM",
                                        "morphTargetTextureInfo",
                                        "morphTargetTextureIndices"};
      std::vector<std::string> samplers{"diffuseSampler", "boneSampler", "morphTargets"};

      // Custom shader?
      if (customShaderOptions) {
        shaderName = customShaderOptions->shaderName;

        if (!customShaderOptions->attributes.empty()) {
          for (const auto& attrib : customShaderOptions->attributes) {
            if (!stl_util::contains(attribs, attrib)) {
              attribs.emplace_back(attrib);
            }
          }
        }

        if (!customShaderOptions->uniforms.empty()) {
          for (const auto& uniform : customShaderOptions->uniforms) {
            if (!stl_util::contains(uniforms, uniform)) {
              uniforms.emplace_back(uniform);
            }
          }
        }

        if (!customShaderOptions->samplers.empty()) {
          for (const auto& sampler : customShaderOptions->samplers) {
            if (!stl_util::contains(samplers, sampler)) {
              samplers.emplace_back(sampler);
            }
          }
        }
      }

      const auto engine = _scene->getEngine();

      IEffectCreationOptions options;
      options.attributes      = std::move(attribs);
      options.uniformsNames   = std::move(uniforms);
      options.samplers        = std::move(samplers);
      options.defines         = std::move(join);
      options.fallbacks       = std::move(fallbacks);
      options.onCompiled      = nullptr;
      options.onError         = nullptr;
      options.indexParameters = {{"maxSimultaneousMorphTargets", morphInfluencers}};

      effect = engine->createEffect(shaderName, options, _scene->getEngine());

      subMeshEffect->effect  = effect;
      subMeshEffect->defines = cachedDefines;
    }

    if (!effect->isReady()) {
      return false;
    }
  }

  if (useBlurExponentialShadowMap() || useBlurCloseExponentialShadowMap()) {
    if (_blurPostProcesses.empty()) {
      _initializeBlurRTTAndPostProcesses();
    }
  }

  if (_kernelBlurXPostprocess && !_kernelBlurXPostprocess->isReady()) {
    return false;
  }
  if (_kernelBlurYPostprocess && !_kernelBlurYPostprocess->isReady()) {
    return false;
  }
  if (_boxBlurPostprocess && !_boxBlurPostprocess->isReady()) {
    return false;
  }

  return true;
}

void ShadowGenerator::prepareDefines(MaterialDefines& defines, unsigned int lightIndex)
{
  const auto scene = _scene;
  const auto light = _light;

  if (!scene->shadowsEnabled() || !light->shadowEnabled) {
    return;
  }

  const auto lightIndexStr = std::to_string(lightIndex);

  defines.boolDef["SHADOW" + lightIndexStr] = true;

  if (useContactHardeningShadow()) {
    defines.boolDef["SHADOWPCSS" + lightIndexStr] = true;
    if (_filteringQuality == ShadowGenerator::QUALITY_LOW) {
      defines.boolDef["SHADOWLOWQUALITY" + lightIndexStr] = true;
    }
    else if (_filteringQuality == ShadowGenerator::QUALITY_MEDIUM) {
      defines.boolDef["SHADOWMEDIUMQUALITY" + lightIndexStr] = true;
    }
    // else default to high.
  }
  else if (usePercentageCloserFiltering()) {
    defines.boolDef["SHADOWPCF" + lightIndexStr] = true;
    if (_filteringQuality == ShadowGenerator::QUALITY_LOW) {
      defines.boolDef["SHADOWLOWQUALITY" + lightIndexStr] = true;
    }
    else if (_filteringQuality == ShadowGenerator::QUALITY_MEDIUM) {
      defines.boolDef["SHADOWMEDIUMQUALITY" + lightIndexStr] = true;
    }
    // else default to high.
  }
  else if (usePoissonSampling()) {
    defines.boolDef["SHADOWPOISSON" + lightIndexStr] = true;
  }
  else if (useExponentialShadowMap() || useBlurExponentialShadowMap()) {
    defines.boolDef["SHADOWESM" + lightIndexStr] = true;
  }
  else if (useCloseExponentialShadowMap() || useBlurCloseExponentialShadowMap()) {
    defines.boolDef["SHADOWCLOSEESM" + lightIndexStr] = true;
  }

  if (light->needCube()) {
    defines.boolDef["SHADOWCUBE" + lightIndexStr] = true;
  }
}

void ShadowGenerator::bindShadowLight(const std::string& lightIndex, Effect* effect)
{
  auto& scene = _scene;
  auto& light = _light;

  if (!scene->shadowsEnabled() || !light->shadowEnabled) {
    return;
  }

  auto& camera = scene->activeCamera();
  if (!camera) {
    return;
  }

  auto shadowMap = getShadowMap();

  if (!shadowMap) {
    return;
  }

  if (!light->needCube()) {
    effect->setMatrix("lightMatrix" + lightIndex, getTransformMatrix());
  }

  // Only PCF uses depth stencil texture.
  if (_filter == ShadowGenerator::FILTER_PCF) {
    effect->setDepthStencilTexture("shadowSampler" + lightIndex, getShadowMapForRendering());
    light->_uniformBuffer->updateFloat4(
      "shadowsInfo", getDarkness(), static_cast<float>(shadowMap->getSize().width),
      1.f / shadowMap->getSize().width, frustumEdgeFalloff, lightIndex);
  }
  else if (_filter == ShadowGenerator::FILTER_PCSS) {
    effect->setDepthStencilTexture("shadowSampler" + lightIndex, getShadowMapForRendering());
    effect->setTexture("depthSampler" + lightIndex, getShadowMapForRendering());
    light->_uniformBuffer->updateFloat4(
      "shadowsInfo", getDarkness(), 1.f / shadowMap->getSize().width,
      _contactHardeningLightSizeUVRatio * shadowMap->getSize().width, frustumEdgeFalloff,
      lightIndex);
  }
  else {
    effect->setTexture("shadowSampler" + lightIndex, getShadowMapForRendering());
    light->_uniformBuffer->updateFloat4("shadowsInfo", getDarkness(),
                                        blurScale() / shadowMap->getSize().width, depthScale(),
                                        frustumEdgeFalloff, lightIndex);
  }

  light->_uniformBuffer->updateFloat2(
    "depthValues", getLight()->getDepthMinZ(*camera),
    getLight()->getDepthMinZ(*camera) + getLight()->getDepthMaxZ(*camera), lightIndex);
}

Matrix ShadowGenerator::getTransformMatrix()
{
  auto& scene = _scene;
  if (_currentRenderID == scene->getRenderId() && _currentFaceIndexCache == _currentFaceIndex) {
    return _transformMatrix;
  }

  _currentRenderID       = scene->getRenderId();
  _currentFaceIndexCache = _currentFaceIndex;

  auto lightPosition = _light->position();
  if (_light->computeTransformedInformation()) {
    lightPosition = _light->transformedPosition();
  }

  Vector3::NormalizeToRef(_light->getShadowDirection(_currentFaceIndex), _lightDirection);
  if (stl_util::almost_equal(std::abs(Vector3::Dot(_lightDirection, Vector3::Up())), 1.f)) {
    // Required to avoid perfectly perpendicular light
    _lightDirection.z = 0.0000000000001f;
  }

  if (_light->needProjectionMatrixCompute() || !lightPosition.equals(_cachedPosition)
      || !_lightDirection.equals(_cachedDirection)) {

    _cachedPosition.copyFrom(lightPosition);
    _cachedDirection.copyFrom(_lightDirection);
    _cacheInitialized = true;

    Matrix::LookAtLHToRef(lightPosition, lightPosition.add(_lightDirection), Vector3::Up(),
                          _viewMatrix);

    auto shadowMap = getShadowMap();

    if (shadowMap) {
      auto& renderList = shadowMap->renderList();
      if (!renderList.empty()) {
        _light->setShadowProjectionMatrix(_projectionMatrix, _viewMatrix, renderList);
      }
    }

    _viewMatrix.multiplyToRef(_projectionMatrix, _transformMatrix);
  }

  return _transformMatrix;
}

void ShadowGenerator::recreateShadowMap()
{
  auto& shadowMap = _shadowMap;
  if (!shadowMap) {
    return;
  }
  // Track render list.
  auto& renderList = shadowMap->renderList();
  // Clean up existing data.
  _disposeRTTandPostProcesses();
  // Reinitializes.
  _initializeGenerator();
  // Reaffect the filter to ensure a correct fallback if necessary.
  filter = filter();
  // Reaffect the filter.
  _applyFilterValues();
  // Reaffect Render List.
  _shadowMap->renderList = renderList;
}

void ShadowGenerator::_disposeBlurPostProcesses()
{
  if (_shadowMap2) {
    _shadowMap2->dispose();
    _shadowMap2 = nullptr;
  }

  if (_boxBlurPostprocess) {
    _boxBlurPostprocess->dispose();
    _boxBlurPostprocess = nullptr;
  }

  if (_kernelBlurXPostprocess) {
    _kernelBlurXPostprocess->dispose();
    _kernelBlurXPostprocess = nullptr;
  }

  if (_kernelBlurYPostprocess) {
    _kernelBlurYPostprocess->dispose();
    _kernelBlurYPostprocess = nullptr;
  }

  _blurPostProcesses.clear();
}

void ShadowGenerator::_disposeRTTandPostProcesses()
{
  if (_shadowMap) {
    _shadowMap->dispose();
    _shadowMap = nullptr;
  }

  _disposeBlurPostProcesses();
}

void ShadowGenerator::dispose()
{
  _disposeRTTandPostProcesses();

  if (_light) {
    _light->_shadowGenerator = nullptr;
    _light->_markMeshesAsLightDirty();
  }

  onBeforeShadowMapRenderMeshObservable.clear();
  onBeforeShadowMapRenderObservable.clear();
  onAfterShadowMapRenderMeshObservable.clear();
  onAfterShadowMapRenderObservable.clear();
}

json ShadowGenerator::serialize() const
{
  return nullptr;
}

ShadowGenerator* ShadowGenerator::Parse(
  const json& /*parsedShadowGenerator*/, Scene* /*scene*/,
  const std::function<IShadowLightPtr(size_t mapSize, const IShadowLightPtr& light)>& /*constr*/)
{
  return nullptr;
}

} // end of namespace BABYLON
