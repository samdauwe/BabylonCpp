#include <babylon/particles/gpu_particle_system.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/random.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/image_processing_configuration_defines.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/color3_gradient.h>
#include <babylon/misc/gradient_helper.h>
#include <babylon/particles/emittertypes/box_particle_emitter.h>
#include <babylon/particles/emittertypes/custom_particle_emitter.h>
#include <babylon/particles/emittertypes/iparticle_emitter_type.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

bool GPUParticleSystem::IsSupported()
{
  if (!Engine::LastCreatedEngine()) {
    return false;
  }
  return Engine::LastCreatedEngine()->webGLVersion() > 1.f;
}

GPUParticleSystem::GPUParticleSystem(const std::string& iName, size_t capacity,
                                     std::optional<size_t> randomTextureSize, Scene* scene,
                                     bool iIsAnimationSheetEnabled, const EffectPtr& customEffect)
    : BaseParticleSystem{iName}
    , activeParticleCount{this, &GPUParticleSystem::get_activeParticleCount,
                          &GPUParticleSystem::set_activeParticleCount}
    , _accumulatedCount{0}
    , _renderEffect{nullptr}
    , _updateEffect{nullptr}
    , _buffer0{nullptr}
    , _buffer1{nullptr}
    , _spriteBuffer{nullptr}
    , _targetIndex{0}
    , _sourceBuffer{nullptr}
    , _targetBuffer{nullptr}
    , _currentRenderId{-1}
    , _started{false}
    , _stopped{false}
    , _timeDelta{0}
    , _randomTexture{nullptr}
    , _randomTexture2{nullptr}
    , _attributesStrideSize{21}
    , _actualFrame{0}
    , _zeroVector3{Vector3::Zero()}
    , _rawTextureWidth{256}
    , _preWarmDone{false}
    , _colorGradientsTexture{nullptr}
    , _angularSpeedGradientsTexture{nullptr}
    , _sizeGradientsTexture{nullptr}
    , _velocityGradientsTexture{nullptr}
    , _limitVelocityGradientsTexture{nullptr}
    , _dragGradientsTexture{nullptr}
{
  _scene = scene ? scene : Engine::LastCreatedScene();

  uniqueId = _scene->getUniqueId();

  _customEffect[0] = customEffect;

  // Setup the default processing configuration to the scene.
  _attachImageProcessingConfiguration(nullptr);
  _engine = _scene->getEngine();

  if (!randomTextureSize.has_value()) {
    randomTextureSize = _engine->getCaps().maxTextureSize;
  }

  _capacity                = capacity;
  _activeCount             = capacity;
  _currentActiveCount      = 0;
  _isAnimationSheetEnabled = iIsAnimationSheetEnabled;

  _scene->particleSystems.emplace_back(this);

  _updateEffectOptions->attributes = {"position",
                                      "initialPosition",
                                      "age",
                                      "life",
                                      "seed",
                                      "size",
                                      "color",
                                      "direction",
                                      "initialDirection",
                                      "angle",
                                      "cellIndex",
                                      "cellStartOffset",
                                      "noiseCoordinates1",
                                      "noiseCoordinates2"};
  _updateEffectOptions->uniformsNames
    = {"currentCount", "timeDelta",  "emitterWM",     "lifeTime",
       "color1",       "color2",     "sizeRange",     "scaleRange",
       "gravity",      "emitPower",  "direction1",    "direction2",
       "minEmitBox",   "maxEmitBox", "radius",        "directionRandomizer",
       "height",       "coneAngle",  "stopFactor",    "angleRange",
       "radiusRange",  "cellInfos",  "noiseStrength", "limitVelocityDamping"};
  _updateEffectOptions->uniformBuffersNames       = {};
  _updateEffectOptions->samplers                  = {"randomSampler",
                                    "randomSampler2",
                                    "sizeGradientSampler",
                                    "angularSpeedGradientSampler",
                                    "velocityGradientSampler",
                                    "limitVelocityGradientSampler",
                                    "noiseSampler",
                                    "dragGradientSampler"};
  _updateEffectOptions->defines                   = "";
  _updateEffectOptions->fallbacks                 = nullptr;
  _updateEffectOptions->onCompiled                = nullptr;
  _updateEffectOptions->onError                   = nullptr;
  _updateEffectOptions->indexParameters           = {};
  _updateEffectOptions->maxSimultaneousLights     = 0;
  _updateEffectOptions->transformFeedbackVaryings = {};

  particleEmitterType = std::make_unique<BoxParticleEmitter>();

  // Random data
  auto maxTextureSize
    = std::min(static_cast<size_t>(_engine->getCaps().maxTextureSize), *randomTextureSize);
  Float32Array d;
  for (size_t i = 0; i < maxTextureSize; ++i) {
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
  }
  _randomTexture = std::make_unique<RawTexture>(
    ArrayBufferView(d), static_cast<int>(maxTextureSize), 1, Constants::TEXTUREFORMAT_RGBA, _scene,
    false, false, Constants::TEXTURE_NEAREST_SAMPLINGMODE, Constants::TEXTURETYPE_FLOAT);
  _randomTexture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _randomTexture->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  d.clear();
  for (size_t i = 0; i < maxTextureSize; ++i) {
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
  }
  _randomTexture2 = std::make_unique<RawTexture>(
    ArrayBufferView(d), static_cast<int>(maxTextureSize), 1, Constants::TEXTUREFORMAT_RGBA, _scene,
    false, false, Constants::TEXTURE_NEAREST_SAMPLINGMODE, Constants::TEXTURETYPE_FLOAT);
  _randomTexture2->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _randomTexture2->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  _randomTextureSize = maxTextureSize;
}

GPUParticleSystem::~GPUParticleSystem() = default;

Type GPUParticleSystem::type() const
{
  return Type::GPUPARTICLESYSTEM;
}

size_t GPUParticleSystem::getCapacity() const
{
  return _capacity;
}

size_t GPUParticleSystem::get_activeParticleCount() const
{
  return _activeCount;
}

void GPUParticleSystem::set_activeParticleCount(size_t value)
{
  _activeCount = std::min(value, _capacity);
}

bool GPUParticleSystem::get_isLocal() const
{
  return false;
}

void GPUParticleSystem::set_isLocal(bool /*value*/)
{
  // Ignore
}

bool GPUParticleSystem::isReady()
{
  if (!_updateEffect) {
    _recreateUpdateEffect();
    _recreateRenderEffect();
    return false;
  }

  if (!hasEmitter() || !_updateEffect->isReady() || !_imageProcessingConfiguration->isReady()
      || !_renderEffect->isReady() || !particleTexture || !particleTexture->isReady()) {
    return false;
  }

  return true;
}

