#include <babylon/mesh/vertex_buffer.h>

#include <babylon/core/data_view.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/mesh/buffer.h>

namespace BABYLON {

constexpr const char* VertexBuffer::PositionKind;
constexpr const char* VertexBuffer::NormalKind;
constexpr const char* VertexBuffer::TangentKind;
constexpr const char* VertexBuffer::UVKind;
constexpr const char* VertexBuffer::UV2Kind;
constexpr const char* VertexBuffer::UV3Kind;
constexpr const char* VertexBuffer::UV4Kind;
constexpr const char* VertexBuffer::UV5Kind;
constexpr const char* VertexBuffer::UV6Kind;
constexpr const char* VertexBuffer::ColorKind;
constexpr const char* VertexBuffer::MatricesIndicesKind;
constexpr const char* VertexBuffer::MatricesWeightsKind;
constexpr const char* VertexBuffer::MatricesIndicesExtraKind;
constexpr const char* VertexBuffer::MatricesWeightsExtraKind;
constexpr const char* VertexBuffer::World0Kind;
constexpr const char* VertexBuffer::World1Kind;
constexpr const char* VertexBuffer::World2Kind;
constexpr const char* VertexBuffer::World3Kind;
constexpr const char* VertexBuffer::CellInfoKind;
constexpr const char* VertexBuffer::OptionsKind;
constexpr const char* VertexBuffer::AgeKind;
constexpr const char* VertexBuffer::LifeKind;
constexpr const char* VertexBuffer::VelocityKind;
constexpr const char* VertexBuffer::DirectionKind;
constexpr const char* VertexBuffer::InitialDirectionKind;
constexpr const char* VertexBuffer::OffsetKind;
constexpr const char* VertexBuffer::SeedKind;
constexpr const char* VertexBuffer::SizeKind;
constexpr const char* VertexBuffer::AngleKind;
constexpr const char* VertexBuffer::CellStartOffsetKind;
constexpr const char* VertexBuffer::NoiseCoordinates1Kind;
constexpr const char* VertexBuffer::NoiseCoordinates2Kind;
constexpr const char* VertexBuffer::RemapDataKind;

constexpr const unsigned int VertexBuffer::BYTE;
constexpr const unsigned int VertexBuffer::UNSIGNED_BYTE;
constexpr const unsigned int VertexBuffer::SHORT;
constexpr const unsigned int VertexBuffer::UNSIGNED_SHORT;
constexpr const unsigned int VertexBuffer::INT;
constexpr const unsigned int VertexBuffer::UNSIGNED_INT;
constexpr const unsigned int VertexBuffer::FLOAT;

VertexBuffer::VertexBuffer(
  Engine* engine, const std::variant<Float32Array, Buffer*> data,
  const std::string& kind, bool updatable,
  const std::optional<bool>& postponeInternalCreation,
  std::optional<size_t> stride, const std::optional<bool>& instanced,
  const std::optional<size_t>& offset, const std::optional<size_t>& size,
  std::optional<unsigned int> iType, bool iNormalized, bool useBytes)
    : instanceDivisor{this, &VertexBuffer::get_instanceDivisor,
                      &VertexBuffer::set_instanceDivisor}
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
    _size = size.has_value() ? *size :
                               (stride ? (*stride / typeByteLength) :
                                         VertexBuffer::DeduceStride(kind));
    byteStride
      = stride.has_value() ?
          *stride :
          buffer->byteStride ? buffer->byteStride : (_size * typeByteLength);
    byteOffset = offset.has_value() ? *offset : 0ull;
  }
  else {
    _size = size.has_value() ?
              *size :
              stride ? *stride : VertexBuffer::DeduceStride(kind);
    byteStride = stride.has_value() ?
                   (*stride * typeByteLength) :
                   (buffer->byteStride != 0 ? buffer->byteStride :
                                              (_size * typeByteLength));
    byteOffset = (offset.has_value() ? *offset : 0) * typeByteLength;
  }

  normalized = iNormalized;

  _instanced       = instanced.has_value() ? *instanced : false;
  _instanceDivisor = _instanced ? 1 : 0;
}

