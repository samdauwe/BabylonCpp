#include <babylon/particles/gpu_particle_system.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/random.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/math/scalar.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/particles/emittertypes/box_particle_emitter.h>
#include <babylon/particles/emittertypes/iparticle_emitter_Type.h>
#include <babylon/particles/particle_system.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

bool GPUParticleSystem::IsSupported()
{
  if (!Engine::LastCreatedEngine()) {
    return false;
  }
  return Engine::LastCreatedEngine()->webGLVersion() > 1.f;
}

GPUParticleSystem::GPUParticleSystem(const string_t& iName, size_t capacity,
                                     nullable_t<int> randomTextureSize,
                                     Scene* scene, bool isAnimationSheetEnabled)
    : direction1{this, &GPUParticleSystem::get_direction1,
                 &GPUParticleSystem::set_direction1}
    , direction2{this, &GPUParticleSystem::get_direction2,
                 &GPUParticleSystem::set_direction2}
    , minEmitBox{this, &GPUParticleSystem::get_minEmitBox,
                 &GPUParticleSystem::set_minEmitBox}
    , maxEmitBox{this, &GPUParticleSystem::get_maxEmitBox,
                 &GPUParticleSystem::set_maxEmitBox}
    , forceDepthWrite{false}
    , activeParticleCount{this, &GPUParticleSystem::get_activeParticleCount,
                          &GPUParticleSystem::set_activeParticleCount}
    , isAnimationSheetEnabled{this,
                              &GPUParticleSystem::get_isAnimationSheetEnabled}
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
    , _isBillboardBased{true}
    , _colorGradientsTexture{nullptr}
    , _angularSpeedGradientsTexture{nullptr}
    , _sizeGradientsTexture{nullptr}
    , _velocityGradientsTexture{nullptr}
{
  // IParticleSystem
  {
    id                    = iName;
    name                  = iName;
    renderingGroupId      = 0;
    layerMask             = 0x0FFFFFFF;
    updateSpeed           = 0.01f;
    targetStopDuration    = 0;
    particleTexture       = nullptr;
    blendMode             = ParticleSystem::BLENDMODE_ONEONE;
    minLifeTime           = 1.f;
    maxLifeTime           = 1.f;
    minSize               = 1.f;
    maxSize               = 1.f;
    minScaleX             = 1.f;
    maxScaleX             = 1.f;
    minScaleY             = 1.f;
    maxScaleY             = 1.f;
    color1                = Color4{1.f, 1.f, 1.f, 1.f};
    color2                = Color4{1.f, 1.f, 1.f, 1.f};
    colorDead             = Color4{0.f, 0.f, 0.f, 0.f};
    emitRate              = 100;
    gravity               = Vector3::Zero();
    minEmitPower          = 1.f;
    maxEmitPower          = 1.f;
    minAngularSpeed       = 0.f;
    maxAngularSpeed       = 0.f;
    particleEmitterType   = nullptr;
    preWarmCycles         = 0;
    preWarmStepOffset     = 1;
    minInitialRotation    = 0.f;
    maxInitialRotation    = 0.f;
    spriteCellChangeSpeed = 1.f;
    startSpriteCellID     = 0;
    endSpriteCellID       = 0;
    spriteCellWidth       = 0;
    spriteCellHeight      = 0;
    translationPivot      = Vector2(0.f, 0.f);
    noiseTexture          = nullptr;
    noiseStrength         = Vector3(10.f, 10.f, 10.f);
    billboardMode         = AbstractMesh::BILLBOARDMODE_ALL;
  }

  _scene  = scene ? scene : Engine::LastCreatedScene();
  _engine = _scene->getEngine();

  if (!randomTextureSize.has_value()) {
    randomTextureSize = _engine->getCaps().maxTextureSize;
  }

  _capacity                = capacity;
  _activeCount             = capacity;
  _currentActiveCount      = 0;
  _isAnimationSheetEnabled = isAnimationSheetEnabled;

  scene->particleSystems.emplace_back(this);

  _updateEffectOptions->attributes
    = {"position", "age",      "life",      "seed",
       "size",     "color",    "direction", "initialDirection",
       "angle",    "cellIndex"};
  _updateEffectOptions->uniformsNames
    = {"currentCount", "timeDelta",  "emitterWM",    "lifeTime",
       "color1",       "color2",     "sizeRange",    "scaleRange",
       "gravity",      "emitPower",  "direction1",   "direction2",
       "minEmitBox",   "maxEmitBox", "radius",       "directionRandomizer",
       "height",       "coneAngle",  "stopFactor",   "angleRange",
       "radiusRange",  "cellInfos",  "noiseStrength"};
  _updateEffectOptions->uniformBuffersNames = {};
  _updateEffectOptions->samplers
    = {"randomSampler",           "randomSampler2",
       "sizeGradientSampler",     "angularSpeedGradientSampler",
       "velocityGradientSampler", "noiseSampler"};
  _updateEffectOptions->defines                   = "";
  _updateEffectOptions->fallbacks                 = nullptr;
  _updateEffectOptions->onCompiled                = nullptr;
  _updateEffectOptions->onError                   = nullptr;
  _updateEffectOptions->indexParameters           = {};
  _updateEffectOptions->maxSimultaneousLights     = 0;
  _updateEffectOptions->transformFeedbackVaryings = {};

  particleEmitterType = ::std::make_unique<BoxParticleEmitter>();

  // Random data
  auto maxTextureSize
    = std::min(_engine->getCaps().maxTextureSize, *randomTextureSize);
  Float32Array d;
  for (int i = 0; i < maxTextureSize; ++i) {
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
  }
  _randomTexture = ::std::make_unique<RawTexture>(
    ArrayBufferView(d), maxTextureSize, 1, EngineConstants::TEXTUREFORMAT_RGBA,
    _scene, false, false, TextureConstants::NEAREST_SAMPLINGMODE,
    EngineConstants::TEXTURETYPE_FLOAT);
  _randomTexture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _randomTexture->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  d.clear();
  for (int i = 0; i < maxTextureSize; ++i) {
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
    d.emplace_back(Math::random());
  }
  _randomTexture2 = ::std::make_unique<RawTexture>(
    ArrayBufferView(d), maxTextureSize, 1, EngineConstants::TEXTUREFORMAT_RGBA,
    _scene, false, false, TextureConstants::NEAREST_SAMPLINGMODE,
    EngineConstants::TEXTURETYPE_FLOAT);
  _randomTexture2->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _randomTexture2->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  _randomTextureSize = maxTextureSize;
}

