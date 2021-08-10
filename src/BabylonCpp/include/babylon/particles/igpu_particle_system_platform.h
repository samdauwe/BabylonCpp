#ifndef BABYLON_PARTICLES_IGPU_PARTICLE_SYSTEM_PLATFORM_H
#define BABYLON_PARTICLES_IGPU_PARTICLE_SYSTEM_PLATFORM_H

#include <memory>
#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Buffer;
FWD_CLASS_SPTR(Effect)
FWD_CLASS_SPTR(UniformBufferEffectCommonAccessor)
FWD_CLASS_SPTR(VertexBuffer)

struct BABYLON_SHARED_EXPORT IGPUParticleSystemPlatform {

  bool alignDataInBuffer = false;

  virtual bool isUpdateBufferCreated() const = 0;
  virtual bool isUpdateBufferReady() const   = 0;

  virtual UniformBufferEffectCommonAccessorPtr createUpdateBuffer(const std::string& defines) = 0;
  virtual void
  createVertexBuffers(Buffer* updateBuffer,
                      const std::unordered_map<std::string, VertexBufferPtr>& renderVertexBuffers)
    = 0;

  virtual void bindDrawBuffers(unsigned int index, Effect* effect) = 0;

  virtual void preUpdateParticleBuffer() = 0;
  virtual void updateParticleBuffer(unsigned int index, Buffer* targetBuffer,
                                    unsigned int currentActiveCount)
    = 0;

  virtual void releaseBuffers()       = 0;
  virtual void releaseVertexBuffers() = 0;

}; // end of struct IGPUParticleSystemPlatform

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_IGPU_PARTICLE_SYSTEM_PLATFORM_H
