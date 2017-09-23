#include <babylon/lights/shadows/shadow_generator.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/vector2.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ShadowGenerator::ShadowGenerator(int mapSize, IShadowLight* light,
                                 bool useFullFloatFirst)
    : ShadowGenerator(ISize{mapSize, mapSize}, light, useFullFloatFirst)
{
}

ShadowGenerator::ShadowGenerator(const ISize& mapSize, IShadowLight* light,
                                 bool useFullFloatFirst)
    : frustumEdgeFalloff{0.f}
    , forceBackFacesOnly{false}
    , _bias{0.00005f}
    , _blurBoxOffset{1}
    , _blurScale{2.f}
    , _blurKernel{1.f}
    , _useKernelBlur{false}
    , _filter{ShadowGenerator::FILTER_NONE}
    , _darkness{0.f}
    , _transparencyShadow{false}
    , _shadowMap{nullptr}
    , _shadowMap2{nullptr}
    , _light{light}
    , _scene{light->getScene()}
    , _lightDirection{Vector3::Zero()}
    , _effect{nullptr}
    , _viewMatrix{Matrix::Zero()}
    , _projectionMatrix{Matrix::Zero()}
    , _transformMatrix{Matrix::Zero()}
    , _worldViewProjection{Matrix::Zero()}
    , _cacheInitialized{false}
    , _downSamplePostprocess{nullptr}
    , _boxBlurPostprocess{nullptr}
    , _kernelBlurXPostprocess{nullptr}
    , _kernelBlurYPostprocess{nullptr}
    , _mapSize{mapSize}
    , _currentFaceIndex{0}
    , _currentFaceIndexCache{0}
    , _useFullFloat{true}
    , _textureType{0}
    , _isCube{false}
    , _defaultTextureMatrix{Matrix::Identity()}
{
  light->_shadowGenerator = this;

  // Texture type fallback from float to int if not supported.
  const auto& caps = _scene->getEngine()->getCaps();

  if (!useFullFloatFirst) {
    if (caps.textureHalfFloatRender && caps.textureHalfFloatLinearFiltering) {
      _textureType = EngineConstants::TEXTURETYPE_HALF_FLOAT;
    }
    else if (caps.textureFloatRender && caps.textureFloatLinearFiltering) {
      _textureType = EngineConstants::TEXTURETYPE_FLOAT;
    }
    else {
      _textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
    }
  }
  else {
    if (caps.textureFloatRender && caps.textureFloatLinearFiltering) {
      _textureType = EngineConstants::TEXTURETYPE_FLOAT;
    }
    else if (caps.textureHalfFloatRender
             && caps.textureHalfFloatLinearFiltering) {
      _textureType = EngineConstants::TEXTURETYPE_HALF_FLOAT;
    }
    else {
      _textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
    }
  }

  _initializeGenerator();
}

ShadowGenerator::~ShadowGenerator()
{
}

float ShadowGenerator::bias() const
{
  return _bias;
}

void ShadowGenerator::setBias(float iBias)
{
  _bias = iBias;
}

int ShadowGenerator::blurBoxOffset() const
{
  return _blurBoxOffset;
}

void ShadowGenerator::setBlurBoxOffset(int value)
{
  if (_blurBoxOffset == value) {
    return;
  }

  _blurBoxOffset = value;
  _disposeBlurPostProcesses();
}

float ShadowGenerator::blurScale() const
{
  return _blurScale;
}

void ShadowGenerator::setBlurScale(float value)
{
  if (stl_util::almost_equal(_blurScale, value)) {
    return;
  }

  _blurScale = value;
  _disposeBlurPostProcesses();
}

float ShadowGenerator::blurKernel() const
{
  return _blurKernel;
}

void ShadowGenerator::setBlurKernel(float value)
{
  if (stl_util::almost_equal(_blurKernel, value)) {
    return;
  }

  _blurKernel = value;
  _disposeBlurPostProcesses();
}

bool ShadowGenerator::useKernelBlur() const
{
  return _useKernelBlur;
}

void ShadowGenerator::setUseKernelBlur(bool value)
{
  if (_useKernelBlur == value) {
    return;
  }

  _useKernelBlur = value;
  _disposeBlurPostProcesses();
}

