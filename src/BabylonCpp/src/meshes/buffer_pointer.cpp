#include <babylon/meshes/buffer_pointer.h>

namespace BABYLON {

BufferPointer::BufferPointer() = default;

BufferPointer::BufferPointer(bool _active, unsigned int _indx, int _size, unsigned int _type,
                             bool _normalized, int _stride, int _offset,
                             const WebGLDataBufferPtr& _buffer)
    : active{_active}
    , index{_indx}
    , size{_size}
    , type{_type}
    , normalized{_normalized}
    , stride{_stride}
    , offset{_offset}
    , buffer{_buffer}
{
}

BufferPointer::~BufferPointer() = default;

} // end of namespace BABYLON