bool GPUParticleSystem::isStarted() const
{
  return _started;
}

bool GPUParticleSystem::isStopped()
{
  return _stopped;
}

bool GPUParticleSystem::isStopping() const
{
  return false; // Stop is immediate on GPU
}

size_t GPUParticleSystem::getActiveCount() const
{
  return _currentActiveCount;
}

void GPUParticleSystem::start(size_t delay)
{
  if (!targetStopDuration && _hasTargetStopDurationDependantGradient()) {
    throw std::runtime_error(
      "Particle system started with a targetStopDuration dependant gradient (eg. "
      "startSizeGradients) but no targetStopDuration set");
  }
  if (delay > 0) {
    // Timeout
  }
  _started     = true;
  _stopped     = false;
  _preWarmDone = false;

  // Animations
  if (beginAnimationOnStart && !animations.empty()) {
    // getScene()->beginAnimation(this, beginAnimationFrom, beginAnimationTo,
    //                            beginAnimationLoop);
  }
}

void GPUParticleSystem::stop()
{
  _stopped = true;
}

void GPUParticleSystem::reset()
{
  _releaseBuffers();
  _releaseVAOs();
  _currentActiveCount = 0;
  _targetIndex        = 0;
}

Observable<Effect>& GPUParticleSystem::get_onBeforeDrawParticlesObservable()
{
  return _onBeforeDrawParticlesObservable;
}

std::string GPUParticleSystem::get_vertexShaderName() const
{
  return "gpuRenderParticles";
}

std::string GPUParticleSystem::getClassName() const
{
  return "GPUParticleSystem";
}

EffectPtr GPUParticleSystem::getCustomEffect(unsigned int iBlendMode)
{
  return stl_util::contains(_customEffect, iBlendMode) ? _customEffect[iBlendMode] :
                                                         _customEffect[0];
}

void GPUParticleSystem::setCustomEffect(const EffectPtr& effect, unsigned int iBlendMode)
{
  _customEffect[iBlendMode] = effect;
}

BaseParticleSystem&
GPUParticleSystem::_removeGradientAndTexture(float gradient, std::vector<ColorGradient>& gradients,
                                             const RawTexturePtr& texture)
{
  BaseParticleSystem::_removeGradientAndTexture(gradient, gradients, texture);
  _releaseBuffers();

  return *this;
}

BaseParticleSystem&
GPUParticleSystem::_removeGradientAndTexture(float gradient, std::vector<Color3Gradient>& gradients,
                                             const RawTexturePtr& texture)
{
  BaseParticleSystem::_removeGradientAndTexture(gradient, gradients, texture);

  _releaseBuffers();

  return *this;
}

BaseParticleSystem&
GPUParticleSystem::_removeGradientAndTexture(float gradient, std::vector<FactorGradient>& gradients,
                                             const RawTexturePtr& texture)
{
  BaseParticleSystem::_removeGradientAndTexture(gradient, gradients, texture);

  _releaseBuffers();

  return *this;
}

GPUParticleSystem& GPUParticleSystem::addColorGradient(float gradient, const Color4& iColor1,
                                                       const std::optional<Color4>& /*color2*/)
{
  ColorGradient colorGradient(gradient, iColor1);
  _colorGradients.emplace_back(colorGradient);

  _refreshColorGradient(true);

  _releaseBuffers();

  return *this;
}

void GPUParticleSystem::_refreshColorGradient(bool reorder)
{
  if (!_colorGradients.empty()) {
    if (reorder) {
      stl_util::sort_js_style(_colorGradients, [](const ColorGradient& a, const ColorGradient& b) {
        if (a.gradient < b.gradient) {
          return -1;
        }
        else if (a.gradient > b.gradient) {
          return 1;
        }

        return 0;
      });
    }

    if (_colorGradientsTexture) {
      _colorGradientsTexture->dispose();
      _colorGradientsTexture = nullptr;
    }
  }
}

void GPUParticleSystem::forceRefreshGradients()
{
  _refreshColorGradient();
  _refreshFactorGradient(_sizeGradients, "_sizeGradientsTexture");
  _refreshFactorGradient(_angularSpeedGradients, "_angularSpeedGradientsTexture");
  _refreshFactorGradient(_velocityGradients, "_velocityGradientsTexture");
  _refreshFactorGradient(_limitVelocityGradients, "_limitVelocityGradientsTexture");
  _refreshFactorGradient(_dragGradients, "_dragGradientsTexture");

  reset();
}

GPUParticleSystem& GPUParticleSystem::removeColorGradient(float gradient)
{
  _removeGradientAndTexture(gradient, _colorGradients, _colorGradientsTexture);
  _colorGradientsTexture = nullptr;

  return *this;
}

void GPUParticleSystem::_addFactorGradient(std::vector<FactorGradient>& factorGradients,
                                           float gradient, float factor)
{
  FactorGradient valueGradient(gradient, factor);
  factorGradients.emplace_back(valueGradient);

  _releaseBuffers();
}

GPUParticleSystem& GPUParticleSystem::addSizeGradient(float gradient, float factor,
                                                      const std::optional<float>& /*factor2*/)
{
  _addFactorGradient(_sizeGradients, gradient, factor);

  _refreshFactorGradient(_sizeGradients, "_sizeGradientsTexture", true);

  _releaseBuffers();

  return *this;
}

GPUParticleSystem& GPUParticleSystem::removeSizeGradient(float gradient)
{
  _removeGradientAndTexture(gradient, _sizeGradients, _sizeGradientsTexture);
  _sizeGradientsTexture = nullptr;

  return *this;
}

void GPUParticleSystem::_refreshFactorGradient(std::vector<FactorGradient>& factorGradients,
                                               const std::string& textureName, bool reorder)
{
  if (factorGradients.empty()) {
    return;
  }

  if (reorder) {
    stl_util::sort_js_style(factorGradients, [](const FactorGradient& a, const FactorGradient& b) {
      if (a.gradient < b.gradient) {
        return -1;
      }
      else if (a.gradient > b.gradient) {
        return 1;
      }

      return 0;
    });
  }

  const auto disposeTexture = [](RawTexturePtr& texture) -> void {
    texture->dispose();
    texture = nullptr;
  };

  if (textureName == "_sizeGradientsTexture") {
    disposeTexture(_sizeGradientsTexture);
  }
  else if (textureName == "_angularSpeedGradientsTexture") {
    disposeTexture(_angularSpeedGradientsTexture);
  }
  else if (textureName == "_velocityGradientsTexture") {
    disposeTexture(_velocityGradientsTexture);
  }
  else if (textureName == "_limitVelocityGradientsTexture") {
    disposeTexture(_limitVelocityGradientsTexture);
  }
  else if (textureName == "_dragGradientsTexture") {
    disposeTexture(_dragGradientsTexture);
  }
}