VertexBuffer::~VertexBuffer()
{
}

unsigned int VertexBuffer::get_instanceDivisor() const
{
  return _instanceDivisor;
}

void VertexBuffer::set_instanceDivisor(unsigned int value)
{
  _instanceDivisor = value;
  if (value == 0) {
    _instanced = false;
  }
  else {
    _instanced = true;
  }
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

GL::IGLBuffer* VertexBuffer::getBuffer()
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

// Methods
GL::IGLBuffer* VertexBuffer::create()
{
  return _getBuffer()->create();
}

GL::IGLBuffer* VertexBuffer::create(const Float32Array& data)
{
  return _getBuffer()->create(data);
}

GL::IGLBuffer* VertexBuffer::update(const Float32Array& data)
{
  return _getBuffer()->update(data);
}

GL::IGLBuffer* VertexBuffer::updateDirectly(const Float32Array& data,
                                            size_t offset, bool useBytes)
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

void VertexBuffer::forEach(
  size_t count, const std::function<void(float value, size_t index)>& callback)
{
  VertexBuffer::ForEach(_buffer->getData(), byteOffset, byteStride, _size, type,
                        count, normalized, callback);
}

size_t VertexBuffer::DeduceStride(const std::string& kind)
{
  // Deduce stride from kind
  if (kind == VertexBuffer::UVKind || kind == VertexBuffer::UV2Kind
      || kind == VertexBuffer::UV3Kind || kind == VertexBuffer::UV4Kind
      || kind == VertexBuffer::UV5Kind || kind == VertexBuffer::UV6Kind) {
    return 2;
  }
  else if (kind == VertexBuffer::NormalKind
           || kind == VertexBuffer::PositionKind) {
    return 3;
  }
  else if (kind == VertexBuffer::ColorKind
           || kind == VertexBuffer::MatricesIndicesKind
           || kind == VertexBuffer::MatricesIndicesExtraKind
           || kind == VertexBuffer::MatricesIndicesExtraKind
           || kind == VertexBuffer::MatricesWeightsKind
           || kind == VertexBuffer::MatricesWeightsExtraKind
           || kind == VertexBuffer::TangentKind) {
    return 4;
  }
  else {
    throw std::runtime_error(String::printf("Invalid kind '%s'", kind.c_str()));
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
    case VertexBuffer::FLOAT:
      return 4;
    default:
      throw std::runtime_error("Invalid type " + std::to_string(type));
  }
}

void VertexBuffer::ForEach(
  const Float32Array& data, size_t byteOffset, size_t byteStride,
  size_t componentCount, unsigned int /*componentType*/, size_t count,
  bool /*normalized*/,
  const std::function<void(float value, size_t index)>& callback)
{
  auto offset = byteOffset / 4;
  auto stride = byteStride / 4;
  for (size_t index = 0; index < count; index += componentCount) {
    for (size_t componentIndex = 0; componentIndex < componentCount;
         componentIndex++) {
      callback(data[offset + componentIndex], index + componentIndex);
    }
    offset += stride;
  }
}

void VertexBuffer::ForEach(
  const std::variant<ArrayBuffer, DataView>& data, size_t byteOffset,
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
    for (size_t componentIndex = 0; componentIndex < componentCount;
         componentIndex++) {
      const auto value = VertexBuffer::_GetFloatValue(
        dataView, componentType, componentByteOffset, normalized);
      callback(value, index + componentIndex);
      componentByteOffset += componentByteLength;
    }
    byteOffset += byteStride;
  }
}

float VertexBuffer::_GetFloatValue(const DataView& dataView, unsigned int type,
                                   size_t byteOffset, bool normalized)
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
        value = std::max(value / 16383.f, -1.f);
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
    case VertexBuffer::FLOAT: {
      return dataView.getFloat32(byteOffset, true);
    }
    default: {
      throw std::runtime_error("Invalid component type "
                               + std::to_string(type));
    }
  }
}

} // end of namespace BABYLON