float ShadowGenerator::depthScale() const
{
  return _depthScale ? *_depthScale : _light->getDepthScale();
}

void ShadowGenerator::setDepthScale(float value)
{
  _depthScale = value;
}

unsigned int ShadowGenerator::filter() const
{
  return _filter;
}

void ShadowGenerator::setFilter(unsigned int value)
{
  // Blurring the cubemap is going to be too expensive. Reverting to unblurred
  // version
  if (_light->needCube()) {
    if (value == ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP) {
      setUseExponentialShadowMap(true);
      return;
    }
    else if (value == ShadowGenerator::FILTER_BLURCLOSEEXPONENTIALSHADOWMAP) {
      setUseCloseExponentialShadowMap(true);
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

bool ShadowGenerator::usePoissonSampling() const
{
  return filter() == ShadowGenerator::FILTER_POISSONSAMPLING;
}

void ShadowGenerator::setUsePoissonSampling(bool value)
{
  setFilter(value ? ShadowGenerator::FILTER_POISSONSAMPLING :
                    ShadowGenerator::FILTER_NONE);
}

bool ShadowGenerator::useVarianceShadowMap() const
{
  BABYLON_LOG_WARN(
    "ShadowGenerator",
    "VSM are now replaced by ESM. Please use useExponentialShadowMap instead.")
  return useExponentialShadowMap();
}

void ShadowGenerator::setUseVarianceShadowMap(bool value)
{
  BABYLON_LOG_WARN(
    "ShadowGenerator",
    "VSM are now replaced by ESM. Please use useExponentialShadowMap instead.");
  setUseExponentialShadowMap(value);
}

bool ShadowGenerator::useBlurVarianceShadowMap() const
{
  BABYLON_LOG_WARN("ShadowGenerator",
                   "VSM are now replaced by ESM. Please use "
                   "useBlurExponentialShadowMap instead.");
  return useBlurExponentialShadowMap();
}

void ShadowGenerator::setUseBlurVarianceShadowMap(bool value)
{
  BABYLON_LOG_WARN("ShadowGenerator",
                   "VSM are now replaced by ESM. Please use "
                   "useBlurExponentialShadowMap instead.");
  setUseBlurExponentialShadowMap(value);
}

bool ShadowGenerator::useExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_EXPONENTIALSHADOWMAP;
}

void ShadowGenerator::setUseExponentialShadowMap(bool value)
{
  setFilter(value ? ShadowGenerator::FILTER_EXPONENTIALSHADOWMAP :
                    ShadowGenerator::FILTER_NONE);
}

bool ShadowGenerator::useBlurExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP;
}

void ShadowGenerator::setUseBlurExponentialShadowMap(bool value)
{
  setFilter(value ? ShadowGenerator::FILTER_BLUREXPONENTIALSHADOWMAP :
                    ShadowGenerator::FILTER_NONE);
}

bool ShadowGenerator::useCloseExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_CLOSEEXPONENTIALSHADOWMAP;
}

void ShadowGenerator::setUseCloseExponentialShadowMap(bool value)
{
  setFilter(value ? ShadowGenerator::FILTER_CLOSEEXPONENTIALSHADOWMAP :
                    ShadowGenerator::FILTER_NONE);
}

bool ShadowGenerator::useBlurCloseExponentialShadowMap() const
{
  return filter() == ShadowGenerator::FILTER_BLURCLOSEEXPONENTIALSHADOWMAP;
}

void ShadowGenerator::setUseBlurCloseExponentialShadowMap(bool value)
{
  setFilter(value ? ShadowGenerator::FILTER_BLURCLOSEEXPONENTIALSHADOWMAP :
                    ShadowGenerator::FILTER_NONE);
}

float ShadowGenerator::getDarkness() const
{
  return _darkness;
}

ShadowGenerator& ShadowGenerator::setDarkness(float darkness)
{
  if (darkness >= 1.f) {
    _darkness = 1.f;
  }
  else if (darkness <= 0.f) {
    _darkness = 0.f;
  }
  else {
    _darkness = darkness;
  }
  return *this;
}

ShadowGenerator& ShadowGenerator::setTransparencyShadow(bool hasShadow)
{
  _transparencyShadow = hasShadow;
  return *this;
}

