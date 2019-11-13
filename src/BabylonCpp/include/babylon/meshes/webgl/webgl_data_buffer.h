#ifndef BABYLON_MESHES_WEBGL_DATA_BUFFER_H
#define BABYLON_MESHES_WEBGL_DATA_BUFFER_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/data_buffer.h>

namespace BABYLON {

namespace GL {
class IGLBuffer;
} // end of namespace GL

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT WebGLDataBuffer
    : public DataBuffer<GL::IGLBuffer*> {

public:
  WebGLDataBuffer(GL::IGLBuffer* resource);
  virtual ~WebGLDataBuffer(); // = default

  GL::IGLBuffer* underlyingResource() override;

private:
  GL::IGLBuffer* _buffer;

}; // end of class ISimplifier

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_WEBGL_DATA_BUFFER_H
