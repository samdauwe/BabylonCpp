#ifndef BABYLON_PARTICLES_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_PARTICLE_SYSTEM_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/iparticle_system.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ParticleSystem : public IAnimatable,
                                             public IParticleSystem {

public:
  /** Statics **/
  static constexpr int BLENDMODE_ONEONE   = 0;
  static constexpr int BLENDMODE_STANDARD = 1;

public:
  ParticleSystem(const string_t& name, size_t capacity, Scene* scene,
                 Effect* customEffect = nullptr);
  virtual ~ParticleSystem();

  virtual IReflect::Type type() const override;

  void setOnDispose(
    const ::std::function<void(ParticleSystem*, EventState&)>& callback);
  void recycleParticle(Particle* particle);
  size_t getCapacity() const;
  bool isAlive() const;
  bool isStarted() const override;
  void start();
  void stop();
  void _appendParticleVertex(unsigned int index, Particle* particle,
                             int offsetX, int offsetY);
  void animate() override;
  size_t render() override;
  void dispose(bool doNotRecurse = false) override;
  vector_t<Animation*> getAnimations() override;
  IParticleSystem* clone(const string_t& name, Mesh* newEmitter) override;
  Json::object serialize() const override;

  // Statics
  static ParticleSystem* Parse(const Json::value& parsedParticleSystem,
                               Scene* scene, const string_t& url);

private:
  void _update(int newParticles);
  Effect* _getEffect();

public:
  // Members
  vector_t<Animation*> animations;
  int emitRate;
  int manualEmitCount;
  float updateSpeed;
  int targetStopDuration;
  bool disposeOnStop;
  float minEmitPower;
  float maxEmitPower;
  float minLifeTime;
  float maxLifeTime;
  float minSize;
  float maxSize;
  float minAngularSpeed;
  float maxAngularSpeed;
  Texture* particleTexture;
  string_t customShader;
  bool preventAutoStart;
  Observable<ParticleSystem> onDisposeObservable;
  ::std::function<void(vector_t<Particle*>& particles)> updateFunction;
  ::std::function<void()> onAnimationEnd;
  int blendMode;
  bool forceDepthWrite;
  Vector3 gravity;
  Vector3 direction1;
  Vector3 direction2;
  Vector3 minEmitBox;
  Vector3 maxEmitBox;
  Color4 color1;
  Color4 color2;
  Color4 colorDead;
  Color4 textureMask;
  ::std::function<void(float emitPower, const Matrix& worldMatrix,
                       Vector3& directionToUpdate, Particle* particle)>
    startDirectionFunction;
  ::std::function<void(const Matrix& worldMatrix, Vector3& positionToUpdate,
                       Particle* particle)>
    startPositionFunction;

private:
  Observer<ParticleSystem>::Ptr _onDisposeObserver;
  vector_t<Particle*> particles;
  size_t _capacity;
  Scene* _scene;
  vector_t<Particle*> _stockParticles;
  int _newPartsExcess;
  Float32Array _vertexData;
  unique_ptr_t<Buffer> _vertexBuffer;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  Effect* _customEffect;
  string_t _cachedDefines;

  Color4 _scaledColorStep;
  Color4 _colorDiff;
  Vector3 _scaledDirection;
  Vector3 _scaledGravity;
  int _currentRenderId;

  bool _alive;
  bool _started;
  bool _stopped;
  int _actualFrame;
  int _scaledUpdateSpeed;

}; // end of class ParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_SYSTEM_H