RenderTargetTexture* ShadowGenerator::getShadowMap()
{
  return _shadowMap.get();
}

RenderTargetTexture* ShadowGenerator::getShadowMapForRendering()
{
  if (_shadowMap2) {
    return _shadowMap2.get();
  }

  return _shadowMap.get();
}

IShadowLight* ShadowGenerator::getLight()
{
  return _light;
}

void ShadowGenerator::_initializeGenerator()
{
  _light->_markMeshesAsLightDirty();
  _initializeShadowMap();
}

void ShadowGenerator::_initializeShadowMap()
{
  // Render target
  _shadowMap = ::std::make_unique<RenderTargetTexture>(
    _light->name + "_shadowMap", _mapSize, _scene, false, true, _textureType,
    _light->needCube());
  _shadowMap->wrapU                     = TextureConstants::CLAMP_ADDRESSMODE;
  _shadowMap->wrapV                     = TextureConstants::CLAMP_ADDRESSMODE;
  _shadowMap->anisotropicFilteringLevel = 1;
  _shadowMap->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  _shadowMap->renderParticles = false;

  // Record Face Index before render.
  _shadowMap->onBeforeRenderObservable.add([this](int* faceIndex, EventState&) {
    _currentFaceIndex = static_cast<unsigned int>(*faceIndex);
  });

  // Custom render function.
  _shadowMap->customRenderFunction
    = [this](const vector_t<SubMesh*>& opaqueSubMeshes,
             const vector_t<SubMesh*>& transparentSubMeshes,
             const vector_t<SubMesh*>& alphaTestSubMeshes) {
        _renderForShadowMap(opaqueSubMeshes, transparentSubMeshes,
                            alphaTestSubMeshes);
      };

  // Blur if required afer render.
  _shadowMap->onAfterUnbindObservable.add([this](RenderTargetTexture*,
                                                 EventState&) {
    if (!useBlurExponentialShadowMap() && !useBlurCloseExponentialShadowMap()) {
      return;
    }

    if (_blurPostProcesses.empty()) {
      _initializeBlurRTTAndPostProcesses();
    }

    _scene->postProcessManager->directRender(
      _blurPostProcesses, getShadowMapForRendering()->getInternalTexture());
  });

  // Clear according to the chosen filter.
  _shadowMap->onClearObservable.add([this](Engine* engine, EventState&) {
    if (useExponentialShadowMap() || useBlurExponentialShadowMap()) {
      engine->clear(Color4(0.f, 0.f, 0.f, 0.f), true, true, true);
    }
    else {
      engine->clear(Color4(1.f, 1.f, 1.f, 1.f), true, true, true);
    }
  });
}

void ShadowGenerator::_initializeBlurRTTAndPostProcesses()
{
  auto engine    = _scene->getEngine();
  int targetSize = static_cast<int>(_mapSize.width / blurScale());

  if (!useKernelBlur() || blurScale() != 1.f) {
    _shadowMap2 = ::std::make_unique<RenderTargetTexture>(
      _light->name + "_shadowMap2", ISize{targetSize, targetSize}, _scene,
      false, true, _textureType);
    _shadowMap2->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
    _shadowMap2->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
    _shadowMap2->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  }

  if (useKernelBlur()) {
    _kernelBlurXPostprocess = ::std::make_unique<BlurPostProcess>(
      _light->name + "KernelBlurX", Vector2(1.f, 0.f), blurKernel(), 1.f,
      nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, engine, false,
      _textureType);
    _kernelBlurXPostprocess->width  = targetSize;
    _kernelBlurXPostprocess->height = targetSize;
    _kernelBlurXPostprocess->onApplyObservable.add(
      [this](Effect* effect, EventState&) {
        effect->setTexture("textureSampler", _shadowMap.get());
      });

    _kernelBlurYPostprocess = ::std::make_unique<BlurPostProcess>(
      _light->name + "KernelBlurY", Vector2(0.f, 1.f), blurKernel(), 1.f,
      nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, engine, false,
      _textureType);

    _kernelBlurXPostprocess->autoClear = false;
    _kernelBlurYPostprocess->autoClear = false;

    if (_textureType == EngineConstants::TEXTURETYPE_UNSIGNED_INT) {
      static_cast<BlurPostProcess*>(_kernelBlurXPostprocess.get())
        ->setPackedFloat(true);
      static_cast<BlurPostProcess*>(_kernelBlurYPostprocess.get())
        ->setPackedFloat(true);
    }

    _blurPostProcesses
      = {_kernelBlurXPostprocess.get(), _kernelBlurYPostprocess.get()};
  }
  else {
#if 0
    _boxBlurPostprocess = ::std::make_unique<PostProcess>(
      _light->name + "DepthBoxBlur", "depthBoxBlur",
      {"screenSize", "boxOffset"}, {}, 1.f, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, engine, false,
      "#define OFFSET " + ::std::to_string(_blurBoxOffset), _textureType);
    _boxBlurPostprocess->onApplyObservable.add([this](Effect* effect) {
      int targetSize = static_cast<int>(_mapSize.width / blurScale());
      effect->setFloat2("screenSize", targetSize, targetSize);
      effect->setTexture("textureSampler", _shadowMap.get());
  });

  _boxBlurPostprocess->autoClear = false;

  _blurPostProcesses = {_boxBlurPostprocess.get()};
#endif
  }
}