GPUParticleSystem::~GPUParticleSystem()
{
}

Vector3& GPUParticleSystem::get_direction1()
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    return boxParticleEmitter->direction1;
  }

  return _zeroVector3;
}

void GPUParticleSystem::set_direction1(const Vector3& value)
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    boxParticleEmitter->direction1 = value;
  }
}

Vector3& GPUParticleSystem::get_direction2()
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    return boxParticleEmitter->direction2;
  }

  return _zeroVector3;
}

void GPUParticleSystem::set_direction2(const Vector3& value)
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    boxParticleEmitter->direction2 = value;
  }
}

Vector3& GPUParticleSystem::get_minEmitBox()
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    return boxParticleEmitter->minEmitBox;
  }

  return _zeroVector3;
}

void GPUParticleSystem::set_minEmitBox(const Vector3& value)
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    boxParticleEmitter->minEmitBox = value;
  }
}

Vector3& GPUParticleSystem::get_maxEmitBox()
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    return boxParticleEmitter->maxEmitBox;
  }

  return _zeroVector3;
}

void GPUParticleSystem::set_maxEmitBox(const Vector3& value)
{
  if (auto boxParticleEmitter
      = static_cast<BoxParticleEmitter*>(particleEmitterType.get())) {
    boxParticleEmitter->maxEmitBox = value;
  }
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

bool GPUParticleSystem::get_isAnimationSheetEnabled() const
{
  return _isAnimationSheetEnabled;
}

bool GPUParticleSystem::get_isBillboardBased() const
{
  return _isBillboardBased;
}

void GPUParticleSystem::set_isBillboardBased(bool value)
{
  if (_isBillboardBased == value) {
    return;
  }

  _isBillboardBased = value;

  _releaseBuffers();
}

bool GPUParticleSystem::isReady()
{
  if (!_updateEffect) {
    _recreateUpdateEffect();
    _recreateRenderEffect();
    return false;
  }

  if (/*!emitter ||*/ !_updateEffect->isReady() || !_renderEffect->isReady()
      || !particleTexture || !particleTexture->isReady()) {
    return false;
  }

  return true;
}

bool GPUParticleSystem::isStarted() const
{
  return _started;
}

void GPUParticleSystem::start(size_t delay)
{
  if (delay > 0) {
    // Timeout
  }
  _started     = true;
  _stopped     = false;
  _preWarmDone = false;
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

const char* GPUParticleSystem::getClassName() const
{
  return "GPUParticleSystem";
}

vector_t<ColorGradient>& GPUParticleSystem::getColorGradients()
{
  return _colorGradients;
}

vector_t<FactorGradient>& GPUParticleSystem::getSizeGradients()
{
  return _sizeGradients;
}

vector_t<FactorGradient>& GPUParticleSystem::getAngularSpeedGradients()
{
  return _angularSpeedGradients;
}

vector_t<FactorGradient>& GPUParticleSystem::getVelocityGradients()
{
  return _velocityGradients;
}

template <typename T>
GPUParticleSystem& GPUParticleSystem::_removeGradient(float gradient,
                                                      vector_t<T>& gradients,
                                                      RawTexture* texture)
{
  if (gradients.empty()) {
    return *this;
  }

  gradients.erase(
    ::std::remove_if(gradients.begin(), gradients.end(),
                     [&gradient](const IValueGradient& valueGradient) {
                       return stl_util::almost_equal(valueGradient.gradient,
                                                     gradient);
                     }),
    gradients.end());

  if (texture) {
    texture->dispose();
  }

  _releaseBuffers();

  return *this;
}

GPUParticleSystem&
GPUParticleSystem::addColorGradient(float gradient, const Color4& color1,
                                    const nullable_t<Color4>& /*color2*/)
{
  ColorGradient colorGradient;
  colorGradient.gradient = gradient;
  colorGradient.color1   = color1;
  _colorGradients.emplace_back(colorGradient);

  ::std::sort(_colorGradients.begin(), _colorGradients.end(),
              [](const ColorGradient& a, const ColorGradient& b) {
                if (a.gradient < b.gradient) {
                  return -1;
                }
                else if (a.gradient > b.gradient) {
                  return 1;
                }

                return 0;
              });

  if (_colorGradientsTexture) {
    _colorGradientsTexture->dispose();
    _colorGradientsTexture = nullptr;
  }

  _releaseBuffers();

  return *this;
}

GPUParticleSystem& GPUParticleSystem::removeColorGradient(float gradient)
{
  _removeGradient<ColorGradient>(gradient, _colorGradients,
                                 _colorGradientsTexture.get());
  _colorGradientsTexture = nullptr;

  return *this;
}

void GPUParticleSystem::_addFactorGradient(
  vector_t<FactorGradient>& factorGradients, float gradient, float factor)
{
  FactorGradient valueGradient;
  valueGradient.gradient = gradient;
  valueGradient.factor1  = factor;
  factorGradients.emplace_back(valueGradient);

  ::std::sort(factorGradients.begin(), factorGradients.end(),
              [](const FactorGradient& a, const FactorGradient& b) {
                if (a.gradient < b.gradient) {
                  return -1;
                }
                else if (a.gradient > b.gradient) {
                  return 1;
                }

                return 0;
              });

  _releaseBuffers();
}

GPUParticleSystem&
GPUParticleSystem::addSizeGradient(float gradient, float factor,
                                   const nullable_t<float>& /*factor2*/)
{
  _addFactorGradient(_sizeGradients, gradient, factor);

  if (_sizeGradientsTexture) {
    _sizeGradientsTexture->dispose();
    _sizeGradientsTexture = nullptr;
  }

  _releaseBuffers();

  return *this;
}

GPUParticleSystem& GPUParticleSystem::removeSizeGradient(float gradient)
{
  _removeGradient(gradient, _sizeGradients, _sizeGradientsTexture.get());
  _sizeGradientsTexture = nullptr;

  return *this;
}

GPUParticleSystem&
GPUParticleSystem::addAngularSpeedGradient(float gradient, float factor,
                                           const nullable_t<float>& /*factor2*/)
{
  _addFactorGradient(_angularSpeedGradients, gradient, factor);

  if (_angularSpeedGradientsTexture) {
    _angularSpeedGradientsTexture->dispose();
    _angularSpeedGradientsTexture = nullptr;
  }

  _releaseBuffers();

  return *this;
}

GPUParticleSystem& GPUParticleSystem::removeAngularSpeedGradient(float gradient)
{
  _removeGradient(gradient, _angularSpeedGradients,
                  _angularSpeedGradientsTexture.get());
  _angularSpeedGradientsTexture = nullptr;

  return *this;
}

GPUParticleSystem&
GPUParticleSystem::addVelocityGradient(float gradient, float factor,
                                       const nullable_t<float>& /*factor2*/)
{
  _addFactorGradient(_velocityGradients, gradient, factor);

  if (_velocityGradientsTexture) {
    _velocityGradientsTexture->dispose();
    _velocityGradientsTexture = nullptr;
  }

  _releaseBuffers();

  return *this;
}

GPUParticleSystem& GPUParticleSystem::removeVelocityGradient(float gradient)
{
  _removeGradient(gradient, _velocityGradients,
                  _velocityGradientsTexture.get());
  _velocityGradientsTexture = nullptr;

  return *this;
}

unique_ptr_t<GL::IGLVertexArrayObject>
GPUParticleSystem::_createUpdateVAO(Buffer* source)
{
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> updateVertexBuffers;
  updateVertexBuffers["position"]
    = source->createVertexBuffer(VertexBuffer::PositionKind, 0, 3);
  updateVertexBuffers["age"]
    = source->createVertexBuffer(VertexBuffer::AgeKind, 3, 1);
  updateVertexBuffers["life"]
    = source->createVertexBuffer(VertexBuffer::LifeKind, 4, 1);
  updateVertexBuffers["seed"]
    = source->createVertexBuffer(VertexBuffer::SeedKind, 5, 1);
  updateVertexBuffers["size"]
    = source->createVertexBuffer(VertexBuffer::SizeKind, 9, 3);
  size_t offset = 12;

  if (!_colorGradientsTexture) {
    updateVertexBuffers["color"]
      = source->createVertexBuffer(VertexBuffer::ColorKind, offset, 4);
    offset += 4;
  }

  updateVertexBuffers["direction"]
    = source->createVertexBuffer(VertexBuffer::DirectionKind, offset, 3);
  offset += 3;

  if (!_isBillboardBased) {
    updateVertexBuffers["initialDirection"] = source->createVertexBuffer(
      VertexBuffer::InitialDirectionKind, offset, 3);
    offset += 3;
  }

  if (_angularSpeedGradientsTexture) {
    updateVertexBuffers["angle"]
      = source->createVertexBuffer(VertexBuffer::AngleKind, offset, 1);
    offset += 1;
  }
  else {
    updateVertexBuffers["angle"]
      = source->createVertexBuffer(VertexBuffer::AngleKind, offset, 2);
    offset += 2;
  }

  if (_isAnimationSheetEnabled) {
    updateVertexBuffers["cellIndex"]
      = source->createVertexBuffer(VertexBuffer::CellIndexKind, offset, 1);
    offset += 1;
  }

  auto vao = _engine->recordVertexArrayObject(
    stl_util::to_raw_ptr_map(updateVertexBuffers), nullptr,
    _updateEffect.get());
  _engine->bindArrayBuffer(nullptr);

  return vao;
}

unique_ptr_t<GL::IGLVertexArrayObject>
GPUParticleSystem::_createRenderVAO(Buffer* source, Buffer* spriteSource)
{
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> renderVertexBuffers;
  auto attributesStrideSizeT      = static_cast<size_t>(_attributesStrideSize);
  renderVertexBuffers["position"] = source->createVertexBuffer(
    VertexBuffer::PositionKind, 0, 3, attributesStrideSizeT, true);
  renderVertexBuffers["age"] = source->createVertexBuffer(
    VertexBuffer::AgeKind, 3, 1, attributesStrideSizeT, true);
  renderVertexBuffers["life"] = source->createVertexBuffer(
    VertexBuffer::LifeKind, 4, 1, attributesStrideSizeT, true);
  renderVertexBuffers["size"] = source->createVertexBuffer(
    VertexBuffer::SizeKind, 9, 3, attributesStrideSizeT, true);

  size_t offset = 12;

  if (!_colorGradientsTexture) {
    renderVertexBuffers["color"] = source->createVertexBuffer(
      VertexBuffer::ColorKind, offset, 4, attributesStrideSizeT, true);
    offset += 4;
  }

  offset += 3; // Direction

  if (!_isBillboardBased) {
    renderVertexBuffers["initialDirection"]
      = source->createVertexBuffer(VertexBuffer::InitialDirectionKind, offset,
                                   3, attributesStrideSizeT, true);
    offset += 3;
  }

  renderVertexBuffers["angle"] = source->createVertexBuffer(
    VertexBuffer::AngleKind, offset, 1, attributesStrideSizeT, true);
  if (_angularSpeedGradientsTexture) {
    offset++;
  }
  else {
    offset += 2;
  }

  if (_isAnimationSheetEnabled) {
    renderVertexBuffers["cellIndex"] = source->createVertexBuffer(
      VertexBuffer::CellIndexKind, attributesStrideSizeT, 1,
      _attributesStrideSize, true);
    offset += 1;
  }

  renderVertexBuffers["offset"]
    = spriteSource->createVertexBuffer(VertexBuffer::OffsetKind, 0, 2);
  renderVertexBuffers["uv"]
    = spriteSource->createVertexBuffer(VertexBuffer::UVKind, 2, 2);

  auto vao = _engine->recordVertexArrayObject(
    stl_util::to_raw_ptr_map(renderVertexBuffers), nullptr,
    _renderEffect.get());
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
  }

  for (size_t particleIndex = 0; particleIndex < _capacity; ++particleIndex) {
    // position
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);

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
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);

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
  _buffer0
    = ::std::make_unique<Buffer>(engine, data, false, _attributesStrideSize);
  _buffer1
    = ::std::make_unique<Buffer>(engine, data, false, _attributesStrideSize);
  _spriteBuffer = ::std::make_unique<Buffer>(engine, spriteData, false, 4);

  // Update VAO
  _updateVAO.clear();
  _updateVAO.emplace_back(_createUpdateVAO(_buffer0.get()));
  _updateVAO.emplace_back(_createUpdateVAO(_buffer1.get()));

  // Render VAO
  _renderVAO.clear();
  _renderVAO.emplace_back(
    _createRenderVAO(_buffer1.get(), _spriteBuffer.get()));
  _renderVAO.emplace_back(
    _createRenderVAO(_buffer0.get(), _spriteBuffer.get()));

  // Links
  _sourceBuffer = _buffer0.get();
  _targetBuffer = _buffer1.get();
}