GPUParticleSystem&
GPUParticleSystem::addAngularSpeedGradient(float gradient, float factor,
                                           const std::optional<float>& /*factor2*/)
{
  _addFactorGradient(_angularSpeedGradients, gradient, factor);
  _refreshFactorGradient(_angularSpeedGradients, "_angularSpeedGradientsTexture", true);

  _releaseBuffers();

  return *this;
}

GPUParticleSystem& GPUParticleSystem::removeAngularSpeedGradient(float gradient)
{
  _removeGradientAndTexture(gradient, _angularSpeedGradients, _angularSpeedGradientsTexture);
  _angularSpeedGradientsTexture = nullptr;

  return *this;
}

GPUParticleSystem& GPUParticleSystem::addVelocityGradient(float gradient, float factor,
                                                          const std::optional<float>& /*factor2*/)
{
  _addFactorGradient(_velocityGradients, gradient, factor);
  _refreshFactorGradient(_velocityGradients, "_velocityGradientsTexture", true);

  _releaseBuffers();

  return *this;
}

GPUParticleSystem& GPUParticleSystem::removeVelocityGradient(float gradient)
{
  _removeGradientAndTexture(gradient, _velocityGradients, _velocityGradientsTexture);
  _velocityGradientsTexture = nullptr;

  return *this;
}

IParticleSystem&
GPUParticleSystem::addLimitVelocityGradient(float gradient, float factor,
                                            const std::optional<float>& /*factor2*/)
{
  _addFactorGradient(_limitVelocityGradients, gradient, factor);
  _refreshFactorGradient(_limitVelocityGradients, "_limitVelocityGradientsTexture", true);

  _releaseBuffers();

  return *this;
}

IParticleSystem& GPUParticleSystem::removeLimitVelocityGradient(float gradient)
{
  _removeGradientAndTexture(gradient, _limitVelocityGradients, _limitVelocityGradientsTexture);
  _limitVelocityGradientsTexture = nullptr;

  return *this;
}

IParticleSystem& GPUParticleSystem::addDragGradient(float gradient, float factor,
                                                    const std::optional<float>& /*factor2*/
)
{
  _addFactorGradient(_dragGradients, gradient, factor);
  _refreshFactorGradient(_dragGradients, "_dragGradientsTexture", true);

  _releaseBuffers();

  return *this;
}

IParticleSystem& GPUParticleSystem::removeDragGradient(float gradient)
{
  _removeGradientAndTexture(gradient, _dragGradients, _dragGradientsTexture);
  _dragGradientsTexture = nullptr;

  return *this;
}

