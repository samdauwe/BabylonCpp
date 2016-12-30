#include <babylon/mesh/buffer_pointer.h>

namespace BABYLON {

BufferPointer::BufferPointer()
{
}

BufferPointer::BufferPointer(unsigned int _indx, int _size, unsigned int _type,
                             bool _normalized, int _stride, int _offset,
                             GL::IGLBuffer* _buffer)
    : indx{_indx}
    , size{_size}
    , type{_type}
    , normalized{_normalized}
    , stride{_stride}
    , offset{_offset}
    , buffer{_buffer}
{
}

BufferPointer::~BufferPointer()
{
}

} // end of namespace BABYLON
