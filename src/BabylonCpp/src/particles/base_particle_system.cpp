#include <babylon/particles/base_particle_system.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/image_processing_configuration_defines.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/particles/emittertypes/box_particle_emitter.h>
#include <babylon/particles/emittertypes/cone_particle_emitter.h>
#include <babylon/particles/emittertypes/cylinder_particle_emitter.h>
#include <babylon/particles/emittertypes/hemispheric_particle_emitter.h>
#include <babylon/particles/emittertypes/point_particle_emitter.h>
#include <babylon/particles/emittertypes/sphere_directed_particle_emitter.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

BaseParticleSystem::BaseParticleSystem(const string_t& iName)
    : manualEmitCount{-1}
    , disposeOnStop{false}
    , onAnimationEnd{nullptr}
    , forceDepthWrite{false}
    , textureMask{Color4(1.f, 1.f, 1.f, 1.f)}
    , isAnimationSheetEnabled{this,
                              &BaseParticleSystem::get_isAnimationSheetEnabled,
                              &BaseParticleSystem::set_isAnimationSheetEnabled}
    , direction1{this, &BaseParticleSystem::get_direction1,
                 &BaseParticleSystem::set_direction1}
    , direction2{this, &BaseParticleSystem::get_direction2,
                 &BaseParticleSystem::set_direction2}
    , minEmitBox{this, &BaseParticleSystem::get_minEmitBox,
                 &BaseParticleSystem::set_minEmitBox}
    , maxEmitBox{this, &BaseParticleSystem::get_maxEmitBox,
                 &BaseParticleSystem::set_maxEmitBox}
    , imageProcessingConfiguration{this, &BaseParticleSystem::get_direction1,
                                   &BaseParticleSystem::set_direction1}
    , _isBillboardBased{true}
    , _imageProcessingConfigurationDefines{std::make_shared<
        ImageProcessingConfigurationDefines>()}
    , _zeroVector3{Vector3::Zero()}
{
  // IParticleSystem
  {
    id                    = iName;
    name                  = iName;
    renderingGroupId      = 0;
    emitRate              = 0;
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
    limitVelocityDamping  = 0.4f;
  }
}

BaseParticleSystem::~BaseParticleSystem()
{
}

bool BaseParticleSystem::get_isAnimationSheetEnabled() const
{
  return _isAnimationSheetEnabled;
}

void BaseParticleSystem::set_isAnimationSheetEnabled(bool value)
{
  if (_isAnimationSheetEnabled == value) {
    return;
  }

  _isAnimationSheetEnabled = value;

  _reset();
}

vector_t<FactorGradient>& BaseParticleSystem::getDragGradients()
{
  return _dragGradients;
}

vector_t<FactorGradient>& BaseParticleSystem::getLimitVelocityGradients()
{
  return _limitVelocityGradients;
}

vector_t<ColorGradient>& BaseParticleSystem::getColorGradients()
{
  return _colorGradients;
}

vector_t<FactorGradient>& BaseParticleSystem::getSizeGradients()
{
  return _sizeGradients;
}

vector_t<FactorGradient>& BaseParticleSystem::getLifeTimeGradients()
{
  return _lifeTimeGradients;
}

vector_t<FactorGradient>& BaseParticleSystem::getAngularSpeedGradients()
{
  return _angularSpeedGradients;
}

vector_t<FactorGradient>& BaseParticleSystem::getVelocityGradients()
{
  return _velocityGradients;
}

Vector3& BaseParticleSystem::get_direction1()
{
  if (auto boxParticleEmitter
      = std::dynamic_pointer_cast<BoxParticleEmitter>(particleEmitterType)) {
    return boxParticleEmitter->direction1;
  }

  return _zeroVector3;
}

void BaseParticleSystem::set_direction1(const Vector3& value)
{
  if (auto boxParticleEmitter
      = std::dynamic_pointer_cast<BoxParticleEmitter>(particleEmitterType)) {
    boxParticleEmitter->direction1 = value;
  }
}

Vector3& BaseParticleSystem::get_direction2()
{
  if (auto boxParticleEmitter
      = std::dynamic_pointer_cast<BoxParticleEmitter>(particleEmitterType)) {
    return boxParticleEmitter->direction2;
  }

  return _zeroVector3;
}

void BaseParticleSystem::set_direction2(const Vector3& value)
{
  if (auto boxParticleEmitter
      = std::dynamic_pointer_cast<BoxParticleEmitter>(particleEmitterType)) {
    boxParticleEmitter->direction2 = value;
  }
}

Vector3& BaseParticleSystem::get_minEmitBox()
{
  if (auto boxParticleEmitter
      = std::dynamic_pointer_cast<BoxParticleEmitter>(particleEmitterType)) {
    return boxParticleEmitter->minEmitBox;
  }

  return _zeroVector3;
}

