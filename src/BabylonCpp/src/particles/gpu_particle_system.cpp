#include <babylon/particles/gpu_particle_system.h>

#include <babylon/core/json.h>
#include <babylon/core/random.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/particles/emittertypes/box_particle_emitter.h>
#include <babylon/particles/emittertypes/iparticle_emitter_Type.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

bool GPUParticleSystem::IsSupported()
{
  if (!Engine::LastCreatedEngine()) {
    return false;
  }
  return Engine::LastCreatedEngine()->webGLVersion() > 1.f;
}

GPUParticleSystem::GPUParticleSystem(const string_t& iName, size_t capacity,
                                     Nullable<int> randomTextureSize,
                                     Scene* scene)
    : minAngularSpeed{0}
    , maxAngularSpeed{0}
    , direction1{this, &GPUParticleSystem::get_direction1,
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
    , _attributesStrideSize{16}
    , _actualFrame{0}
    , _zeroVector3{Vector3::Zero()}
{
  // IParticleSystem
  {
    id                  = iName;
    name                = iName;
    renderingGroupId    = 0;
    layerMask           = 0x0FFFFFFF;
    updateSpeed         = 0.01f;
    targetStopDuration  = 0;
    particleTexture     = nullptr;
    blendMode           = ParticleSystem::BLENDMODE_ONEONE;
    minLifeTime         = 1.f;
    maxLifeTime         = 1.f;
    minSize             = 1.f;
    maxSize             = 1.f;
    color1              = Color4{1.f, 1.f, 1.f, 1.f};
    color2              = Color4{1.f, 1.f, 1.f, 1.f};
    colorDead           = Color4{0.f, 0.f, 0.f, 0.f};
    emitRate            = 100;
    gravity             = Vector3::Zero();
    minEmitPower        = 1.f;
    maxEmitPower        = 1.f;
    particleEmitterType = nullptr;
  }

  _scene  = scene ? scene : Engine::LastCreatedScene();
  _engine = _scene->getEngine();

  if (randomTextureSize.isNull()) {
    randomTextureSize = _engine->getCaps().maxTextureSize;
  }

  _capacity           = capacity;
  _activeCount        = capacity;
  _currentActiveCount = 0;

  scene->particleSystems.emplace_back(this);

  _updateEffectOptions->attributes = {
    "position", "age", "life", "seed", "size", "color", "direction", "angle"};
  _updateEffectOptions->uniformsNames
    = {"currentCount", "timeDelta",  "generalRandoms", "emitterWM",
       "lifeTime",     "color1",     "color2",         "sizeRange",
       "gravity",      "emitPower",  "direction1",     "direction2",
       "minEmitBox",   "maxEmitBox", "radius",         "directionRandomizer",
       "height",       "coneAngle",  "stopFactor",     "angleRange"};
  _updateEffectOptions->uniformBuffersNames   = {};
  _updateEffectOptions->samplers              = {"randomSampler"};
  _updateEffectOptions->defines               = "";
  _updateEffectOptions->fallbacks             = nullptr;
  _updateEffectOptions->onCompiled            = nullptr;
  _updateEffectOptions->onError               = nullptr;
  _updateEffectOptions->indexParameters       = {};
  _updateEffectOptions->maxSimultaneousLights = 0;
  _updateEffectOptions->transformFeedbackVaryings
    = {"outPosition", "outAge",   "outLife",      "outSeed",
       "outSize",     "outColor", "outDirection", "outAngle"};

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

  _randomTextureSize  = maxTextureSize;
  particleEmitterType = ::std::make_unique<BoxParticleEmitter>();
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

void GPUParticleSystem::start()
{
  _started = true;
  _stopped = false;
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
    = source->createVertexBuffer(VertexBuffer::SizeKind, 6, 1);
  updateVertexBuffers["color"]
    = source->createVertexBuffer(VertexBuffer::ColorKind, 7, 4);
  updateVertexBuffers["direction"]
    = source->createVertexBuffer(VertexBuffer::DirectionKind, 11, 3);
  updateVertexBuffers["angle"]
    = source->createVertexBuffer(VertexBuffer::AngleKind, 14, 2);

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
    VertexBuffer::SizeKind, 6, 1, attributesStrideSizeT, true);
  renderVertexBuffers["color"] = source->createVertexBuffer(
    VertexBuffer::ColorKind, 7, 4, attributesStrideSizeT, true);
  renderVertexBuffers["angle"] = source->createVertexBuffer(
    VertexBuffer::AngleKind, 14, 2, attributesStrideSizeT, true);

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
  for (size_t particleIndex = 0; particleIndex < _capacity; ++particleIndex) {
    // position
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);

    // Age and life
    data.emplace_back(
      0.f); // create the particle as a dead one to create a new one at start
    data.emplace_back(0.f);

    // Seed
    data.emplace_back(Math::random());

    // Size
    data.emplace_back(0.f);

    // color
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);

    // direction
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);

    // angle
    data.emplace_back(0.f);
    data.emplace_back(0.f);
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
  auto defines
    = particleEmitterType ? particleEmitterType->getEffectDefines() : "";
  if (_updateEffect && _updateEffectOptions->defines == defines) {
    return;
  }
  _updateEffectOptions->defines = defines;
  _updateEffect                 = ::std::make_unique<Effect>(
    "gpuUpdateParticles", *_updateEffectOptions, _scene->getEngine());
}

