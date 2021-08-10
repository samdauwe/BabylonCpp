#include <babylon/particles/webgl2_particle_system.h>

#include <babylon/buffers/buffer.h>
#include <babylon/buffers/vertex_buffer.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/draw_wrapper.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/materials/textures/thin_texture.h>
#include <babylon/materials/uniform_buffer_effect_common_accessor.h>
#include <babylon/particles/emittertypes/custom_particle_emitter.h>
#include <babylon/particles/gpu_particle_system.h>

namespace BABYLON {

WebGL2ParticleSystem::WebGL2ParticleSystem(GPUParticleSystem* parent, ThinEngine* engine)
    : alignDataInBuffer{false}, _updateEffect{nullptr}
{
  _parent = parent;
  _engine = engine;

  _updateEffectOptions.attributes = {
    "position",          //
    "initialPosition",   //
    "age",               //
    "life",              //
    "seed",              //
    "size",              //
    "color",             //
    "direction",         //
    "initialDirection",  //
    "angle",             //
    "cellIndex",         //
    "cellStartOffset",   //
    "noiseCoordinates1", //
    "noiseCoordinates2", //
  };
  _updateEffectOptions.uniformsNames = {
    "currentCount",         //
    "timeDelta",            //
    "emitterWM",            //
    "lifeTime",             //
    "color1",               //
    "color2",               //
    "sizeRange",            //
    "scaleRange",           //
    "gravity",              //
    "emitPower",            //
    "direction1",           //
    "direction2",           //
    "minEmitBox",           //
    "maxEmitBox",           //
    "radius",               //
    "directionRandomizer",  //
    "height",               //
    "coneAngle",            //
    "stopFactor",           //
    "angleRange",           //
    "radiusRange",          //
    "cellInfos",            //
    "noiseStrength",        //
    "limitVelocityDamping", //
  };
  _updateEffectOptions.uniformBuffersNames = {};
  _updateEffectOptions.samplers            = {
    "randomSampler",                //
    "randomSampler2",               //
    "sizeGradientSampler",          //
    "angularSpeedGradientSampler",  //
    "velocityGradientSampler",      //
    "limitVelocityGradientSampler", //
    "noiseSampler",                 //
    "dragGradientSampler",          //
  };
  _updateEffectOptions.defines                   = "";
  _updateEffectOptions.fallbacks                 = nullptr;
  _updateEffectOptions.onCompiled                = nullptr;
  _updateEffectOptions.onError                   = nullptr;
  _updateEffectOptions.indexParameters           = {};
  _updateEffectOptions.maxSimultaneousLights     = 0;
  _updateEffectOptions.transformFeedbackVaryings = {};
}

WebGL2ParticleSystem::~WebGL2ParticleSystem() = default;

bool WebGL2ParticleSystem::isUpdateBufferCreated() const
{
  return _updateEffect != nullptr;
}

bool WebGL2ParticleSystem::isUpdateBufferReady() const
{
  return _updateEffect ? _updateEffect->isReady() : false;
}

UniformBufferEffectCommonAccessorPtr
WebGL2ParticleSystem::createUpdateBuffer(const std::string& defines)
{
  _updateEffectOptions.transformFeedbackVaryings = {"outPosition"};
  _updateEffectOptions.transformFeedbackVaryings.emplace_back("outAge");
  _updateEffectOptions.transformFeedbackVaryings.emplace_back("outSize");
  _updateEffectOptions.transformFeedbackVaryings.emplace_back("outLife");
  _updateEffectOptions.transformFeedbackVaryings.emplace_back("outSeed");
  _updateEffectOptions.transformFeedbackVaryings.emplace_back("outDirection");

  const auto parentAsCustomParticleEmitter
    = std::static_pointer_cast<CustomParticleEmitter>(_parent->particleEmitterType);
  if (parentAsCustomParticleEmitter) {
    _updateEffectOptions.transformFeedbackVaryings.emplace_back("outInitialPosition");
  }

  if (!_parent->_colorGradientsTexture) {
    _updateEffectOptions.transformFeedbackVaryings.emplace_back("outColor");
  }

  if (!_parent->_isBillboardBased) {
    _updateEffectOptions.transformFeedbackVaryings.emplace_back("outInitialDirection");
  }

  if (_parent->noiseTexture()) {
    _updateEffectOptions.transformFeedbackVaryings.emplace_back("outNoiseCoordinates1");
    _updateEffectOptions.transformFeedbackVaryings.emplace_back("outNoiseCoordinates2");
  }

  _updateEffectOptions.transformFeedbackVaryings.emplace_back("outAngle");

  if (_parent->isAnimationSheetEnabled()) {
    _updateEffectOptions.transformFeedbackVaryings.emplace_back("outCellIndex");
    if (_parent->spriteRandomStartCell) {
      _updateEffectOptions.transformFeedbackVaryings.emplace_back("outCellStartOffset");
    }
  }

  _updateEffectOptions.defines = defines;
  _updateEffect                = Effect::New("gpuUpdateParticles", _updateEffectOptions, _engine);

  return std::make_shared<UniformBufferEffectCommonAccessor>(_updateEffect.get());
}

void WebGL2ParticleSystem::createVertexBuffers(
  Buffer* updateBuffer, const std::unordered_map<std::string, VertexBufferPtr>& renderVertexBuffers)
{
  _updateVAO.emplace_back(_createUpdateVAO(updateBuffer));

  _renderVAO.emplace_back(_engine->recordVertexArrayObject(
    renderVertexBuffers, nullptr, _parent->_getWrapper(_parent->blendMode)->effect));
  _engine->bindArrayBuffer(nullptr);
}

void WebGL2ParticleSystem::bindDrawBuffers(unsigned int index, Effect* /*effect*/)
{
  _engine->bindVertexArrayObject(_renderVAO[index], nullptr);
}

void WebGL2ParticleSystem::preUpdateParticleBuffer()
{
  const auto engine = static_cast<Engine*>(_engine);

  if (!engine) {
    throw std::runtime_error(
      "GPU particles cannot work without a full Engine. ThinEngine is not supported");
  }

  _engine->enableEffect(_updateEffect);
}

void WebGL2ParticleSystem::updateParticleBuffer(unsigned int index, Buffer* targetBuffer,
                                                unsigned int currentActiveCount)
{
  _updateEffect->setTexture("randomSampler", _parent->_randomTexture);
  _updateEffect->setTexture("randomSampler2", _parent->_randomTexture2);

  if (_parent->_sizeGradientsTexture) {
    _updateEffect->setTexture("sizeGradientSampler", _parent->_sizeGradientsTexture);
  }

  if (_parent->_angularSpeedGradientsTexture) {
    _updateEffect->setTexture("angularSpeedGradientSampler",
                              _parent->_angularSpeedGradientsTexture);
  }

  if (_parent->_velocityGradientsTexture) {
    _updateEffect->setTexture("velocityGradientSampler", _parent->_velocityGradientsTexture);
  }

  if (_parent->_limitVelocityGradientsTexture) {
    _updateEffect->setTexture("limitVelocityGradientSampler",
                              _parent->_limitVelocityGradientsTexture);
  }

  if (_parent->_dragGradientsTexture) {
    _updateEffect->setTexture("dragGradientSampler", _parent->_dragGradientsTexture);
  }

  if (_parent->noiseTexture()) {
    _updateEffect->setTexture("noiseSampler", _parent->noiseTexture());
  }

  // Bind source VAO
  _engine->bindVertexArrayObject(_updateVAO[index], nullptr);

  // Update
  const auto engine = static_cast<Engine*>(_engine);

  engine->bindTransformFeedbackBuffer(targetBuffer->getBuffer());
  engine->setRasterizerState(false);
  engine->beginTransformFeedback(true);
  engine->drawArraysType(Constants::MATERIAL_PointListDrawMode, 0, currentActiveCount);
  engine->endTransformFeedback();
  engine->setRasterizerState(true);
  engine->bindTransformFeedbackBuffer(nullptr);
}

void WebGL2ParticleSystem::releaseBuffers()
{
}

void WebGL2ParticleSystem::releaseVertexBuffers()
{
  for (const auto& updateVAO : _updateVAO) {
    _engine->releaseVertexArrayObject(updateVAO);
  }
  _updateVAO = {};

  for (const auto& renderVAO : _renderVAO) {
    _engine->releaseVertexArrayObject(renderVAO);
  }
  _renderVAO = {};
}

WebGLVertexArrayObjectPtr WebGL2ParticleSystem::_createUpdateVAO(Buffer* source)
{
  std::unordered_map<std::string, VertexBufferPtr> updateVertexBuffers = {};
  updateVertexBuffers["position"] = source->createVertexBuffer("position", 0, 3);

  size_t offset              = 3;
  updateVertexBuffers["age"] = source->createVertexBuffer("age", offset, 1);
  offset += 1;
  updateVertexBuffers["size"] = source->createVertexBuffer("size", offset, 3);
  offset += 3;
  updateVertexBuffers["life"] = source->createVertexBuffer("life", offset, 1);
  offset += 1;
  updateVertexBuffers["seed"] = source->createVertexBuffer("seed", offset, 4);
  offset += 4;
  updateVertexBuffers["direction"] = source->createVertexBuffer("direction", offset, 3);
  offset += 3;

  const auto parentAsCustomParticleEmitter
    = std::static_pointer_cast<CustomParticleEmitter>(_parent->particleEmitterType);
  if (parentAsCustomParticleEmitter) {
    updateVertexBuffers["initialPosition"]
      = source->createVertexBuffer("initialPosition", offset, 3);
    offset += 3;
  }

  if (!_parent->_colorGradientsTexture) {
    updateVertexBuffers["color"] = source->createVertexBuffer("color", offset, 4);
    offset += 4;
  }

  if (!_parent->_isBillboardBased) {
    updateVertexBuffers["initialDirection"]
      = source->createVertexBuffer("initialDirection", offset, 3);
    offset += 3;
  }

  if (_parent->noiseTexture()) {
    updateVertexBuffers["noiseCoordinates1"]
      = source->createVertexBuffer("noiseCoordinates1", offset, 3);
    offset += 3;
    updateVertexBuffers["noiseCoordinates2"]
      = source->createVertexBuffer("noiseCoordinates2", offset, 3);
    offset += 3;
  }

  if (_parent->_angularSpeedGradientsTexture) {
    updateVertexBuffers["angle"] = source->createVertexBuffer("angle", offset, 1);
    offset += 1;
  }
  else {
    updateVertexBuffers["angle"] = source->createVertexBuffer("angle", offset, 2);
    offset += 2;
  }

  if (_parent->_isAnimationSheetEnabled) {
    updateVertexBuffers["cellIndex"] = source->createVertexBuffer("cellIndex", offset, 1);
    offset += 1;
    if (_parent->spriteRandomStartCell) {
      updateVertexBuffers["cellStartOffset"]
        = source->createVertexBuffer("cellStartOffset", offset, 1);
      offset += 1;
    }
  }

  const auto vao = _engine->recordVertexArrayObject(updateVertexBuffers, nullptr, _updateEffect);
  _engine->bindArrayBuffer(nullptr);

  return vao;
}

} // end of namespace BABYLON