void ShadowGenerator::_renderForShadowMap(
  const vector_t<SubMesh*>& opaqueSubMeshes,
  const vector_t<SubMesh*>& alphaTestSubMeshes,
  const vector_t<SubMesh*>& transparentSubMeshes)
{
  for (const auto& opaqueSubMesh : opaqueSubMeshes) {
    _renderSubMeshForShadowMap(opaqueSubMesh);
  }

  for (const auto& alphaTestSubMesh : alphaTestSubMeshes) {
    _renderSubMeshForShadowMap(alphaTestSubMesh);
  }

  if (_transparencyShadow) {
    for (const auto& transparentSubMesh : transparentSubMeshes) {
      _renderSubMeshForShadowMap(transparentSubMesh);
    }
  }
}

void ShadowGenerator::_renderSubMeshForShadowMap(SubMesh* subMesh)
{
  auto mesh   = subMesh->getRenderingMesh();
  auto scene  = _scene;
  auto engine = scene->getEngine();

  // Culling
  engine->setState(subMesh->getMaterial()->backFaceCulling());

  // Managing instances
  auto batch = mesh->_getInstancesRenderList(subMesh->_id);
  if (batch->mustReturn) {
    return;
  }

  auto hardwareInstancedRendering
    = (engine->getCaps().instancedArrays)
      && (stl_util::contains(batch->visibleInstances, subMesh->_id))
      && (!batch->visibleInstances[subMesh->_id].empty());
  if (isReady(subMesh, hardwareInstancedRendering)) {
    engine->enableEffect(_effect);
    mesh->_bind(subMesh, _effect, Material::TriangleFillMode);
    auto material = subMesh->getMaterial();

    _effect->setFloat2("biasAndScale", bias(), depthScale());

    _effect->setMatrix("viewProjection", getTransformMatrix());
    _effect->setVector3("lightPosition", getLight()->position);

    _effect->setFloat2("depthValues",
                       getLight()->getDepthMinZ(scene->activeCamera),
                       getLight()->getDepthMinZ(scene->activeCamera)
                         + getLight()->getDepthMaxZ(scene->activeCamera));

    // Alpha test
    if (material && material->needAlphaTesting()) {
      auto alphaTexture = material->getAlphaTestTexture();
      if (alphaTexture) {
        _effect->setTexture("diffuseSampler", alphaTexture);
        _effect->setMatrix("diffuseMatrix",
                           alphaTexture->getTextureMatrix() ?
                             *alphaTexture->getTextureMatrix() :
                             _defaultTextureMatrix);
      }
    }

    // Bones
    if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
      _effect->setMatrices("mBones",
                           mesh->skeleton()->getTransformMatrices(mesh));
    }

    if (forceBackFacesOnly) {
      engine->setState(true, 0, false, true);
    }

    // Draw
    mesh->_processRendering(
      subMesh, _effect, Material::TriangleFillMode, batch,
      hardwareInstancedRendering,
      [&](bool /*isInstance*/, Matrix world, Material* /*effectiveMaterial*/) {
        _effect->setMatrix("world", world);
      });

    if (forceBackFacesOnly) {
      engine->setState(true, 0, false, false);
    }
  }
  else {
    // Need to reset refresh rate of the shadowMap
    _shadowMap->resetRefreshCounter();
  }
}

