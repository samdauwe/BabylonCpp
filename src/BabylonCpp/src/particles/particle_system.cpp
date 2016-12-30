#include <babylon/particles/particle_system.h>

#include <babylon/core/random.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/particles/particle.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

ParticleSystem::ParticleSystem(const std::string& _name, size_t capacity,
                               Scene* scene, Effect* customEffect)
    : id{_name}
    , name{_name}
    , renderingGroupId{0}
    , emitter{nullptr}
    , emitRate{10}
    , manualEmitCount{-1}
    , updateSpeed{0.01f}
    , targetStopDuration{0}
    , disposeOnStop{false}
    , minEmitPower{1}
    , maxEmitPower{1}
    , minLifeTime{1}
    , maxLifeTime{1}
    , minSize{1}
    , maxSize{1}
    , minAngularSpeed{0}
    , maxAngularSpeed{0}
    , layerMask{0x0FFFFFFF}
    , blendMode{ParticleSystem::BLENDMODE_ONEONE}
    , forceDepthWrite{false}
    , gravity{Vector3::Zero()}
    , direction1{Vector3(0.f, 1.f, 0.f)}
    , direction2{Vector3(0.f, 1.f, 0.f)}
    , minEmitBox{Vector3(-0.5f, -0.5f, -0.5f)}
    , maxEmitBox{Vector3(0.5f, 0.5f, 0.5f)}
    , color1{Color4(1.f, 1.f, 1.f, 1.f)}
    , color2{Color4(1.f, 1.f, 1.f, 1.f)}
    , colorDead{Color4(0.f, 0.f, 0.f, 1.f)}
    , textureMask{Color4(1.f, 1.f, 1.f, 1.f)}
    , _capacity{capacity}
    , _scene{scene}
    , _newPartsExcess{0}
    , _customEffect{customEffect}
    , _scaledColorStep{Color4(0.f, 0.f, 0.f, 0.f)}
    , _colorDiff{Color4(0.f, 0.f, 0.f, 0.f)}
    , _scaledDirection{Vector3::Zero()}
    , _scaledGravity{Vector3::Zero()}
    , _currentRenderId{-1}
    , _started{false}
    , _stopped{false}
    , _actualFrame{0}
{
  _scene->particleSystems.emplace_back(this);

  Uint32Array indices;
  int index = 0;
  for (size_t count = 0; count < capacity; ++count) {
    indices.emplace_back(index);
    indices.emplace_back(index + 1);
    indices.emplace_back(index + 2);
    indices.emplace_back(index);
    indices.emplace_back(index + 2);
    indices.emplace_back(index + 3);
    index += 4;
  }

  _indexBuffer = scene->getEngine()->createIndexBuffer(indices);

  // 11 floats per particle (x, y, z, r, g, b, a, angle, size, offsetX, offsetY)
  // + 1 filler
  _vertexData.resize(capacity * 11 * 4);
  /*_vertexBuffer = new Buffer(scene->getEngine(), _vertexData, true, 11);

  VertexBuffer* positions
    = _vertexBuffer->createVertexBuffer(VertexBuffer::PositionKind, 0, 3);
  VertexBuffer* colors
    = _vertexBuffer->createVertexBuffer(VertexBuffer::ColorKindChars, 3, 4);
  VertexBuffer* options = _vertexBuffer->createVertexBuffer("options", 7, 4);

  _vertexBuffers[VertexBuffer::PositionKindChars] = positions;
  _vertexBuffers[VertexBuffer::ColorKindChars]    = colors;
  _vertexBuffers["options"]                       = options;*/

  // Default behaviors
  startDirectionFunction
    = [this](float emitPower, const Matrix& worldMatrix,
             Vector3& directionToUpdate, Particle* /*particle*/) {
        float randX = Math::randomNumber(direction1.x, direction2.x);
        float randY = Math::randomNumber(direction1.y, direction2.y);
        float randZ = Math::randomNumber(direction1.z, direction2.z);

        Vector3::TransformNormalFromFloatsToRef(
          randX * emitPower, randY * emitPower, randZ * emitPower, worldMatrix,
          directionToUpdate);
      };

  startPositionFunction = [this](const Matrix& worldMatrix,
                                 Vector3& positionToUpdate,
                                 Particle* /*particle*/) {
    float randX = Math::randomNumber(minEmitBox.x, maxEmitBox.x);
    float randY = Math::randomNumber(minEmitBox.y, maxEmitBox.y);
    float randZ = Math::randomNumber(minEmitBox.z, maxEmitBox.z);

    Vector3::TransformCoordinatesFromFloatsToRef(randX, randY, randZ,
                                                 worldMatrix, positionToUpdate);
  };

  updateFunction = [this](std::vector<Particle*>& _particles) {
    for (unsigned int pIndex = 0; pIndex < _particles.size(); ++pIndex) {
      Particle* particle = _particles[pIndex];
      particle->age += static_cast<float>(_scaledUpdateSpeed);

      if (particle->age >= particle->lifeTime) {
        // Recycle by swapping with last particle
        recycleParticle(particle);
        --pIndex;
        continue;
      }
      else {
        particle->colorStep.scaleToRef(_scaledUpdateSpeed, _scaledColorStep);
        particle->color.addInPlace(_scaledColorStep);

        if (particle->color.a < 0.f) {
          particle->color.a = 0.f;
        }

        particle->angle
          += particle->angularSpeed * static_cast<float>(_scaledUpdateSpeed);

        particle->direction.scaleToRef(_scaledUpdateSpeed, _scaledDirection);
        particle->position.addInPlace(_scaledDirection);

        gravity.scaleToRef(_scaledUpdateSpeed, _scaledGravity);
        particle->direction.addInPlace(_scaledGravity);
      }
    }
  };
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::setOnDispose(const FastFunc<void()>& /*callback*/)
{
  /*if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);*/
}

void ParticleSystem::recycleParticle(Particle* particle)
{
  Particle* lastParticle = particles.back();
  particles.pop_back();

  if (lastParticle != particle) {
    lastParticle->copyTo(*particle);
    _stockParticles.emplace_back(lastParticle);
  }
}

size_t ParticleSystem::getCapacity() const
{
  return _capacity;
}

bool ParticleSystem::isAlive() const
{
  return _alive;
}

bool ParticleSystem::isStarted() const
{
  return _started;
}

void ParticleSystem::start()
{
  _started     = true;
  _stopped     = false;
  _actualFrame = 0;
}

void ParticleSystem::stop()
{
  _stopped = true;
}

void ParticleSystem::_appendParticleVertex(unsigned int index,
                                           Particle* particle, int offsetX,
                                           int offsetY)
{
  unsigned int offset      = index * 11;
  _vertexData[offset]      = particle->position.x;
  _vertexData[offset + 1]  = particle->position.y;
  _vertexData[offset + 2]  = particle->position.z;
  _vertexData[offset + 3]  = particle->color.r;
  _vertexData[offset + 4]  = particle->color.g;
  _vertexData[offset + 5]  = particle->color.b;
  _vertexData[offset + 6]  = particle->color.a;
  _vertexData[offset + 7]  = particle->angle;
  _vertexData[offset + 8]  = particle->size;
  _vertexData[offset + 9]  = static_cast<float>(offsetX);
  _vertexData[offset + 10] = static_cast<float>(offsetY);
}

void ParticleSystem::_update(int newParticles)
{
  // Update current
  _alive = !particles.empty();

  updateFunction(particles);

  // Add new ones
  Matrix worldMatrix;

  // if (emitter->position()) {
  //  worldMatrix = *emitter->getWorldMatrix();
  //}
  // else {
  // worldMatrix = Matrix::Translation(emitter->x, emitter->y, emitter->z);
  //}

  for (int index = 0; index < newParticles; ++index) {
    if (particles.size() == _capacity) {
      break;
    }

    Particle* particle = nullptr;
    if (!_stockParticles.empty()) {
      particle = _stockParticles.back();
      _stockParticles.pop_back();
      particle->age = 0;
    }
    else {
      particle = new Particle();
    }
    particles.emplace_back(particle);

    float emitPower = Math::randomNumber(minEmitPower, maxEmitPower);

    startDirectionFunction(emitPower, worldMatrix, particle->direction,
                           particle);

    particle->lifeTime = Math::randomNumber(minLifeTime, maxLifeTime);

    particle->size = Math::randomNumber(minSize, maxSize);
    particle->angularSpeed
      = Math::randomNumber(minAngularSpeed, maxAngularSpeed);

    startPositionFunction(worldMatrix, particle->position, particle);

    float step = Math::random();

    Color4::LerpToRef(color1, color2, step, particle->color);

    colorDead.subtractToRef(particle->color, _colorDiff);
    _colorDiff.scaleToRef(1.f / particle->lifeTime, particle->colorStep);
  }
}

Effect* ParticleSystem::_getEffect()
{
  if (_customEffect) {
    return _customEffect;
  };

  std::vector<std::string> defines;

  if (_scene->clipPlane()) {
    defines.emplace_back("#define CLIPPLANE");
  }

  // Effect
  std::string joined = String::join(defines, '\n');
  if (_cachedDefines != joined) {
    _cachedDefines = joined;

    _effect = _scene->getEngine()->createEffect(
      "particles", {VertexBuffer::PositionKindChars,
                    VertexBuffer::ColorKindChars, "options"},
      {"invView", "view", "projection", "vClipPlane", "textureMask"},
      {"diffuseSampler"}, joined);
  }

  return _effect;
}

void ParticleSystem::animate()
{
  if (!_started)
    return;

  Effect* effect = _getEffect();

  // Check
  if (!emitter || !effect->isReady() || !particleTexture
      || !particleTexture->isReady())
    return;

  if (_currentRenderId == _scene->getRenderId()) {
    return;
  }

  _currentRenderId = _scene->getRenderId();

  _scaledUpdateSpeed
    = static_cast<int>(updateSpeed * _scene->getAnimationRatio());

  // determine the number of particles we need to create
  int newParticles = 0;

  if (manualEmitCount > -1) {
    newParticles    = manualEmitCount;
    _newPartsExcess = 0;
    manualEmitCount = 0;
  }
  else {
    newParticles = ((emitRate * _scaledUpdateSpeed) >> 0);
    _newPartsExcess += emitRate * _scaledUpdateSpeed - newParticles;
  }

  if (_newPartsExcess > 1) {
    newParticles += _newPartsExcess >> 0;
    _newPartsExcess -= _newPartsExcess >> 0;
  }

  _alive = false;

  if (!_stopped) {
    _actualFrame += _scaledUpdateSpeed;

    if (targetStopDuration && _actualFrame >= targetStopDuration)
      stop();
  }
  else {
    newParticles = 0;
  }

  _update(newParticles);

  // Stopped?
  if (_stopped) {
    if (!_alive) {
      _started = false;
      if (disposeOnStop) {
        _scene->_toBeDisposed.emplace_back(this);
      }
    }
  }

  // Update VBO
  unsigned int offset = 0;
  for (auto& particle : particles) {
    _appendParticleVertex(++offset, particle, 0, 0);
    _appendParticleVertex(++offset, particle, 1, 0);
    _appendParticleVertex(++offset, particle, 1, 1);
    _appendParticleVertex(++offset, particle, 0, 1);
  }
  //_scene->getEngine()->updateDynamicVertexBuffer(_vertexBuffer, _vertices);
}

size_t ParticleSystem::render()
{
  Effect* effect = _getEffect();

  // Check
  if (!emitter || !effect->isReady() || !particleTexture
      || !particleTexture->isReady() || !particles.size()) {
    return 0;
  }

  Engine* engine = _scene->getEngine();

  // Render
  engine->enableEffect(effect);
  engine->setState(false);

  Matrix viewMatrix = _scene->getViewMatrix();
  effect->setTexture("diffuseSampler", particleTexture);
  effect->setMatrix("view", viewMatrix);
  effect->setMatrix("projection", _scene->getProjectionMatrix());
  effect->setFloat4("textureMask", textureMask.r, textureMask.g, textureMask.b,
                    textureMask.a);

  if (_scene->clipPlane()) {
    Plane* clipPlane = _scene->clipPlane();
    Matrix invView   = viewMatrix;
    invView.invert();
    effect->setMatrix("invView", invView);
    effect->setFloat4("vClipPlane", clipPlane->normal.x, clipPlane->normal.y,
                      clipPlane->normal.z, clipPlane->d);
  }

  // VBOs
  engine->bindBuffers(_vertexBuffers, _indexBuffer.get(), effect);

  // Draw order
  if (blendMode == ParticleSystem::BLENDMODE_ONEONE) {
    engine->setAlphaMode(Engine::ALPHA_ONEONE);
  }
  else {
    engine->setAlphaMode(Engine::ALPHA_COMBINE);
  }

  if (forceDepthWrite) {
    engine->setDepthWrite(true);
  }

  engine->draw(true, 0, particles.size() * 6);
  engine->setAlphaMode(Engine::ALPHA_DISABLE);

  return particles.size();
}

void ParticleSystem::dispose(bool /*doNotRecurse*/)
{
  if (_vertexBuffer) {
    //_scene->getEngine()->_releaseBuffer(_vertexBuffer);
    _vertexBuffer = nullptr;
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer.reset(nullptr);
  }

  if (particleTexture) {
    particleTexture->dispose();
    particleTexture = nullptr;
  }

  // Remove from scene
  // TODO FIXME
  /*auto it = std::find(_scene->particleSystems.begin(),
                      _scene->particleSystems.end(), this);
  if (it != _scene->particleSystems.end()) {
    _scene->particleSystems.erase(it);
  }*/

  // Callback
  // if (onDispose) {
  //  onDispose();
  //}
}

std::vector<Animation*> ParticleSystem::getAnimations()
{
  return animations;
}

ParticleSystem* ParticleSystem::clone(const std::string& /*iName*/,
                                      Mesh* /*newEmitter*/)
{
  // ParticleSystem* result = new ParticleSystem(_name, _capacity, _scene);

  // TODO FIXME
  // Tools.DeepCopy(this, result, ["particles"],
  //               [ "_vertexDeclaration", "_vertexStrideSize" ]);

  // if (newEmitter == nullptr) {
  //  newEmitter = emitter;
  //}

  /*result->emitter = newEmitter;
  if (particleTexture) {
    result->particleTexture = new Texture(particleTexture->url, _scene);
  }

  result->start();

  return result;*/
  return nullptr;
}

ParticleSystem*
ParticleSystem::Parse(const Json::value& /*parsedParticleSystem*/,
                      Scene* /*scene*/, const std::string& /*url*/)
{
  return nullptr;
}

} // end of namespace BABYLON
