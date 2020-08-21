#include <babylon/meshes/vertex_buffer.h>

#include <babylon/core/data_view.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/meshes/buffer.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

VertexBuffer::VertexBuffer(ThinEngine* engine, const std::variant<Float32Array, Buffer*>& data,
                           const std::string& kind, bool updatable,
                           const std::optional<bool>& postponeInternalCreation,
                           std::optional<size_t> stride, const std::optional<bool>& instanced,
                           const std::optional<size_t>& offset, const std::optional<size_t>& size,
                           std::optional<unsigned int> iType, bool iNormalized, bool useBytes,
                           unsigned int divisor)
    : instanceDivisor{this, &VertexBuffer::get_instanceDivisor, &VertexBuffer::set_instanceDivisor}
{
  if (std::holds_alternative<Buffer*>(data)) {
    _buffer      = std::get<Buffer*>(data);
    _ownedBuffer = nullptr;
    _ownsBuffer  = false;
  }
  else {
    _ownedBuffer = std::make_unique<Buffer>(
      engine, std::get<Float32Array>(data), updatable, stride,
      postponeInternalCreation.has_value() ? *postponeInternalCreation : false,
      instanced.has_value() ? *instanced : false);
    _buffer     = nullptr;
    _ownsBuffer = true;
  }

  _kind = kind;

  if (!iType.has_value()) {
    type = VertexBuffer::FLOAT;
  }
  else {
    type = *iType;
  }

  const auto typeByteLength = VertexBuffer::GetTypeByteLength(type);

  auto buffer = _buffer ? _buffer : _ownedBuffer.get();
  if (useBytes) {
    _size = size.has_value() ?
              *size :
              (stride ? (*stride / typeByteLength) : VertexBuffer::DeduceStride(kind));
    byteStride = stride.has_value() ?
                   *stride :
                   buffer->byteStride ? buffer->byteStride : (_size * typeByteLength);
    byteOffset = offset.has_value() ? *offset : 0ull;
  }
  else {
    _size      = size.has_value() ? *size : stride ? *stride : VertexBuffer::DeduceStride(kind);
    byteStride = stride.has_value() ?
                   (*stride * typeByteLength) :
                   (buffer->byteStride != 0 ? buffer->byteStride : (_size * typeByteLength));
    byteOffset = (offset.has_value() ? *offset : 0) * typeByteLength;
  }

  normalized = iNormalized;

  _instanced       = instanced.has_value() ? *instanced : false;
  _instanceDivisor = _instanced ? divisor : 0;
}

VertexBuffer::~VertexBuffer() = default;

unsigned int VertexBuffer::get_instanceDivisor() const
{
  return _instanceDivisor;
}

void VertexBuffer::set_instanceDivisor(unsigned int value)
{
  _instanceDivisor = value;
  _instanced       = value != 0;
}

void VertexBuffer::_rebuild()
{
  if (!_buffer) {
    return;
  }

  _buffer->_rebuild();
}

const std::string& VertexBuffer::getKind() const
{
  return _kind;
}

Buffer* VertexBuffer::_getBuffer() const
{
  if (_ownsBuffer) {
    return _ownedBuffer.get();
  }
  else {
    return _buffer;
  }
}

bool VertexBuffer::isUpdatable() const
{
  return _getBuffer()->isUpdatable();
}

Float32Array& VertexBuffer::getData()
{
  return _getBuffer()->getData();
}

WebGLDataBufferPtr& VertexBuffer::getBuffer()
{
  return _getBuffer()->getBuffer();
}

size_t VertexBuffer::getStrideSize() const
{
  return byteStride / VertexBuffer::GetTypeByteLength(type);
}

size_t VertexBuffer::getOffset() const
{
  return byteOffset / VertexBuffer::GetTypeByteLength(type);
}

size_t VertexBuffer::getSize() const
{
  return _size;
}

bool VertexBuffer::getIsInstanced() const
{
  return _instanced;
}

unsigned int VertexBuffer::getInstanceDivisor() const
{
  return _instanceDivisor;
}

WebGLDataBufferPtr VertexBuffer::create()
{
  return _getBuffer()->create();
}

WebGLDataBufferPtr VertexBuffer::create(const Float32Array& data)
{
  return _getBuffer()->create(data);
}

WebGLDataBufferPtr VertexBuffer::update(const Float32Array& data)
{
  return _getBuffer()->update(data);
}

WebGLDataBufferPtr VertexBuffer::updateDirectly(const Float32Array& data, size_t offset,
                                                bool useBytes)
{
  return _getBuffer()->updateDirectly(data, offset, std::nullopt, useBytes);
}

void VertexBuffer::dispose()
{
  if (_ownsBuffer && _ownedBuffer) {
    _ownedBuffer->dispose();
    _ownedBuffer = nullptr;
  }
}