void ShadowGenerator::_applyFilterValues()
{
  if (filter() == ShadowGenerator::FILTER_NONE) {
    _shadowMap->updateSamplingMode(TextureConstants::NEAREST_SAMPLINGMODE);
  }
  else {
    _shadowMap->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  }
}

void ShadowGenerator::forceCompilation(
  const ::std::function<void(ShadowGenerator* generator)>& onCompiled,
  const ShadowGeneratorCompileOptions& options)
{
  vector_t<SubMesh*> subMeshes;
  std::size_t currentIndex = 0;

  for (auto& mesh : getShadowMap()->renderList) {
    for (auto& subMesh : mesh->subMeshes) {
      subMeshes.emplace_back(subMesh.get());
    }
  }

  const auto checkReady = [&]() {
    if (!_scene || !_scene->getEngine()) {
      return;
    }

    const auto& subMesh = subMeshes[currentIndex];

    if (isReady(subMesh, options.useInstances)) {
      ++currentIndex;
      if (currentIndex >= subMeshes.size()) {
        if (onCompiled) {
          onCompiled(this);
        }
        return;
      }
    }
  };

  if (!subMeshes.empty()) {
    checkReady();
  }
}

bool ShadowGenerator::isReady(SubMesh* subMesh, bool useInstances)
{
  vector_t<string_t> defines;

  if (_textureType != EngineConstants::TEXTURETYPE_UNSIGNED_INT) {
    defines.emplace_back("#define FLOAT");
  }

  if (useExponentialShadowMap() || useBlurExponentialShadowMap()) {
    defines.emplace_back("#define ESM");
  }

  vector_t<string_t> attribs{VertexBuffer::PositionKindChars};

  auto mesh     = subMesh->getMesh();
  auto material = subMesh->getMaterial();

  // Alpha test
  if (material && material->needAlphaTesting()) {
    auto alphaTexture = material->getAlphaTestTexture();
    if (alphaTexture) {
      defines.emplace_back("#define ALPHATEST");
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        attribs.emplace_back(VertexBuffer::UVKindChars);
        defines.emplace_back("#define UV1");
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        if (alphaTexture->coordinatesIndex == 1) {
          attribs.emplace_back(VertexBuffer::UV2KindChars);
          defines.emplace_back("#define UV2");
        }
      }
    }
  }

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    if (mesh->numBoneInfluencers() > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKindChars);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKindChars);
    }
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + ::std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back(
      String::concat("#define BonesPerMesh "
                     + ::std::to_string(mesh->skeleton()->bones.size() + 1)));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    attribs.emplace_back(VertexBuffer::World0KindChars);
    attribs.emplace_back(VertexBuffer::World1KindChars);
    attribs.emplace_back(VertexBuffer::World2KindChars);
    attribs.emplace_back(VertexBuffer::World3KindChars);
  }

  // Get correct effect
  string_t join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    EffectCreationOptions options;
    options.attributes = ::std::move(attribs);
    options.uniformsNames
      = {"world",         "mBones",      "viewProjection", "diffuseMatrix",
         "lightPosition", "depthValues", "biasAndScale"};
    options.samplers = {"diffuseSampler"};
    options.defines  = ::std::move(join);

    _effect = _scene->getEngine()->createEffect("shadowMap", options,
                                                _scene->getEngine());
  }

  return _effect->isReady();
}

void ShadowGenerator::prepareDefines(MaterialDefines& defines,
                                     unsigned int lightIndex)
{
  auto scene = _scene;
  auto light = _light;

  if (!scene->shadowsEnabled() || !light->shadowEnabled) {
    return;
  }

  defines.shadows[lightIndex] = true;

  if (usePoissonSampling()) {
    defines.shadowpcfs[lightIndex] = true;
  }
  else if (useExponentialShadowMap() || useBlurExponentialShadowMap()) {
    defines.shadowesms[lightIndex] = true;
  }
  else if (useCloseExponentialShadowMap()
           || useBlurCloseExponentialShadowMap()) {
    defines.shadowcloseesms[lightIndex] = true;
  }

  if (light->needCube()) {
    defines.shadowcubes[lightIndex] = true;
  }
}

