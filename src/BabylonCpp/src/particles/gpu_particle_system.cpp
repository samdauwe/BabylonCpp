#include <babylon/particles/gpu_particle_system.h>

#include <babylon/core/json.h>
#include <babylon/core/random.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

GPUParticleSystem::GPUParticleSystem(const string_t& iName, size_t capacity,
                                     Scene* scene)
{
  id        = iName;
  name      = iName;
  _scene    = scene ? scene : Engine::LastCreatedScene();
  _capacity = capacity;
  _engine   = _scene->getEngine();

  renderingGroupId = 0;
  layerMask        = 0x0FFFFFFF;

  _currentRenderId = -1;
  _started         = true;

  scene->particleSystems.emplace_back(this);

  EffectCreationOptions renderEffectOptions;
  renderEffectOptions.attributes = {"position", "age", "life", "velocity"};

  _renderEffect = ::std::make_unique<Effect>(
    "gpuRenderParticles", renderEffectOptions, _scene->getEngine());

  EffectCreationOptions updateEffectOptions;
  updateEffectOptions.attributes    = {"position", "age", "life", "velocity"};
  updateEffectOptions.uniformsNames = {};
  updateEffectOptions.uniformBuffersNames   = {};
  updateEffectOptions.samplers              = {};
  updateEffectOptions.defines               = "";
  updateEffectOptions.fallbacks             = nullptr;
  updateEffectOptions.onCompiled            = nullptr;
  updateEffectOptions.onError               = nullptr;
  updateEffectOptions.indexParameters       = {};
  updateEffectOptions.maxSimultaneousLights = 0;
  updateEffectOptions.transformFeedbackVaryings
    = {"outPosition", "outAge", "outLife", "outVelocity"};

  _updateEffect = ::std::make_unique<Effect>(
    "gpuUpdateParticles", updateEffectOptions, _scene->getEngine());
}

GPUParticleSystem::~GPUParticleSystem()
{
}

bool GPUParticleSystem::isStarted() const
{
  return _started;
}

void GPUParticleSystem::start()
{
  _started = true;
}

void GPUParticleSystem::stop()
{
  _started = false;
}

void GPUParticleSystem::animate()
{
}

void GPUParticleSystem::_initialize()
{
  if (_renderVAO) {
    return;
  }

  Float32Array data;
  for (size_t particleIndex = 0; particleIndex < _capacity; ++particleIndex) {
    // position
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);

    float life = 1.f + Math::random() * 10.f; // TODO: var
    // create the particle as a dead one to create a new one at start
    data.emplace_back(life + 1.f);
    data.emplace_back(life);

    // velocity
    data.emplace_back(0.f);
    data.emplace_back(0.f);
    data.emplace_back(0.f);
  }

  // Conversion lambda
  const auto to_raw_ptr_map
    = [](const unordered_map_t<string_t, unique_ptr_t<VertexBuffer>>& c) {
        unordered_map_t<string_t, VertexBuffer*> res;
        for (auto& kv : c) {
          res[kv.first] = kv.second.get();
        };
        return res;
      };

  // Update VAO
  _updateBuffer
    = ::std::make_unique<Buffer>(_scene->getEngine(), data, false, 0);
  _updateVertexBuffers[VertexBuffer::PositionKindChars]
    = _updateBuffer->createVertexBuffer(VertexBuffer::PositionKind, 0, 3, 3);
  _updateVertexBuffers[VertexBuffer::AgeKindChars]
    = _updateBuffer->createVertexBuffer(VertexBuffer::AgeKind, 3, 1, 1);
  _updateVertexBuffers[VertexBuffer::LifeKindChars]
    = _updateBuffer->createVertexBuffer(VertexBuffer::LifeKind, 4, 1, 1);
  _updateVertexBuffers[VertexBuffer::VelocityKindChars]
    = _updateBuffer->createVertexBuffer(VertexBuffer::VelocityKind, 5, 3, 3);

  _updateVAO = _engine->recordVertexArrayObject(
    to_raw_ptr_map(_updateVertexBuffers), nullptr, _updateEffect.get());
  _engine->bindArrayBuffer(nullptr);

  // Render VAO
  _renderBuffer
    = ::std::make_unique<Buffer>(_scene->getEngine(), data, false, 0);
  _renderVertexBuffers[VertexBuffer::PositionKindChars]
    = _renderBuffer->createVertexBuffer(VertexBuffer::PositionKind, 0, 3, 3);
  _renderVertexBuffers[VertexBuffer::AgeKindChars]
    = _renderBuffer->createVertexBuffer(VertexBuffer::AgeKind, 3, 1, 1);
  _renderVertexBuffers[VertexBuffer::LifeKindChars]
    = _renderBuffer->createVertexBuffer(VertexBuffer::LifeKind, 4, 1, 1);
  _renderVertexBuffers[VertexBuffer::VelocityKindChars]
    = _renderBuffer->createVertexBuffer(VertexBuffer::VelocityKind, 5, 3, 3);

  _renderVAO = _engine->recordVertexArrayObject(
    to_raw_ptr_map(_renderVertexBuffers), nullptr, _renderEffect.get());
  _engine->bindArrayBuffer(nullptr);

  // Links
  _sourceVAO = _updateVAO.get();
  _targetVAO = _renderVAO.get();

  _sourceBuffer = _updateBuffer.get();
  _targetBuffer = _renderBuffer.get();
}

size_t GPUParticleSystem::render()
{
  if (/*!emitter ||*/ !_updateEffect->isReady() || !_renderEffect->isReady()) {
    return 0;
  }

  // Get everything ready to render
  _initialize();

  if (_currentRenderId == _scene->getRenderId()) {
    return 0;
  }

  _currentRenderId = _scene->getRenderId();

  // Enable update effect
  _engine->enableEffect(_updateEffect.get());
  _engine->setState(false);

  // Bind source VAO
  _engine->bindVertexArrayObject(_sourceVAO, nullptr);

  // Update
  _engine->bindTransformFeedbackBuffer(_targetBuffer->getBuffer());
  _engine->setRasterizerState(false);
  _engine->beginTransformFeedback();

  _engine->drawArraysType(Material::PointListDrawMode(), 0,
                          static_cast<int>(_capacity));
  _engine->endTransformFeedback();
  _engine->setRasterizerState(true);
  _engine->bindTransformFeedbackBuffer(nullptr);

  // Enable render effect
  _engine->enableEffect(_renderEffect.get());

  // Bind source VAO
  _engine->bindVertexArrayObject(_targetVAO, nullptr);

  // Render
  _engine->drawArraysType(Material::PointListDrawMode(), 0,
                          static_cast<int>(_capacity));

  // Switch VAOs
  auto tmpVAO = _sourceVAO;
  _sourceVAO  = _targetVAO;
  _targetVAO  = tmpVAO;

  // Switch buffers
  auto tmpBuffer = _sourceBuffer;
  _sourceBuffer  = _targetBuffer;
  _targetBuffer  = tmpBuffer;

  return 0;
}

void GPUParticleSystem::rebuild()
{
}

void GPUParticleSystem::dispose(bool /*doNotRecurse*/)
{
  // Remove from scene
  _scene->particleSystems.erase(
    ::std::remove_if(
      _scene->particleSystems.begin(), _scene->particleSystems.end(),
      [this](const unique_ptr_t<IParticleSystem>& particleSystem) {
        return particleSystem.get() == this;
      }),
    _scene->particleSystems.end());

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

} // end of namespace BABYLON
