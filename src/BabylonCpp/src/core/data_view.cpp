#include <babylon/core/data_view.h>

namespace BABYLON {

DataView::DataView(const ArrayBuffer& buffer)
    : _buffer{buffer}, _byteLength{buffer.size()}, _byteOffset{0}
{
}

DataView::DataView(const ArrayBuffer& buffer, size_t byteOffset,
                   size_t byteLength)
    : _buffer{buffer}, _byteLength{byteLength}, _byteOffset{byteOffset}
{
}

DataView::DataView(const DataView& other)
    : _buffer{other._buffer}
    , _byteLength{other._byteLength}
    , _byteOffset{other._byteOffset}
{
}

DataView::DataView(DataView&& other)
    : _buffer{::std::move(other._buffer)}
    , _byteLength{::std::move(other._byteLength)}
    , _byteOffset{::std::move(other._byteOffset)}
{
}

DataView& DataView::operator=(const DataView& other)
{
  if (&other != this) {
    _buffer     = other._buffer;
    _byteLength = other._byteLength;
    _byteOffset = other._byteOffset;
  }

  return *this;
}

DataView& DataView::operator=(DataView&& other)
{
  if (&other != this) {
    _buffer     = ::std::move(other._buffer);
    _byteLength = ::std::move(other._byteLength);
    _byteOffset = ::std::move(other._byteOffset);
  }

  return *this;
}

DataView::~DataView()
{
}

int8_t DataView::getInt8(size_t /*byteOffset*/) const
{
  return int8_t{0};
}

uint8_t DataView::getUint8(size_t /*byteOffset*/) const
{
  return uint8_t{0};
}

int16_t DataView::getInt16(size_t /*byteOffset*/, bool /*littleEndian*/) const
{
  return int16_t{0};
}

uint16_t DataView::getUint16(size_t /*byteOffset*/, bool /*littleEndian*/) const
{
  return uint16_t{0};
}

float DataView::getFloat32(size_t /*byteOffset*/, bool /*littleEndian*/) const
{
  return float{0};
}

} // end of namespace BABYLON
