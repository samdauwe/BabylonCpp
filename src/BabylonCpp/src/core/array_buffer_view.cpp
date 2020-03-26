#include <babylon/core/array_buffer_view.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

ArrayBufferView::ArrayBufferView() = default;

ArrayBufferView::ArrayBufferView(const Int8Array& buffer)
    : byteOffset{0}, _uint8Array{stl_util::to_array<uint8_t>(buffer)}
{
}

ArrayBufferView::ArrayBufferView(const ArrayBuffer& arrayBuffer)
    : byteOffset{0}, _uint8Array{arrayBuffer}
{
}

ArrayBufferView::ArrayBufferView(const Uint16Array& buffer)
    : byteOffset{0}, _uint8Array{stl_util::to_array<uint8_t>(buffer)}
{
}

ArrayBufferView::ArrayBufferView(const Uint32Array& buffer)
    : byteOffset{0}, _uint8Array{stl_util::to_array<uint8_t>(buffer)}
{
}

ArrayBufferView::ArrayBufferView(const Float32Array& buffer)
    : byteOffset{0}, _uint8Array{stl_util::to_array<uint8_t>(buffer)}
{
}

ArrayBufferView::ArrayBufferView(const ArrayBufferView& other) = default;

ArrayBufferView::ArrayBufferView(ArrayBufferView&& other) = default;

ArrayBufferView& ArrayBufferView::operator=(const ArrayBufferView& other) = default;

ArrayBufferView& ArrayBufferView::operator=(ArrayBufferView&& other) = default;

ArrayBufferView::~ArrayBufferView() = default;

void ArrayBufferView::clear()
{
  _uint8Array.clear();
}

size_t ArrayBufferView::byteLength() const
{
  return _uint8Array.size();
}

ArrayBufferView::operator bool() const
{
  return !_uint8Array.empty();
}

Int8Array ArrayBufferView::int8Array() const
{
  return stl_util::to_array<int8_t>(_uint8Array);
}

Uint8Array& ArrayBufferView::buffer()
{
  return _uint8Array;
}

const Uint8Array& ArrayBufferView::buffer() const
{
  return _uint8Array;
}

Uint8Array& ArrayBufferView::uint8Array()
{
  return _uint8Array;
}

const Uint8Array& ArrayBufferView::uint8Array() const
{
  return _uint8Array;
}

Int16Array ArrayBufferView::int16Array() const
{
  return stl_util::to_array<int16_t>(_uint8Array);
}

Uint16Array ArrayBufferView::uint16Array() const
{
  return stl_util::to_array<uint16_t>(_uint8Array);
}

Int32Array ArrayBufferView::int32Array() const
{
  return stl_util::to_array<int32_t>(_uint8Array);
}

Uint32Array ArrayBufferView::uint32Array() const
{
  return stl_util::to_array<uint32_t>(_uint8Array);
}

Float32Array ArrayBufferView::float32Array() const
{
  return stl_util::to_array<float>(_uint8Array);
}

} // end of namespace BABYLON