void VertexBuffer::forEach(size_t count,
                           const std::function<void(float value, size_t index)>& callback)
{
  VertexBuffer::ForEach(_buffer->getData(), byteOffset, byteStride, _size, type, count, normalized,
                        callback);
}

size_t VertexBuffer::DeduceStride(const std::string& kind)
{
  // Deduce stride from kind
  if (kind == VertexBuffer::UVKind || kind == VertexBuffer::UV2Kind || kind == VertexBuffer::UV3Kind
      || kind == VertexBuffer::UV4Kind || kind == VertexBuffer::UV5Kind
      || kind == VertexBuffer::UV6Kind) {
    return 2;
  }
  else if (kind == VertexBuffer::NormalKind || kind == VertexBuffer::PositionKind) {
    return 3;
  }
  else if (kind == VertexBuffer::ColorKind || kind == VertexBuffer::MatricesIndicesKind
           || kind == VertexBuffer::MatricesIndicesExtraKind
           || kind == VertexBuffer::MatricesIndicesExtraKind
           || kind == VertexBuffer::MatricesWeightsKind
           || kind == VertexBuffer::MatricesWeightsExtraKind || kind == VertexBuffer::TangentKind) {
    return 4;
  }
  else {
    throw std::runtime_error(StringTools::printf("Invalid kind '%s'", kind.c_str()));
  }
}

unsigned int VertexBuffer::GetTypeByteLength(unsigned int type)
{
  switch (type) {
    case VertexBuffer::BYTE:
    case VertexBuffer::UNSIGNED_BYTE:
      return 1;
    case VertexBuffer::SHORT:
    case VertexBuffer::UNSIGNED_SHORT:
      return 2;
    case VertexBuffer::INT:
    case VertexBuffer::UNSIGNED_INT:
    case VertexBuffer::FLOAT:
      return 4;
    default:
      throw std::runtime_error("Invalid type " + std::to_string(type));
  }
}

void VertexBuffer::ForEach(const Float32Array& data, size_t byteOffset, size_t byteStride,
                           size_t componentCount, unsigned int /*componentType*/, size_t count,
                           bool /*normalized*/,
                           const std::function<void(float value, size_t index)>& callback)
{
  auto offset = byteOffset / 4;
  auto stride = byteStride / 4;
  for (size_t index = 0; index < count; index += componentCount) {
    for (size_t componentIndex = 0; componentIndex < componentCount; componentIndex++) {
      callback(data[offset + componentIndex], index + componentIndex);
    }
    offset += stride;
  }
}

void VertexBuffer::ForEach(const std::variant<ArrayBuffer, DataView>& data, size_t byteOffset,
                           size_t byteStride, size_t componentCount, unsigned int componentType,
                           size_t count, bool normalized,
                           const std::function<void(float value, size_t index)>& callback)
{
  DataView dataView = std::holds_alternative<ArrayBuffer>(data) ?
                        DataView(std::get<ArrayBuffer>(data)) :
                        DataView(std::get<DataView>(data));
  auto componentByteLength = VertexBuffer::GetTypeByteLength(componentType);
  for (size_t index = 0; index < count; index += componentCount) {
    auto componentByteOffset = byteOffset;
    for (size_t componentIndex = 0; componentIndex < componentCount; componentIndex++) {
      const auto value
        = VertexBuffer::_GetFloatValue(dataView, componentType, componentByteOffset, normalized);
      callback(value, index + componentIndex);
      componentByteOffset += componentByteLength;
    }
    byteOffset += byteStride;
  }
}

float VertexBuffer::_GetFloatValue(const DataView& dataView, unsigned int type, size_t byteOffset,
                                   bool normalized)
{
  switch (type) {
    case VertexBuffer::BYTE: {
      auto value = static_cast<float>(dataView.getInt8(byteOffset));
      if (normalized) {
        value = std::max(value / 127.f, -1.f);
      }
      return value;
    }
    case VertexBuffer::UNSIGNED_BYTE: {
      auto value = static_cast<float>(dataView.getUint8(byteOffset));
      if (normalized) {
        value = value / 255.f;
      }
      return value;
    }
    case VertexBuffer::SHORT: {
      auto value = static_cast<float>(dataView.getInt16(byteOffset, true));
      if (normalized) {
        value = std::max(value / 32767.f, -1.f);
      }
      return value;
    }
    case VertexBuffer::UNSIGNED_SHORT: {
      auto value = static_cast<float>(dataView.getUint16(byteOffset, true));
      if (normalized) {
        value = value / 65535.f;
      }
      return value;
    }
    case VertexBuffer::INT: {
      return static_cast<float>(dataView.getInt32(byteOffset, true));
    }
    case VertexBuffer::UNSIGNED_INT: {
      return static_cast<float>(dataView.getUint32(byteOffset, true));
    }
    case VertexBuffer::FLOAT: {
      return dataView.getFloat32(byteOffset, true);
    }
    default: {
      throw std::runtime_error("Invalid component type " + std::to_string(type));
    }
  }
}

} // end of namespace BABYLON