void GPUParticleSystem::_recreateUpdateEffect()
{
  ostringstream_t definesStream;
  definesStream << (particleEmitterType ?
                      particleEmitterType->getEffectDefines() :
                      "");

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

  if (isAnimationSheetEnabled) {
    definesStream << "\n#define ANIMATESHEET";
  }

  if (noiseTexture) {
    definesStream << "\n#define NOISE";
  }

  auto defines = definesStream.str();

  if (_updateEffect && _updateEffectOptions->defines == defines) {
    return;
  }

  _updateEffectOptions->transformFeedbackVaryings
    = {"outPosition", "outAge", "outLife", "outSeed", "outSize"};

  if (!_colorGradientsTexture) {
    _updateEffectOptions->transformFeedbackVaryings.emplace_back("outColor");
  }

  _updateEffectOptions->transformFeedbackVaryings.emplace_back("outDirection");

  if (!_isBillboardBased) {
    _updateEffectOptions->transformFeedbackVaryings.emplace_back(
      "outInitialDirection");
  }

  _updateEffectOptions->transformFeedbackVaryings.emplace_back("outAngle");

  if (isAnimationSheetEnabled) {
    _updateEffectOptions->transformFeedbackVaryings.emplace_back(
      "outCellIndex");
  }

  _updateEffectOptions->defines = ::std::move(defines);
  _updateEffect                 = ::std::make_unique<Effect>(
    "gpuUpdateParticles", *_updateEffectOptions, _scene->getEngine());
}

