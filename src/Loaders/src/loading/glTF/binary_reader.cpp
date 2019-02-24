#include <babylon/loading/glTF/binary_reader.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {
namespace GLTF2 {

BinaryReader::BinaryReader(const ArrayBuffer& arrayBuffer)
    : _arrayBuffer{arrayBuffer}
    , _dataView{DataView(arrayBuffer)}
    , _byteOffset{0}
{
}

BinaryReader::~BinaryReader()
{
}

size_t BinaryReader::getPosition() const
{
  return _byteOffset;
}

size_t BinaryReader::getLength() const
{
  return _arrayBuffer.size();
}

uint32_t BinaryReader::readUint32()
{
  const auto value = _dataView.getUint32(_byteOffset, true);
  _byteOffset += 4;
  return value;
}

Uint8Array BinaryReader::readUint8Array(size_t length)
{
  const auto value
    = stl_util::to_array<uint8_t>(_arrayBuffer, _byteOffset, length);
  _byteOffset += length;
  return value;
}

void BinaryReader::skipBytes(size_t length)
{
  _byteOffset += length;
}

} // end of namespace GLTF2
} // end of namespace BABYLON
