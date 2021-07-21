#ifndef BABYLON_MESHES_WEBGL_DATA_BUFFER_H
#define BABYLON_MESHES_WEBGL_DATA_BUFFER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/buffers/data_buffer.h>

namespace BABYLON {

namespace GL {
FWD_CLASS_SPTR(IGLBuffer)
} // end of namespace GL

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT WebGLDataBuffer : public DataBuffer<GL::IGLBufferPtr> {

public:
  WebGLDataBuffer(const GL::IGLBufferPtr& resource);
  ~WebGLDataBuffer() override; // = default

  GL::IGLBufferPtr underlyingResource() override;

private:
  GL::IGLBufferPtr _buffer;

}; // end of class ISimplifier

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_WEBGL_DATA_BUFFER_H