void GPUParticleSystem::_recreateRenderEffect()
{
  ostringstream_t definesStream;
  if (_scene->clipPlane()) {
    definesStream << "\n#define CLIPPLANE";
  }

  if (_isBillboardBased) {
    definesStream << "\n#define BILLBOARD";

    switch (billboardMode) {
      case AbstractMesh::BILLBOARDMODE_Y:
        definesStream << "\n#define BILLBOARDY";
        break;
      case AbstractMesh::BILLBOARDMODE_ALL:
      default:
        break;
    }
  }

  if (_colorGradientsTexture) {
    definesStream << "\n#define COLORGRADIENTS";
  }

  if (isAnimationSheetEnabled) {
    definesStream << "\n#define ANIMATESHEET";
  }

  auto defines = definesStream.str();

  if (_renderEffect && _renderEffect->defines == defines) {
    return;
  }

  EffectCreationOptions renderEffectOptions;
  renderEffectOptions.attributes
    = {"position",         "age",   "life",     "size", "color", "offset", "uv",
       "initialDirection", "angle", "cellIndex"};
  renderEffectOptions.uniformsNames
    = {"view",       "projection", "colorDead",        "invView",
       "vClipPlane", "sheetInfos", "translationPivot", "eyePosition"};
  renderEffectOptions.samplers = {"textureSampler", "colorGradientSampler"};
  renderEffectOptions.defines  = ::std::move(defines);

  _renderEffect = ::std::make_unique<Effect>(
    "gpuRenderParticles", renderEffectOptions, _scene->getEngine());
}