void ShadowGenerator::bindShadowLight(const string_t& lightIndex,
                                      Effect* effect)
{
  auto scene = _scene;
  auto light = _light;

  if (!scene->shadowsEnabled() || !light->shadowEnabled) {
    return;
  }

  if (!light->needCube()) {
    effect->setMatrix("lightMatrix" + lightIndex, getTransformMatrix());
  }
  effect->setTexture("shadowSampler" + lightIndex, getShadowMapForRendering());
  light->_uniformBuffer->updateFloat4(
    "shadowsInfo", getDarkness(), blurScale() / getShadowMap()->getSize().width,
    depthScale(), frustumEdgeFalloff, lightIndex);
  light->_uniformBuffer->updateFloat2(
    "depthValues", getLight()->getDepthMinZ(scene->activeCamera),
    getLight()->getDepthMinZ(scene->activeCamera)
      + getLight()->getDepthMaxZ(scene->activeCamera),
    lightIndex);
}

Matrix ShadowGenerator::getTransformMatrix()
{
  auto scene = _scene;
  if (_currentRenderID == scene->getRenderId()
      && _currentFaceIndexCache == _currentFaceIndex) {
    return _transformMatrix;
  }

  _currentRenderID       = scene->getRenderId();
  _currentFaceIndexCache = _currentFaceIndex;

  auto lightPosition = _light->position;
  if (_light->computeTransformedInformation()) {
    lightPosition = _light->transformedPosition();
  }

  Vector3::NormalizeToRef(_light->getShadowDirection(_currentFaceIndex),
                          _lightDirection);

  if (stl_util::almost_equal(
        ::std::abs(Vector3::Dot(_lightDirection, Vector3::Up())), 1.f)) {
    // Required to avoid perfectly perpendicular light
    _lightDirection.z = 0.0000000000001f;
  }

  if (_light->needProjectionMatrixCompute()
      || !lightPosition.equals(_cachedPosition)
      || !_lightDirection.equals(_cachedDirection)) {

    _cachedPosition   = lightPosition;
    _cachedDirection  = _lightDirection;
    _cacheInitialized = true;

    Matrix::LookAtLHToRef(lightPosition, lightPosition.add(_lightDirection),
                          Vector3::Up(), _viewMatrix);

    _light->setShadowProjectionMatrix(_projectionMatrix, _viewMatrix,
                                      getShadowMap()->renderList);

    _viewMatrix.multiplyToRef(_projectionMatrix, _transformMatrix);
  }

  return _transformMatrix;
}

void ShadowGenerator::recreateShadowMap()
{
  // Track render list.
  auto& renderList = _shadowMap->renderList;
  // Clean up existing data.
  _disposeRTTandPostProcesses();
  // Reinitializes.
  _initializeGenerator();
  // Reaffect the filter to ensure a correct fallback if necessary.
  setFilter(filter());
  // Reaffect the filter.
  _applyFilterValues();
  // Reaffect Render List.
  _shadowMap->renderList = renderList;
}