void BaseParticleSystem::set_minEmitBox(const Vector3& value)
{
  if (auto boxParticleEmitter
      = std::dynamic_pointer_cast<BoxParticleEmitter>(particleEmitterType)) {
    boxParticleEmitter->minEmitBox = value;
  }
}

Vector3& BaseParticleSystem::get_maxEmitBox()
{
  if (auto boxParticleEmitter
      = std::dynamic_pointer_cast<BoxParticleEmitter>(particleEmitterType)) {
    return boxParticleEmitter->maxEmitBox;
  }

  return _zeroVector3;
}

void BaseParticleSystem::set_maxEmitBox(const Vector3& value)
{
  if (auto boxParticleEmitter
      = std::dynamic_pointer_cast<BoxParticleEmitter>(particleEmitterType)) {
    boxParticleEmitter->maxEmitBox = value;
  }
}

bool BaseParticleSystem::get_isBillboardBased() const
{
  return _isBillboardBased;
}

void BaseParticleSystem::set_isBillboardBased(bool value)
{
  if (_isBillboardBased == value) {
    return;
  }

  _isBillboardBased = value;
  _reset();
}

ImageProcessingConfigurationPtr&
BaseParticleSystem::get_imageProcessingConfiguration()
{
  return _imageProcessingConfiguration;
}

void BaseParticleSystem::set_imageProcessingConfiguration(
  const ImageProcessingConfigurationPtr& value)
{
  _attachImageProcessingConfiguration(value);
}

void BaseParticleSystem::_attachImageProcessingConfiguration(
  const ImageProcessingConfigurationPtr& configuration)
{
  if (configuration == _imageProcessingConfiguration) {
    return;
  }

  // Pick the scene configuration if needed.
  if (!configuration) {
    // _imageProcessingConfiguration = _scene->imageProcessingConfiguration;
  }
  else {
    _imageProcessingConfiguration = configuration;
  }
}

void BaseParticleSystem::_reset()
{
}

PointParticleEmitterPtr
BaseParticleSystem::createPointEmitter(const Vector3& direction1,
                                       const Vector3& direction2)
{
  auto particleEmitter        = std::make_shared<PointParticleEmitter>();
  particleEmitter->direction1 = direction1;
  particleEmitter->direction2 = direction2;

  particleEmitterType = particleEmitter;
  return particleEmitter;
}

HemisphericParticleEmitterPtr
BaseParticleSystem::createHemisphericEmitter(float radius, float radiusRange)
{
  auto particleEmitter
    = std::make_shared<HemisphericParticleEmitter>(radius, radiusRange);
  particleEmitterType = particleEmitter;
  return particleEmitter;
}

SphereParticleEmitterPtr
BaseParticleSystem::createSphereEmitter(float radius, float radiusRange)
{
  auto particleEmitter
    = std::make_shared<SphereParticleEmitter>(radius, radiusRange);
  particleEmitterType = particleEmitter;
  return particleEmitter;
}

SphereDirectedParticleEmitterPtr
BaseParticleSystem::createDirectedSphereEmitter(float radius,
                                                const Vector3& direction1,
                                                const Vector3& direction2)
{
  auto particleEmitter = std::make_shared<SphereDirectedParticleEmitter>(
    radius, direction1, direction2);
  particleEmitterType = particleEmitter;
  return particleEmitter;
}

CylinderParticleEmitterPtr BaseParticleSystem::createCylinderEmitter(
  float radius, float height, float radiusRange, float directionRandomizer)
{
  auto particleEmitter = std::make_shared<CylinderParticleEmitter>(
    radius, height, radiusRange, directionRandomizer);
  particleEmitterType = particleEmitter;
  return particleEmitter;
}

ConeParticleEmitterPtr BaseParticleSystem::createConeEmitter(float radius,
                                                             float angle)
{
  auto particleEmitter = std::make_shared<ConeParticleEmitter>(radius, angle);
  particleEmitterType  = particleEmitter;
  return particleEmitter;
}

BoxParticleEmitterPtr BaseParticleSystem::createBoxEmitter(
  const Vector3& iDirection1, const Vector3& iDirection2,
  const Vector3& iMinEmitBox, const Vector3& iMaxEmitBox)
{
  auto particleEmitter = std::make_shared<BoxParticleEmitter>();
  particleEmitterType  = particleEmitter;
  direction1           = iDirection1;
  direction2           = iDirection2;
  minEmitBox           = iMinEmitBox;
  maxEmitBox           = iMaxEmitBox;
  return particleEmitter;
}

} // end of namespace BABYLON
