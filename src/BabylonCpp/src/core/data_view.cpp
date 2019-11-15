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

DataView::DataView(const DataView& other) = default;

DataView::DataView(DataView&& other) = default;

DataView& DataView::operator=(const DataView& other) = default;

DataView& DataView::operator=(DataView&& other) = default;

DataView::~DataView() = default;

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

int32_t DataView::getInt32(size_t /*byteOffset*/, bool /*littleEndian*/) const
{
  return int32_t{0};
}

uint16_t DataView::getUint16(size_t /*byteOffset*/, bool /*littleEndian*/) const
{
  return uint16_t{0};
}

uint32_t DataView::getUint32(size_t /*byteOffset*/, bool /*littleEndian*/) const
{
  return uint32_t{0};
}

float DataView::getFloat32(size_t /*byteOffset*/, bool /*littleEndian*/) const
{
  return float{0};
}

int DataView::switchEndianness(int val)
{
  return ((val & 0xFF) << 24)    //
         | ((val & 0xFF00) << 8) //
         | ((val >> 8) & 0xFF00) //
         | ((val >> 24) & 0xFF); //
}

} // end of namespace BABYLON