void ShadowGenerator::_disposeBlurPostProcesses()
{
  if (_shadowMap2) {
    _shadowMap2->dispose();
    _shadowMap2.reset(nullptr);
  }

  if (_downSamplePostprocess) {
    _downSamplePostprocess->dispose();
    _downSamplePostprocess.reset(nullptr);
  }

  if (_boxBlurPostprocess) {
    _boxBlurPostprocess->dispose();
    _boxBlurPostprocess.reset(nullptr);
  }

  if (_kernelBlurXPostprocess) {
    _kernelBlurXPostprocess->dispose();
    _kernelBlurXPostprocess.reset(nullptr);
  }

  if (_kernelBlurYPostprocess) {
    _kernelBlurYPostprocess->dispose();
    _kernelBlurYPostprocess.reset(nullptr);
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

  _light->_shadowGenerator = nullptr;
  _light->_markMeshesAsLightDirty();
}

Json::object ShadowGenerator::serialize() const
{
  return Json::object();
}

ShadowGenerator*
ShadowGenerator::Parse(const Json::value& parsedShadowGenerator, Scene* scene)
{
  // Casting to point light, as light is missing the position attr and
  // typescript complains.
  const string_t parsedShadowGeneratorLightId
    = Json::GetString(parsedShadowGenerator, "lightId");

  if (parsedShadowGeneratorLightId.empty()) {
    return nullptr;
  }

  auto light = dynamic_cast<PointLight*>(
    scene->getLightByID(parsedShadowGeneratorLightId));

  if (!light) {
    return nullptr;
  }

  auto size            = Json::GetNumber(parsedShadowGenerator, "mapSize", 0);
  auto shadowGenerator = new ShadowGenerator(ISize(size, size), light);
  auto shadowMap       = shadowGenerator->getShadowMap();

  for (const auto& renderItem :
       Json::GetArray(parsedShadowGenerator, "renderList")) {
    auto meshes = scene->getMeshesByID(renderItem.get<string_t>());
    for (auto& mesh : meshes) {
      shadowMap->renderList.emplace_back(mesh);
    }
  }

  if (Json::GetBool(parsedShadowGenerator, "usePoissonSampling")) {
    shadowGenerator->setUsePoissonSampling(true);
  }
  else if (Json::GetBool(parsedShadowGenerator, "useExponentialShadowMap")) {
    shadowGenerator->setUseExponentialShadowMap(true);
  }
  else if (Json::GetBool(parsedShadowGenerator,
                         "useBlurExponentialShadowMap")) {
    shadowGenerator->setUseBlurExponentialShadowMap(true);
  }
  else if (Json::GetBool(parsedShadowGenerator,
                         "useCloseExponentialShadowMap")) {
    shadowGenerator->setUseCloseExponentialShadowMap(true);
  }
  else if (Json::GetBool(parsedShadowGenerator,
                         "useBlurExponentialShadowMap")) {
    shadowGenerator->setUseBlurCloseExponentialShadowMap(true);
  }

  // Backward compat
  else if (Json::GetBool(parsedShadowGenerator, "useVarianceShadowMap")) {
    shadowGenerator->setUseExponentialShadowMap(true);
  }
  else if (Json::GetBool(parsedShadowGenerator, "useBlurVarianceShadowMap")) {
    shadowGenerator->setUseExponentialShadowMap(true);
  }

  if (parsedShadowGenerator.contains("depthScale")) {
    shadowGenerator->setDepthScale(
      Json::GetNumber(parsedShadowGenerator, "depthScale", 30.f));
  }

  if (parsedShadowGenerator.contains("blurScale")) {
    shadowGenerator->setBlurScale(
      Json::GetNumber(parsedShadowGenerator, "blurScale", 2.f));
  }

  if (parsedShadowGenerator.contains("blurBoxOffset")) {
    shadowGenerator->setBlurBoxOffset(
      Json::GetNumber(parsedShadowGenerator, "blurBoxOffset", 0));
  }

  if (parsedShadowGenerator.contains("useKernelBlur")) {
    shadowGenerator->setUseKernelBlur(
      Json::GetBool(parsedShadowGenerator, "useKernelBlur"));
  }

  if (parsedShadowGenerator.contains("blurKernel")) {
    shadowGenerator->setBlurKernel(
      Json::GetNumber(parsedShadowGenerator, "blurKernel", 1.f));
  }

  if (parsedShadowGenerator.contains("bias")) {
    shadowGenerator->setBias(
      Json::GetNumber(parsedShadowGenerator, "bias", 0.00005f));
  }

  if (parsedShadowGenerator.contains("darkness")) {
    shadowGenerator->setDarkness(
      Json::GetNumber(parsedShadowGenerator, "darkness", 0.f));
  }

  if (parsedShadowGenerator.contains("transparencyShadow")) {
    shadowGenerator->setTransparencyShadow(true);
  }

  shadowGenerator->forceBackFacesOnly
    = Json::GetBool(parsedShadowGenerator, "forceBackFacesOnly");

  return shadowGenerator;
}

} // end of namespace BABYLON