void GPUParticleSystem::_recreateRenderEffect()
{
  string_t defines = "";
  if (_scene->clipPlane()) {
    defines = "\n#define CLIPPLANE";
  }

  if (_renderEffect && _renderEffect->defines == defines) {
    return;
  }

  EffectCreationOptions renderEffectOptions;
  renderEffectOptions.attributes
    = {"position", "age", "life", "size", "color", "offset", "uv", "angle"};
  renderEffectOptions.uniformsNames
    = {"view", "projection", "colorDead", "invView", "vClipPlane"};
  renderEffectOptions.samplers = {"textureSampler"};
  renderEffectOptions.defines  = ::std::move(defines);

  _renderEffect = ::std::make_unique<Effect>(
    "gpuRenderParticles", renderEffectOptions, _scene->getEngine());
}

void GPUParticleSystem::animate()
{
  _timeDelta = updateSpeed * _scene->getAnimationRatio();
  _actualFrame += static_cast<int>(_timeDelta);

  if (!_stopped) {
    if (targetStopDuration && _actualFrame >= targetStopDuration) {
      stop();
    }
  }
}

size_t GPUParticleSystem::render()
{
  if (!_started) {
    return 0;
  }

  _recreateUpdateEffect();
  _recreateRenderEffect();

  if (!isReady()) {
    return 0;
  }

  if (_currentRenderId == _scene->getRenderId()) {
    return 0;
  }

  _currentRenderId = _scene->getRenderId();

  // Get everything ready to render
  _initialize();

  _currentActiveCount
    = ::std::min(_activeCount, _currentActiveCount
                                 + static_cast<size_t>(emitRate * _timeDelta));

  // Enable update effect

  _engine->enableEffect(_updateEffect.get());
  _engine->setState(false);

  _updateEffect->setFloat("currentCount", _currentActiveCount);
  _updateEffect->setFloat("timeDelta", _timeDelta);
  _updateEffect->setFloat("stopFactor", _stopped ? 0 : 1);
  _updateEffect->setFloat3("generalRandoms", Math::random(), Math::random(),
                           Math::random());
  _updateEffect->setTexture("randomSampler", _randomTexture.get());
  _updateEffect->setFloat2("lifeTime", minLifeTime, maxLifeTime);
  _updateEffect->setFloat2("emitPower", minEmitPower, maxEmitPower);
  _updateEffect->setDirectColor4("color1", color1);
  _updateEffect->setDirectColor4("color2", color2);
  _updateEffect->setFloat2("sizeRange", minSize, maxSize);
  _updateEffect->setFloat2("angleRange", minAngularSpeed, maxAngularSpeed);
  _updateEffect->setVector3("gravity", gravity);

  if (particleEmitterType) {
    particleEmitterType->applyToShader(_updateEffect.get());
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

  // Enable render effect
  _engine->enableEffect(_renderEffect.get());
  auto viewMatrix = _scene->getViewMatrix();
  _renderEffect->setMatrix("view", viewMatrix);
  _renderEffect->setMatrix("projection", _scene->getProjectionMatrix());
  _renderEffect->setTexture("textureSampler", particleTexture);
  _renderEffect->setDirectColor4("colorDead", colorDead);

  if (_scene->clipPlane()) {
    auto clipPlane = *_scene->clipPlane();
    auto invView   = viewMatrix;
    invView.invert();
    _renderEffect->setMatrix("invView", invView);
    _renderEffect->setFloat4("vClipPlane", clipPlane.normal.x,
                             clipPlane.normal.y, clipPlane.normal.z,
                             clipPlane.d);
  }

  // Draw order
  if (blendMode == ParticleSystem::BLENDMODE_ONEONE) {
    _engine->setAlphaMode(EngineConstants::ALPHA_ONEONE);
  }
  else {
    _engine->setAlphaMode(EngineConstants::ALPHA_COMBINE);
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

  if (_randomTexture) {
    _randomTexture->dispose();
    _randomTexture = nullptr;
  }

  if (disposeTexture && particleTexture) {
    particleTexture->dispose();
    particleTexture = nullptr;
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