void GPUParticleSystem::animate(bool preWarm)
{
  _timeDelta
    = updateSpeed * (preWarm ? preWarmStepOffset : _scene->getAnimationRatio());
  _actualFrame += static_cast<int>(_timeDelta);

  if (!_stopped) {
    if (targetStopDuration && _actualFrame >= targetStopDuration) {
      stop();
    }
  }
}

RawTexture* GPUParticleSystem::_getRawTextureByName(const string_t& textureName)
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

void GPUParticleSystem::_setRawTextureByName(
  const string_t& textureName, unique_ptr_t<RawTexture>&& rawTexture)
{
  if (textureName == "_colorGradientsTexture") {
    _colorGradientsTexture = ::std::move(rawTexture);
  }
  else if (textureName == "_angularSpeedGradientsTexture") {
    _angularSpeedGradientsTexture = ::std::move(rawTexture);
  }
  else if (textureName == "_sizeGradientsTexture") {
    _sizeGradientsTexture = ::std::move(rawTexture);
  }
  else if (textureName == "_velocityGradientsTexture") {
    _velocityGradientsTexture = ::std::move(rawTexture);
  }
}

template <typename T>
void GPUParticleSystem::_createFactorGradientTexture(
  const vector_t<T>& factorGradients, const string_t& textureName)
{
  auto texture = _getRawTextureByName(textureName);

  if (factorGradients.empty() || texture) {
    return;
  }

  Float32Array data(_rawTextureWidth);

  for (unsigned int x = 0; x < _rawTextureWidth; x++) {
    auto ratio = static_cast<float>(x) / static_cast<float>(_rawTextureWidth);

    Tools::GetCurrentGradient<FactorGradient>(
      ratio, factorGradients,
      [&](FactorGradient& currentGradient, FactorGradient& nextGradient,
          float scale) {
        data[x]
          = Scalar::Lerp(currentGradient.factor1, nextGradient.factor1, scale);
      });
  }

  _setRawTextureByName(textureName,
                       RawTexture::CreateRTexture(
                         data, static_cast<int>(_rawTextureWidth), 1, _scene,
                         false, false, TextureConstants::NEAREST_SAMPLINGMODE));
}

