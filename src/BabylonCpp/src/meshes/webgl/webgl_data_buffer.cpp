#include <babylon/meshes/webgl/webgl_data_buffer.h>

namespace BABYLON {

WebGLDataBuffer::WebGLDataBuffer(GL::IGLBuffer* resource) : DataBuffer{}
{
  _buffer = resource;
}

WebGLDataBuffer::~WebGLDataBuffer() = default;

GL::IGLBuffer* WebGLDataBuffer::underlyingResource()
{
  return _buffer;
}

} // end of namespace BABYLON