IParticleSystem& GPUParticleSystem::addEmitRateGradient(float /*gradient*/, float /*factor*/,
                                                        const std::optional<float>& /*factor2*/)
{
  // Do nothing as emit rate is not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::removeEmitRateGradient(float /*gradient*/)
{
  // Do nothing as emit rate is not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::addStartSizeGradient(float /*gradient*/, float /*factor*/,
                                                         const std::optional<float>& /*factor2*/)
{
  // Do nothing as start size is not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::removeStartSizeGradient(float /*gradient*/)
{
  // Do nothing as start size is not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::addColorRemapGradient(float /*gradient*/, float /*min*/,
                                                          float /* max*/)
{
  // Do nothing as start size is not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::removeColorRemapGradient(float /*gradient*/)
{
  // Do nothing as start size is not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::addAlphaRemapGradient(float /*gradient*/, float /*min*/,
                                                          float /*max*/)
{
  // Do nothing as start size is not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::removeAlphaRemapGradient(float /*gradient*/)
{
  // Do nothing as start size is not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::addRampGradient(float /*gradient*/, const Color3& /*color*/)
{
  // Not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::removeRampGradient(float /*gradient*/)
{
  // Not supported by GPUParticleSystem
  return *this;
}

std::vector<Color3Gradient>& GPUParticleSystem::getRampGradients()
{
  return _emptyRampGradients;
}

bool GPUParticleSystem::get_useRampGradients() const
{
  // Not supported by GPUParticleSystem
  return false;
}

void GPUParticleSystem::set_useRampGradients(bool /*value*/)
{
  // Not supported by GPUParticleSystem
}

IParticleSystem& GPUParticleSystem::addLifeTimeGradient(float /*gradient*/, float /*factor*/,
                                                        const std::optional<float>& /*factor2*/)
{
  // Not supported by GPUParticleSystem
  return *this;
}

IParticleSystem& GPUParticleSystem::removeLifeTimeGradient(float /*gradient*/)
{
  // Not supported by GPUParticleSystem
  return *this;
}

void GPUParticleSystem::_reset()
{
  _releaseBuffers();
}

WebGLVertexArrayObjectPtr GPUParticleSystem::_createUpdateVAO(Buffer* source)
{
  std::unordered_map<std::string, VertexBufferPtr> updateVertexBuffers;
  updateVertexBuffers["position"] = source->createVertexBuffer(VertexBuffer::PositionKind, 0, 3);

  size_t offset = 3;
  if (particleEmitterType->getClassName() == "CustomParticleEmitter") {
    updateVertexBuffers["initialPosition"]
      = source->createVertexBuffer("initialPosition", offset, 3);
    offset += 3;
  }
  updateVertexBuffers["age"] = source->createVertexBuffer("age", offset, 1);
  offset += 1;
  updateVertexBuffers["life"] = source->createVertexBuffer("life", offset, 1);
  offset += 1;
  updateVertexBuffers["seed"] = source->createVertexBuffer("seed", offset, 4);
  offset += 4;
  updateVertexBuffers["size"] = source->createVertexBuffer("size", offset, 3);
  offset += 3;

  if (!_colorGradientsTexture) {
    updateVertexBuffers["color"] = source->createVertexBuffer(VertexBuffer::ColorKind, offset, 4);
    offset += 4;
  }

  updateVertexBuffers["direction"]
    = source->createVertexBuffer(VertexBuffer::DirectionKind, offset, 3);
  offset += 3;

  if (!_isBillboardBased) {
    updateVertexBuffers["initialDirection"]
      = source->createVertexBuffer(VertexBuffer::InitialDirectionKind, offset, 3);
    offset += 3;
  }

  if (_angularSpeedGradientsTexture) {
    updateVertexBuffers["angle"] = source->createVertexBuffer(VertexBuffer::AngleKind, offset, 1);
    offset += 1;
  }
  else {
    updateVertexBuffers["angle"] = source->createVertexBuffer(VertexBuffer::AngleKind, offset, 2);
    offset += 2;
  }

  if (_isAnimationSheetEnabled) {
    updateVertexBuffers["cellIndex"]
      = source->createVertexBuffer(VertexBuffer::CellIndexKind, offset, 1);
    offset += 1;
    if (spriteRandomStartCell) {
      updateVertexBuffers["cellStartOffset"]
        = source->createVertexBuffer(VertexBuffer::CellStartOffsetKind, offset, 1);
      offset += 1;
    }
  }

  if (noiseTexture()) {
    updateVertexBuffers["noiseCoordinates1"]
      = source->createVertexBuffer(VertexBuffer::NoiseCoordinates1Kind, offset, 3);
    offset += 3;
    updateVertexBuffers["noiseCoordinates2"]
      = source->createVertexBuffer(VertexBuffer::NoiseCoordinates2Kind, offset, 3);
    // offset += 3;
  }

  auto vao = _engine->recordVertexArrayObject(updateVertexBuffers, nullptr, _updateEffect);
  _engine->bindArrayBuffer(nullptr);

  return vao;
}

WebGLVertexArrayObjectPtr GPUParticleSystem::_createRenderVAO(Buffer* source, Buffer* spriteSource)
{
  std::unordered_map<std::string, VertexBufferPtr> renderVertexBuffers;
  auto attributesStrideSizeT = static_cast<size_t>(_attributesStrideSize);
  renderVertexBuffers["position"]
    = source->createVertexBuffer(VertexBuffer::PositionKind, 0, 3, attributesStrideSizeT, true);
  size_t offset = 3;
  if (particleEmitterType->getClassName() == "CustomParticleEmitter") {
    offset += 3;
  }
  renderVertexBuffers["age"]
    = source->createVertexBuffer("age", offset, 1, _attributesStrideSize, true);
  offset += 1;
  renderVertexBuffers["life"]
    = source->createVertexBuffer("life", offset, 1, _attributesStrideSize, true);
  offset += 5;
  renderVertexBuffers["size"]
    = source->createVertexBuffer("size", offset, 3, _attributesStrideSize, true);
  offset += 3;

  if (!_colorGradientsTexture) {
    renderVertexBuffers["color"]
      = source->createVertexBuffer(VertexBuffer::ColorKind, offset, 4, attributesStrideSizeT, true);
    offset += 4;
  }

  if (billboardMode == ParticleSystem::BILLBOARDMODE_STRETCHED) {
    renderVertexBuffers["direction"] = source->createVertexBuffer(
      VertexBuffer::DirectionKind, offset, 3, _attributesStrideSize, true);
  }
  offset += 3; // Direction

  if (!_isBillboardBased) {
    renderVertexBuffers["initialDirection"] = source->createVertexBuffer(
      VertexBuffer::InitialDirectionKind, offset, 3, attributesStrideSizeT, true);
    offset += 3;
  }

  renderVertexBuffers["angle"]
    = source->createVertexBuffer(VertexBuffer::AngleKind, offset, 1, attributesStrideSizeT, true);
  if (_angularSpeedGradientsTexture) {
    offset++;
  }
  else {
    offset += 2;
  }

  if (_isAnimationSheetEnabled) {
    renderVertexBuffers["cellIndex"] = source->createVertexBuffer(
      VertexBuffer::CellIndexKind, attributesStrideSizeT, 1, _attributesStrideSize, true);
    offset += 1;
    if (spriteRandomStartCell) {
      renderVertexBuffers["cellStartOffset"] = source->createVertexBuffer(
        VertexBuffer::CellStartOffsetKind, offset, 1, _attributesStrideSize, true);
      offset += 1;
    }
  }

  if (noiseTexture()) {
    renderVertexBuffers["noiseCoordinates1"] = source->createVertexBuffer(
      VertexBuffer::NoiseCoordinates1Kind, offset, 3, _attributesStrideSize, true);
    offset += 3;
    renderVertexBuffers["noiseCoordinates2"] = source->createVertexBuffer(
      VertexBuffer::NoiseCoordinates2Kind, offset, 3, _attributesStrideSize, true);
    // offset += 3;
  }

  renderVertexBuffers["offset"] = spriteSource->createVertexBuffer(VertexBuffer::OffsetKind, 0, 2);
  renderVertexBuffers["uv"]     = spriteSource->createVertexBuffer(VertexBuffer::UVKind, 2, 2);

  auto vao = _engine->recordVertexArrayObject(renderVertexBuffers, nullptr, _renderEffect);
  _engine->bindArrayBuffer(nullptr);

  return vao;
}

void GPUParticleSystem::_initialize(bool force)
{
  if (_buffer0 && !force) {
    return;
  }

  auto engine = _scene->getEngine();
  Float32Array data;

  _attributesStrideSize = 21;
  _targetIndex          = 0;

  if (particleEmitterType->getClassName() == "CustomParticleEmitter") {
    _attributesStrideSize += 3;
  }

  if (!isBillboardBased) {
    _attributesStrideSize += 3;
  }

  if (_colorGradientsTexture) {
    _attributesStrideSize -= 4;
  }

  if (_angularSpeedGradientsTexture) {
    _attributesStrideSize -= 1;
  }

  if (_isAnimationSheetEnabled) {
    _attributesStrideSize += 1;
    if (spriteRandomStartCell) {
      _attributesStrideSize += 1;
    }
  }

  if (noiseTexture()) {
    _attributesStrideSize += 6;
  }

  const auto usingCustomEmitter = particleEmitterType->getClassName() == "CustomParticleEmitter";
  auto& tmpVector               = TmpVectors::Vector3Array[0];

  for (size_t particleIndex = 0; particleIndex < _capacity; ++particleIndex) {
    // position
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);

    if (usingCustomEmitter) {
      std::static_pointer_cast<CustomParticleEmitter>(particleEmitterType)
        ->particlePositionGenerator(static_cast<int>(particleIndex), nullptr, tmpVector);
      data.emplace_back(tmpVector.x);
      data.emplace_back(tmpVector.y);
      data.emplace_back(tmpVector.z);
    }

    // Age and life
    // create the particle as a dead one to create a new one at start
    data.emplace_back(0.f);
    data.emplace_back(0.f);

    // Seed
    data.emplace_back(Math::random());
    data.emplace_back(Math::random());
    data.emplace_back(Math::random());
    data.emplace_back(Math::random());

    // Size
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);

    if (!_colorGradientsTexture) {
      // color
      data.emplace_back(0.f);
      data.emplace_back(0.f);
      data.emplace_back(0.f);
      data.emplace_back(0.f);
    }

    // direction
    if (usingCustomEmitter) {
      std::static_pointer_cast<CustomParticleEmitter>(particleEmitterType)
        ->particleDestinationGenerator(static_cast<int>(particleIndex), nullptr, tmpVector);
      data.emplace_back(tmpVector.x);
      data.emplace_back(tmpVector.y);
      data.emplace_back(tmpVector.z);
    }
    else {
      data.emplace_back(0.f);
      data.emplace_back(0.f);
      data.emplace_back(0.f);
    }

    if (!isBillboardBased) {
      // initialDirection
      data.emplace_back(0.f);
      data.emplace_back(0.f);
      data.emplace_back(0.f);
    }

    // angle
    data.emplace_back(0.f);

    if (!_angularSpeedGradientsTexture) {
      data.emplace_back(0.f);
    }

    if (_isAnimationSheetEnabled) {
      data.emplace_back(0.f);
      if (spriteRandomStartCell) {
        data.emplace_back(0.f);
      }
    }

    if (noiseTexture()) { // Random coordinates for reading into noise texture
      data.emplace_back(Math::random());
      data.emplace_back(Math::random());
      data.emplace_back(Math::random());
      data.emplace_back(Math::random());
      data.emplace_back(Math::random());
      data.emplace_back(Math::random());
    }
  }

  // Sprite data
  Float32Array spriteData{
    0.5f,  0.5f,  1.f, 1.f, //
    -0.5f, 0.5f,  0.f, 1.f, //
    -0.5f, -0.5f, 0.f, 0.f, //
    0.5f,  -0.5f, 1.f, 0.f  //
  };

  // Buffers
  _buffer0      = std::make_unique<Buffer>(engine, data, false, _attributesStrideSize);
  _buffer1      = std::make_unique<Buffer>(engine, data, false, _attributesStrideSize);
  _spriteBuffer = std::make_unique<Buffer>(engine, spriteData, false, 4);

  // Update VAO
  _updateVAO.clear();
  _updateVAO.emplace_back(_createUpdateVAO(_buffer0.get()));
  _updateVAO.emplace_back(_createUpdateVAO(_buffer1.get()));

  // Render VAO
  _renderVAO.clear();
  _renderVAO.emplace_back(_createRenderVAO(_buffer1.get(), _spriteBuffer.get()));
  _renderVAO.emplace_back(_createRenderVAO(_buffer0.get(), _spriteBuffer.get()));

  // Links
  _sourceBuffer = _buffer0.get();
  _targetBuffer = _buffer1.get();
}

void GPUParticleSystem::_recreateUpdateEffect()
{
  std::ostringstream definesStream;
  definesStream << (particleEmitterType ? particleEmitterType->getEffectDefines() : "");

  if (_isBillboardBased) {
    definesStream << "\n#define BILLBOARD";
  }

  if (_colorGradientsTexture) {
    definesStream << "\n#define COLORGRADIENTS";
  }

  if (_sizeGradientsTexture) {
    definesStream << "\n#define SIZEGRADIENTS";
  }

  if (_angularSpeedGradientsTexture) {
    definesStream << "\n#define ANGULARSPEEDGRADIENTS";
  }

  if (_velocityGradientsTexture) {
    definesStream << "\n#define VELOCITYGRADIENTS";
  }

  if (_limitVelocityGradientsTexture) {
    definesStream << "\n#define LIMITVELOCITYGRADIENTS";
  }

  if (_dragGradientsTexture) {
    definesStream << "\n#define DRAGGRADIENTS";
  }

  if (isAnimationSheetEnabled) {
    definesStream << "\n#define ANIMATESHEET";
    if (spriteRandomStartCell) {
      definesStream << "\n#define ANIMATESHEETRANDOMSTART";
    }
  }

  if (noiseTexture()) {
    definesStream << "\n#define NOISE";
  }

  auto defines = definesStream.str();

  if (_updateEffect && _updateEffectOptions->defines == defines) {
    return;
  }

  _updateEffectOptions->transformFeedbackVaryings = {"outPosition"};

  if (particleEmitterType->getClassName() == "CustomParticleEmitter") {
    _updateEffectOptions->transformFeedbackVaryings.emplace_back("outInitialPosition");
  }

  _updateEffectOptions->transformFeedbackVaryings.emplace_back("outAge");
  _updateEffectOptions->transformFeedbackVaryings.emplace_back("outLife");
  _updateEffectOptions->transformFeedbackVaryings.emplace_back("outSeed");
  _updateEffectOptions->transformFeedbackVaryings.emplace_back("outSize");

  if (!_colorGradientsTexture) {
    _updateEffectOptions->transformFeedbackVaryings.emplace_back("outColor");
  }

  _updateEffectOptions->transformFeedbackVaryings.emplace_back("outDirection");

  if (!_isBillboardBased) {
    _updateEffectOptions->transformFeedbackVaryings.emplace_back("outInitialDirection");
  }

  _updateEffectOptions->transformFeedbackVaryings.emplace_back("outAngle");

  if (isAnimationSheetEnabled) {
    _updateEffectOptions->transformFeedbackVaryings.emplace_back("outCellIndex");
    if (spriteRandomStartCell) {
      _updateEffectOptions->transformFeedbackVaryings.emplace_back("outCellStartOffset");
    }
  }

  if (noiseTexture()) {
    _updateEffectOptions->transformFeedbackVaryings.emplace_back("outNoiseCoordinates1");
    _updateEffectOptions->transformFeedbackVaryings.emplace_back("outNoiseCoordinates2");
  }

  _updateEffectOptions->defines = std::move(defines);
  _updateEffect = Effect::New("gpuUpdateParticles", *_updateEffectOptions, _scene->getEngine());
}

void GPUParticleSystem::fillDefines(std::vector<std::string>& defines, unsigned int iBlendMode)
{
  if (_scene->clipPlane.has_value()) {
    defines.emplace_back("#define CLIPPLANE");
  }
  if (_scene->clipPlane2.has_value()) {
    defines.emplace_back("#define CLIPPLANE2");
  }
  if (_scene->clipPlane3.has_value()) {
    defines.emplace_back("#define CLIPPLANE3");
  }
  if (_scene->clipPlane4.has_value()) {
    defines.emplace_back("#define CLIPPLANE4");
  }
  if (_scene->clipPlane5.has_value()) {
    defines.emplace_back("#define CLIPPLANE5");
  }
  if (_scene->clipPlane6.has_value()) {
    defines.emplace_back("#define CLIPPLANE6");
  }

  if (iBlendMode == ParticleSystem::BLENDMODE_MULTIPLY) {
    defines.emplace_back("#define BLENDMULTIPLYMODE");
  }

  if (isLocal()) {
    defines.emplace_back("#define LOCAL");
  }

  if (_isBillboardBased) {
    defines.emplace_back("#define BILLBOARD");

    switch (billboardMode) {
      case ParticleSystem::BILLBOARDMODE_Y:
        defines.emplace_back("#define BILLBOARDY");
        break;
      case ParticleSystem::BILLBOARDMODE_STRETCHED:
        defines.emplace_back("#define BILLBOARDSTRETCHED");
        break;
      case ParticleSystem::BILLBOARDMODE_ALL:
      default:
        break;
    }
  }

  if (_colorGradientsTexture) {
    defines.emplace_back("#define COLORGRADIENTS");
  }

  if (isAnimationSheetEnabled) {
    defines.emplace_back("#define ANIMATESHEET");
  }

  if (_imageProcessingConfiguration) {
    _imageProcessingConfiguration->prepareDefines(*_imageProcessingConfigurationDefines);
    defines.emplace_back("" + _imageProcessingConfigurationDefines->toString());
  }
}

void GPUParticleSystem::fillUniformsAttributesAndSamplerNames(std::vector<std::string>& uniforms,
                                                              std::vector<std::string>& attributes,
                                                              std::vector<std::string>& samplers)
{
  stl_util::concat(attributes, {"position", "age", "life", "size", "color", "offset", "uv",
                                "direction", "initialDirection", "angle", "cellIndex"});

  stl_util::concat(uniforms,
                   {"emitterWM", "worldOffset", "view", "projection", "colorDead", "invView",
                    "vClipPlane", "vClipPlane2", "vClipPlane3", "vClipPlane4", "vClipPlane5",
                    "vClipPlane6", "sheetInfos", "translationPivot", "eyePosition"});

  stl_util::concat(samplers, {"diffuseSampler", "colorGradientSampler"});

  if (_imageProcessingConfiguration) {
    ImageProcessingConfiguration::PrepareUniforms(uniforms, *_imageProcessingConfigurationDefines);
    ImageProcessingConfiguration::PrepareSamplers(samplers, *_imageProcessingConfigurationDefines);
  }
}

EffectPtr GPUParticleSystem::_recreateRenderEffect()
{
  const auto customEffect = getCustomEffect();

  if (customEffect) {
    return customEffect;
  }

  std::vector<std::string> defines;

  fillDefines(defines);

  const auto join = StringTools::join(defines, "\n");

  if (_renderEffect && _renderEffect->defines == join) {
    return _renderEffect;
  }

  std::vector<std::string> attributes;
  std::vector<std::string> uniforms;
  std::vector<std::string> samplers;

  fillUniformsAttributesAndSamplerNames(uniforms, attributes, samplers);

  IEffectCreationOptions renderEffectOptions;
  renderEffectOptions.attributes    = std::move(attributes);
  renderEffectOptions.uniformsNames = std::move(uniforms);
  renderEffectOptions.samplers      = std::move(samplers);
  renderEffectOptions.defines       = std::move(join);

  _renderEffect = Effect::New("gpuRenderParticles", renderEffectOptions, _scene->getEngine());

  return _renderEffect;
}

void GPUParticleSystem::animate(bool preWarm)
{
  _timeDelta = updateSpeed * (preWarm ? preWarmStepOffset : _scene->getAnimationRatio());
  _actualFrame += static_cast<int>(_timeDelta);

  if (!_stopped) {
    if (targetStopDuration && _actualFrame >= targetStopDuration) {
      stop();
    }
  }
}

RawTexture* GPUParticleSystem::_getRawTextureByName(const std::string& textureName)
{
  RawTexture* rawTexture = nullptr;

  if (textureName == "_colorGradientsTexture") {
    rawTexture = _colorGradientsTexture.get();
  }
  else if (textureName == "_angularSpeedGradientsTexture") {
    rawTexture = _angularSpeedGradientsTexture.get();
  }
  else if (textureName == "_sizeGradientsTexture") {
    rawTexture = _sizeGradientsTexture.get();
  }
  else if (textureName == "_velocityGradientsTexture") {
    rawTexture = _velocityGradientsTexture.get();
  }

  return rawTexture;
}

void GPUParticleSystem::_setRawTextureByName(const std::string& textureName,
                                             std::unique_ptr<RawTexture>&& rawTexture)
{
  if (textureName == "_colorGradientsTexture") {
    _colorGradientsTexture = std::move(rawTexture);
  }
  else if (textureName == "_angularSpeedGradientsTexture") {
    _angularSpeedGradientsTexture = std::move(rawTexture);
  }
  else if (textureName == "_sizeGradientsTexture") {
    _sizeGradientsTexture = std::move(rawTexture);
  }
  else if (textureName == "_velocityGradientsTexture") {
    _velocityGradientsTexture = std::move(rawTexture);
  }
}

template <typename T>
void GPUParticleSystem::_createFactorGradientTexture(const std::vector<T>& factorGradients,
                                                     const std::string& textureName)
{
  auto texture = _getRawTextureByName(textureName);

  if (factorGradients.empty() || texture) {
    return;
  }

  Float32Array data(_rawTextureWidth);

  for (unsigned int x = 0; x < _rawTextureWidth; x++) {
    auto ratio = static_cast<float>(x) / static_cast<float>(_rawTextureWidth);

    GradientHelper::GetCurrentGradient<FactorGradient>(
      ratio, factorGradients,
      [&](const FactorGradient& currentGradient, const FactorGradient& nextGradient, float scale) {
        data[x] = Scalar::Lerp(currentGradient.factor1, nextGradient.factor1, scale);
      });
  }

  _setRawTextureByName(
    textureName, RawTexture::CreateRTexture(data, static_cast<int>(_rawTextureWidth), 1, _scene,
                                            false, false, TextureConstants::NEAREST_SAMPLINGMODE));
}

void GPUParticleSystem::_createSizeGradientTexture()
{
  _createFactorGradientTexture(_sizeGradients, "_sizeGradientsTexture");
}

void GPUParticleSystem::_createAngularSpeedGradientTexture()
{
  _createFactorGradientTexture(_angularSpeedGradients, "_angularSpeedGradientsTexture");
}

void GPUParticleSystem::_createVelocityGradientTexture()
{
  _createFactorGradientTexture(_velocityGradients, "_velocityGradientsTexture");
}

void GPUParticleSystem::_createLimitVelocityGradientTexture()
{
  _createFactorGradientTexture(_limitVelocityGradients, "_limitVelocityGradientsTexture");
}

void GPUParticleSystem::_createDragGradientTexture()
{
  _createFactorGradientTexture(_dragGradients, "_dragGradientsTexture");
}

void GPUParticleSystem::_createColorGradientTexture()
{
  if (_colorGradients.empty() || _colorGradientsTexture) {
    return;
  }

  Uint8Array data(_rawTextureWidth * 4);
  auto& tmpColor = TmpVectors::Color4Array[0];

  for (unsigned int x = 0; x < _rawTextureWidth; ++x) {
    auto ratio = static_cast<float>(x) / static_cast<float>(_rawTextureWidth);

    GradientHelper::GetCurrentGradient<ColorGradient>(
      ratio, _colorGradients,
      [&](const ColorGradient& currentGradient, const ColorGradient& nextGradient, float scale) {
        Color4::LerpToRef(currentGradient.color1, nextGradient.color1, scale, tmpColor);
        data[x * 4]     = static_cast<uint8_t>(tmpColor.r * 255);
        data[x * 4 + 1] = static_cast<uint8_t>(tmpColor.g * 255);
        data[x * 4 + 2] = static_cast<uint8_t>(tmpColor.b * 255);
        data[x * 4 + 3] = static_cast<uint8_t>(tmpColor.a * 255);
      });
  }

  _colorGradientsTexture
    = RawTexture::CreateRGBATexture(data, static_cast<int>(_rawTextureWidth), 1, _scene, false,
                                    false, TextureConstants::NEAREST_SAMPLINGMODE);
}

size_t GPUParticleSystem::render(bool preWarm)
{
  if (!_started) {
    return 0;
  }

  _createColorGradientTexture();
  _createSizeGradientTexture();
  _createAngularSpeedGradientTexture();
  _createVelocityGradientTexture();
  _createLimitVelocityGradientTexture();
  _createDragGradientTexture();

  _recreateUpdateEffect();
  _recreateRenderEffect();

  if (!isReady()) {
    return 0;
  }

  if (!preWarm) {
    if (!_preWarmDone && preWarmCycles) {
      for (size_t index = 0; index < preWarmCycles; ++index) {
        animate(true);
        render(true);
      }

      _preWarmDone = true;
    }

    if (_currentRenderId == _scene->getFrameId()) {
      return 0;
    }

    _currentRenderId = _scene->getFrameId();
  }

  // Get everything ready to render
  _initialize();

  _accumulatedCount += static_cast<size_t>(emitRate * _timeDelta);
  if (_accumulatedCount > 1) {
    auto intPart = _accumulatedCount;
    _accumulatedCount -= intPart;
    _currentActiveCount = std::min(_activeCount, _currentActiveCount + intPart);
  }

  if (!_currentActiveCount) {
    return 0;
  }

  // Enable update effect
  _engine->enableEffect(_updateEffect);
  _engine->setState(false);

  _updateEffect->setFloat("currentCount", static_cast<float>(_currentActiveCount));
  _updateEffect->setFloat("timeDelta", _timeDelta);
  _updateEffect->setFloat("stopFactor", _stopped ? 0.f : 1.f);
  _updateEffect->setTexture("randomSampler", _randomTexture);
  _updateEffect->setTexture("randomSampler2", _randomTexture2);
  _updateEffect->setFloat2("lifeTime", minLifeTime, maxLifeTime);
  _updateEffect->setFloat2("emitPower", minEmitPower, maxEmitPower);
  if (!_colorGradientsTexture) {
    _updateEffect->setDirectColor4("color1", color1);
    _updateEffect->setDirectColor4("color2", color2);
  }
  _updateEffect->setFloat2("sizeRange", minSize, maxSize);
  _updateEffect->setFloat4("scaleRange", minScaleX, maxScaleX, minScaleY, maxScaleY);
  _updateEffect->setFloat4("angleRange", minAngularSpeed, maxAngularSpeed, minInitialRotation,
                           maxInitialRotation);
  _updateEffect->setVector3("gravity", gravity);

  if (_sizeGradientsTexture) {
    _updateEffect->setTexture("sizeGradientSampler", _sizeGradientsTexture);
  }

  if (_angularSpeedGradientsTexture) {
    _updateEffect->setTexture("angularSpeedGradientSampler", _angularSpeedGradientsTexture);
  }

  if (_velocityGradientsTexture) {
    _updateEffect->setTexture("velocityGradientSampler", _velocityGradientsTexture);
  }

  if (_limitVelocityGradientsTexture) {
    _updateEffect->setTexture("limitVelocityGradientSampler", _limitVelocityGradientsTexture);
    _updateEffect->setFloat("limitVelocityDamping", limitVelocityDamping);
  }

  if (_dragGradientsTexture) {
    _updateEffect->setTexture("dragGradientSampler", _dragGradientsTexture);
  }

  if (particleEmitterType) {
    particleEmitterType->applyToShader(_updateEffect.get());
  }
  if (_isAnimationSheetEnabled) {
    _updateEffect->setFloat3("cellInfos", static_cast<float>(startSpriteCellID),
                             static_cast<float>(endSpriteCellID), spriteCellChangeSpeed);
  }

  if (noiseTexture()) {
    _updateEffect->setTexture("noiseSampler", noiseTexture());
    _updateEffect->setVector3("noiseStrength", noiseStrength);
  }

  Matrix emitterWM;
  if (std::holds_alternative<AbstractMeshPtr>(emitter)) {
    auto emitterMesh = std::get<AbstractMeshPtr>(emitter);
    emitterWM        = emitterMesh->getWorldMatrix();
  }
  else {
    auto emitterPosition = std::get<Vector3>(emitter);
    emitterWM = Matrix::Translation(emitterPosition.x, emitterPosition.y, emitterPosition.z);
  }
  _updateEffect->setMatrix("emitterWM", emitterWM);

  // Bind source VAO
  _engine->bindVertexArrayObject(_updateVAO[_targetIndex], nullptr);

  // Update
  _engine->bindTransformFeedbackBuffer(_targetBuffer->getBuffer());
  _engine->setRasterizerState(false);
  _engine->beginTransformFeedback();
  _engine->drawArraysType(Material::PointListDrawMode, 0, static_cast<int>(_currentActiveCount));
  _engine->endTransformFeedback();
  _engine->setRasterizerState(true);
  _engine->bindTransformFeedbackBuffer(nullptr);

  if (!preWarm) {
    // Enable render effect
    _engine->enableEffect(_renderEffect);
    auto viewMatrix = _scene->getViewMatrix();
    _renderEffect->setMatrix("view", viewMatrix);
    _renderEffect->setMatrix("projection", _scene->getProjectionMatrix());
    _renderEffect->setTexture("textureSampler", particleTexture);
    _renderEffect->setVector2("translationPivot", translationPivot);
    _renderEffect->setVector3("worldOffset", worldOffset);
    if (_colorGradientsTexture) {
      _renderEffect->setTexture("colorGradientSampler", _colorGradientsTexture);
    }
    else {
      _renderEffect->setDirectColor4("colorDead", colorDead);
    }

    if (_isAnimationSheetEnabled && particleTexture) {
      auto baseSize = particleTexture->getBaseSize();
      _renderEffect->setFloat3(
        "sheetInfos", static_cast<float>(spriteCellWidth) / static_cast<float>(baseSize.width),
        static_cast<float>(spriteCellHeight) / static_cast<float>(baseSize.height),
        static_cast<float>(baseSize.width) / static_cast<float>(spriteCellWidth));
    }

    if (_isBillboardBased) {
      const auto& camera = _scene->activeCamera();
      _renderEffect->setVector3("eyePosition", camera->globalPosition);
    }

    if (_scene->clipPlane.has_value() || _scene->clipPlane2.has_value()
        || _scene->clipPlane3.has_value() || _scene->clipPlane4.has_value()
        || _scene->clipPlane5.has_value() || _scene->clipPlane6.has_value()) {
      auto invView = viewMatrix;
      invView.invert();
      _renderEffect->setMatrix("invView", invView);

      MaterialHelper::BindClipPlane(_renderEffect, _scene);
    }

    // image processing
    if (_imageProcessingConfiguration && !_imageProcessingConfiguration->applyByPostProcess) {
      _imageProcessingConfiguration->bind(_renderEffect.get());
    }

    // Draw order
    switch (blendMode) {
      case ParticleSystem::BLENDMODE_ADD:
        _engine->setAlphaMode(Constants::ALPHA_ADD);
        break;
      case ParticleSystem::BLENDMODE_ONEONE:
        _engine->setAlphaMode(Constants::ALPHA_ONEONE);
        break;
      case ParticleSystem::BLENDMODE_STANDARD:
        _engine->setAlphaMode(Constants::ALPHA_COMBINE);
        break;
      case ParticleSystem::BLENDMODE_MULTIPLY:
        _engine->setAlphaMode(Constants::ALPHA_MULTIPLY);
        break;
    }

    if (forceDepthWrite) {
      _engine->setDepthWrite(true);
    }

    // Bind source VAO
    _engine->bindVertexArrayObject(_renderVAO[_targetIndex], nullptr);

    // Render
    _engine->drawArraysType(Material::TriangleFanDrawMode, 0, 4,
                            static_cast<int>(_currentActiveCount));
    _engine->setAlphaMode(Constants::ALPHA_DISABLE);
  }

  // Switch VAOs
  ++_targetIndex;
  if (_targetIndex == 2) {
    _targetIndex = 0;
  }

  // Switch buffers
  std::swap(_sourceBuffer, _targetBuffer);

  return _currentActiveCount;
}

void GPUParticleSystem::rebuild()
{
  _initialize(true);
}

void GPUParticleSystem::_releaseBuffers()
{
  if (_buffer0) {
    _buffer0->dispose();
    _buffer0 = nullptr;
  }
  if (_buffer1) {
    _buffer1->dispose();
    _buffer1 = nullptr;
  }
  if (_spriteBuffer) {
    _spriteBuffer->dispose();
    _spriteBuffer = nullptr;
  }
}

void GPUParticleSystem::_releaseVAOs()
{
  if (_updateVAO.empty()) {
    return;
  }

  for (auto& _updateVAOItem : _updateVAO) {
    _engine->releaseVertexArrayObject(_updateVAOItem);
  }
  _updateVAO.clear();

  for (auto& _renderVAOItem : _renderVAO) {
    _engine->releaseVertexArrayObject(_renderVAOItem);
  }
  _renderVAO.clear();
}

void GPUParticleSystem::dispose(bool disposeTexture, bool /*disposeMaterialAndTextures*/)
{
  // Remove from scene
  stl_util::remove_vector_elements_equal_sharedptr(_scene->particleSystems, this);

  _releaseBuffers();
  _releaseVAOs();

  if (_colorGradientsTexture) {
    _colorGradientsTexture->dispose();
    _colorGradientsTexture = nullptr;
  }

  if (_sizeGradientsTexture) {
    _sizeGradientsTexture->dispose();
    _sizeGradientsTexture = nullptr;
  }

  if (_angularSpeedGradientsTexture) {
    _angularSpeedGradientsTexture->dispose();
    _angularSpeedGradientsTexture = nullptr;
  }

  if (_velocityGradientsTexture) {
    _velocityGradientsTexture->dispose();
    _velocityGradientsTexture = nullptr;
  }

  if (_limitVelocityGradientsTexture) {
    _limitVelocityGradientsTexture->dispose();
    _limitVelocityGradientsTexture = nullptr;
  }

  if (_dragGradientsTexture) {
    _dragGradientsTexture->dispose();
    _dragGradientsTexture = nullptr;
  }

  if (_randomTexture) {
    _randomTexture->dispose();
    _randomTexture = nullptr;
  }

  if (_randomTexture2) {
    _randomTexture2->dispose();
    _randomTexture2 = nullptr;
  }

  if (disposeTexture && particleTexture) {
    particleTexture->dispose();
    particleTexture = nullptr;
  }

  if (disposeTexture && noiseTexture()) {
    noiseTexture()->dispose();
    noiseTexture = nullptr;
  }

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

IParticleSystem* GPUParticleSystem::clone(const std::string& /*name*/, Mesh* /*newEmitter*/)
{
  return nullptr;
}

json GPUParticleSystem::serialize(bool /*serializeTexture*/) const
{
  return nullptr;
}

IParticleSystem* GPUParticleSystem::Parse(const json& /*parsedParticleSystem*/, Scene* /*scene*/,
                                          const std::string& /*rootUrl*/, bool /*doNotStart*/)
{
  return nullptr;
}

} // end of namespace BABYLON