void GPUParticleSystem::_createSizeGradientTexture()
{
  _createFactorGradientTexture(_sizeGradients, "_sizeGradientsTexture");
}

void GPUParticleSystem::_createAngularSpeedGradientTexture()
{
  _createFactorGradientTexture(_angularSpeedGradients,
                               "_angularSpeedGradientsTexture");
}

void GPUParticleSystem::_createVelocityGradientTexture()
{
  _createFactorGradientTexture(_velocityGradients, "_velocityGradientsTexture");
}

void GPUParticleSystem::_createColorGradientTexture()
{
  if (_colorGradients.empty() || _colorGradientsTexture) {
    return;
  }

  Uint8Array data(_rawTextureWidth * 4);
  auto& tmpColor = Tmp::Color4Array[0];

  for (unsigned int x = 0; x < _rawTextureWidth; ++x) {
    auto ratio = static_cast<float>(x) / static_cast<float>(_rawTextureWidth);

    Tools::GetCurrentGradient<ColorGradient>(
      ratio, _colorGradients,
      [&](ColorGradient& currentGradient, ColorGradient& nextGradient,
          float scale) {
        Color4::LerpToRef(currentGradient.color1, nextGradient.color1, scale,
                          tmpColor);
        data[x * 4]     = static_cast<uint8_t>(tmpColor.r * 255);
        data[x * 4 + 1] = static_cast<uint8_t>(tmpColor.g * 255);
        data[x * 4 + 2] = static_cast<uint8_t>(tmpColor.b * 255);
        data[x * 4 + 3] = static_cast<uint8_t>(tmpColor.a * 255);
      });
  }

  _colorGradientsTexture = RawTexture::CreateRGBATexture(
    data, static_cast<int>(_rawTextureWidth), 1, _scene, false, false,
    TextureConstants::NEAREST_SAMPLINGMODE);
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

    if (_currentRenderId == _scene->getRenderId()) {
      return 0;
    }

    _currentRenderId = _scene->getRenderId();
  }

  // Get everything ready to render
  _initialize();

  _accumulatedCount += emitRate * _timeDelta;
  if (_accumulatedCount > 1) {
    auto intPart = _accumulatedCount;
    _accumulatedCount -= intPart;
    _currentActiveCount
      = ::std::min(_activeCount, _currentActiveCount + intPart);
  }

  if (!_currentActiveCount) {
    return 0;
  }

  // Enable update effect
  _engine->enableEffect(_updateEffect.get());
  _engine->setState(false);

  _updateEffect->setFloat("currentCount", _currentActiveCount);
  _updateEffect->setFloat("timeDelta", _timeDelta);
  _updateEffect->setFloat("stopFactor", _stopped ? 0.f : 1.f);
  _updateEffect->setTexture("randomSampler", _randomTexture.get());
  _updateEffect->setTexture("randomSampler2", _randomTexture2.get());
  _updateEffect->setFloat2("lifeTime", minLifeTime, maxLifeTime);
  _updateEffect->setFloat2("emitPower", minEmitPower, maxEmitPower);
  if (!_colorGradientsTexture) {
    _updateEffect->setDirectColor4("color1", color1);
    _updateEffect->setDirectColor4("color2", color2);
  }
  _updateEffect->setFloat2("sizeRange", minSize, maxSize);
  _updateEffect->setFloat4("scaleRange", minScaleX, maxScaleX, minScaleY,
                           maxScaleY);
  _updateEffect->setFloat4("angleRange", minAngularSpeed, maxAngularSpeed,
                           minInitialRotation, maxInitialRotation);
  _updateEffect->setVector3("gravity", gravity);

  if (_sizeGradientsTexture) {
    _updateEffect->setTexture("sizeGradientSampler",
                              _sizeGradientsTexture.get());
  }

  if (_angularSpeedGradientsTexture) {
    _updateEffect->setTexture("angularSpeedGradientSampler",
                              _angularSpeedGradientsTexture.get());
  }

  if (_velocityGradientsTexture) {
    _updateEffect->setTexture("velocityGradientSampler",
                              _velocityGradientsTexture.get());
  }

  if (particleEmitterType) {
    particleEmitterType->applyToShader(_updateEffect.get());
  }
  if (_isAnimationSheetEnabled) {
    _updateEffect->setFloat3("cellInfos", startSpriteCellID, endSpriteCellID,
                             spriteCellChangeSpeed);
  }

  if (noiseTexture) {
    _updateEffect->setTexture("noiseSampler", noiseTexture);
    _updateEffect->setVector3("noiseStrength", noiseStrength);
  }

  Matrix emitterWM;
  if (emitter.is<AbstractMesh*>()) {
    auto emitterMesh = emitter.get<AbstractMesh*>();
    emitterWM        = *emitterMesh->getWorldMatrix();
  }
  else {
    auto emitterPosition = emitter.get<Vector3>();
    emitterWM = Matrix::Translation(emitterPosition.x, emitterPosition.y,
                                    emitterPosition.z);
  }
  _updateEffect->setMatrix("emitterWM", emitterWM);

  // Bind source VAO
  _engine->bindVertexArrayObject(_updateVAO[_targetIndex].get(), nullptr);

  // Update
  _engine->bindTransformFeedbackBuffer(_targetBuffer->getBuffer());
  _engine->setRasterizerState(false);
  _engine->beginTransformFeedback();
  _engine->drawArraysType(Material::PointListDrawMode(), 0,
                          static_cast<int>(_currentActiveCount));
  _engine->endTransformFeedback();
  _engine->setRasterizerState(true);
  _engine->bindTransformFeedbackBuffer(nullptr);

  if (!preWarm) {
    // Enable render effect
    _engine->enableEffect(_renderEffect.get());
    auto viewMatrix = _scene->getViewMatrix();
    _renderEffect->setMatrix("view", viewMatrix);
    _renderEffect->setMatrix("projection", _scene->getProjectionMatrix());
    _renderEffect->setTexture("textureSampler", particleTexture);
    _renderEffect->setVector2("translationPivot", translationPivot);
    if (_colorGradientsTexture) {
      _renderEffect->setTexture("colorGradientSampler",
                                _colorGradientsTexture.get());
    }
    else {
      _renderEffect->setDirectColor4("colorDead", colorDead);
    }

    if (_isAnimationSheetEnabled && particleTexture) {
      auto baseSize = particleTexture->getBaseSize();
      _renderEffect->setFloat3("sheetInfos",
                               static_cast<float>(spriteCellWidth)
                                 / static_cast<float>(baseSize.width),
                               static_cast<float>(spriteCellHeight)
                                 / static_cast<float>(baseSize.height),
                               static_cast<float>(baseSize.width)
                                 / static_cast<float>(spriteCellWidth));
    }

    if (_isBillboardBased) {
      auto camera = _scene->activeCamera;
      _renderEffect->setVector3("eyePosition", camera->globalPosition);
    }

    if (_scene->clipPlane()) {
      auto clipPlane = _scene->clipPlane();
      auto invView   = viewMatrix;
      invView.invert();
      _renderEffect->setMatrix("invView", invView);
      _renderEffect->setFloat4("vClipPlane", clipPlane->normal.x,
                               clipPlane->normal.y, clipPlane->normal.z,
                               clipPlane->d);
    }

    // Draw order
    switch (blendMode) {
      case ParticleSystem::BLENDMODE_ADD:
        _engine->setAlphaMode(EngineConstants::ALPHA_ADD);
        break;
      case ParticleSystem::BLENDMODE_ONEONE:
        _engine->setAlphaMode(EngineConstants::ALPHA_ONEONE);
        break;
      case ParticleSystem::BLENDMODE_STANDARD:
        _engine->setAlphaMode(EngineConstants::ALPHA_COMBINE);
        break;
    }

    if (forceDepthWrite) {
      _engine->setDepthWrite(true);
    }

    // Bind source VAO
    _engine->bindVertexArrayObject(_renderVAO[_targetIndex].get(), nullptr);

    // Render
    _engine->drawArraysType(Material::TriangleFanDrawMode(), 0, 4,
                            static_cast<int>(_currentActiveCount));
    _engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);
  }

  // Switch VAOs
  ++_targetIndex;
  if (_targetIndex == 2) {
    _targetIndex = 0;
  }

  // Switch buffers
  ::std::swap(_sourceBuffer, _targetBuffer);

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
    _engine->releaseVertexArrayObject(_updateVAOItem.get());
  }
  _updateVAO.clear();

  for (auto& _renderVAOItem : _renderVAO) {
    _engine->releaseVertexArrayObject(_renderVAOItem.get());
  }
  _renderVAO.clear();
}

void GPUParticleSystem::dispose(bool disposeTexture,
                                bool /*disposeMaterialAndTextures*/)
{
  // Remove from scene
  _scene->particleSystems.erase(
    ::std::remove_if(
      _scene->particleSystems.begin(), _scene->particleSystems.end(),
      [this](const unique_ptr_t<IParticleSystem>& particleSystem) {
        return particleSystem.get() == this;
      }),
    _scene->particleSystems.end());

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

  if (disposeTexture && noiseTexture) {
    noiseTexture->dispose();
    noiseTexture = nullptr;
  }

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

IParticleSystem* GPUParticleSystem::clone(const string_t& /*name*/,
                                          Mesh* /*newEmitter*/)
{
  return nullptr;
}

Json::object GPUParticleSystem::serialize() const
{
  return Json::object();
}

unique_ptr_t<GPUParticleSystem>
GPUParticleSystem::Parse(const Json::value& /*parsedParticleSystem*/,
                         Scene* /*scene*/, const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
