#ifndef BABYLON_PARTICLES_WEBGL2_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_WEBGL2_PARTICLE_SYSTEM_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/particles/igpu_particle_system_platform.h>

namespace BABYLON {

namespace GL {
class IGLVertexArrayObject;
} // end of namespace GL

class GPUParticleSystem;
class ThinEngine;
using WebGLVertexArrayObjectPtr = std::shared_ptr<GL::IGLVertexArrayObject>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT WebGL2ParticleSystem : public IGPUParticleSystemPlatform {

public:
  WebGL2ParticleSystem(GPUParticleSystem* parent, ThinEngine* engine);
  ~WebGL2ParticleSystem(); // = default

  bool isUpdateBufferCreated() const override;
  bool isUpdateBufferReady() const override;
  UniformBufferEffectCommonAccessorPtr createUpdateBuffer(const std::string& defines) override;
  void createVertexBuffers(
    Buffer* updateBuffer,
    const std::unordered_map<std::string, VertexBufferPtr>& renderVertexBuffers) override;
  void bindDrawBuffers(unsigned int index, Effect* effect) override;
  void preUpdateParticleBuffer() override;
  void updateParticleBuffer(unsigned int index, Buffer* targetBuffer,
                            unsigned int currentActiveCount) override;
  void releaseBuffers() override;
  void releaseVertexBuffers() override;

private:
  WebGLVertexArrayObjectPtr _createUpdateVAO(Buffer* source);

public:
  const bool alignDataInBuffer;

private:
  GPUParticleSystem* _parent;
  ThinEngine* _engine;
  EffectPtr _updateEffect;
  IEffectCreationOptions _updateEffectOptions;
  std::vector<WebGLVertexArrayObjectPtr> _renderVAO;
  std::vector<WebGLVertexArrayObjectPtr> _updateVAO;

}; // end of class WebGL2ParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_SYSTEM_H
