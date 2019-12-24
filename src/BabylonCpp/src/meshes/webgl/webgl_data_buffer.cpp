#include <babylon/meshes/webgl/webgl_data_buffer.h>

namespace BABYLON {

WebGLDataBuffer::WebGLDataBuffer(const GL::IGLBufferPtr& resource)
{
  _buffer = resource;
}

WebGLDataBuffer::~WebGLDataBuffer() = default;

GL::IGLBufferPtr WebGLDataBuffer::underlyingResource()
{
  return _buffer;
}

} // end of namespace BABYLON
