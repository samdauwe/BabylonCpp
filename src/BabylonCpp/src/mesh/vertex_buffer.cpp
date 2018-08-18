#include <babylon/mesh/vertex_buffer.h>

#include <babylon/core/data_view.h>
#include <babylon/engine/engine.h>
#include <babylon/mesh/buffer.h>

namespace BABYLON {

constexpr unsigned int VertexBuffer::PositionKind;
constexpr unsigned int VertexBuffer::NormalKind;
constexpr unsigned int VertexBuffer::TangentKind;
constexpr unsigned int VertexBuffer::UVKind;
constexpr unsigned int VertexBuffer::UV2Kind;
constexpr unsigned int VertexBuffer::UV3Kind;
constexpr unsigned int VertexBuffer::UV4Kind;
constexpr unsigned int VertexBuffer::UV5Kind;
constexpr unsigned int VertexBuffer::UV6Kind;
constexpr unsigned int VertexBuffer::ColorKind;
constexpr unsigned int VertexBuffer::MatricesIndicesKind;
constexpr unsigned int VertexBuffer::MatricesWeightsKind;
constexpr unsigned int VertexBuffer::MatricesIndicesExtraKind;
constexpr unsigned int VertexBuffer::MatricesWeightsExtraKind;
constexpr unsigned int VertexBuffer::World0Kind;
constexpr unsigned int VertexBuffer::World1Kind;
constexpr unsigned int VertexBuffer::World2Kind;
constexpr unsigned int VertexBuffer::World3Kind;
constexpr unsigned int VertexBuffer::CellInfoKind;
constexpr unsigned int VertexBuffer::OptionsKind;
constexpr unsigned int VertexBuffer::AgeKind;
constexpr unsigned int VertexBuffer::LifeKind;
constexpr unsigned int VertexBuffer::VelocityKind;
constexpr unsigned int VertexBuffer::DirectionKind;
constexpr unsigned int VertexBuffer::InitialDirectionKind;
constexpr unsigned int VertexBuffer::OffsetKind;
constexpr unsigned int VertexBuffer::SeedKind;
constexpr unsigned int VertexBuffer::SizeKind;
constexpr unsigned int VertexBuffer::AngleKind;

constexpr const char* VertexBuffer::PositionKindChars;
constexpr const char* VertexBuffer::NormalKindChars;
constexpr const char* VertexBuffer::TangentKindChars;
constexpr const char* VertexBuffer::UVKindChars;
constexpr const char* VertexBuffer::UV2KindChars;
constexpr const char* VertexBuffer::UV3KindChars;
constexpr const char* VertexBuffer::UV4KindChars;
constexpr const char* VertexBuffer::UV5KindChars;
constexpr const char* VertexBuffer::UV6KindChars;
constexpr const char* VertexBuffer::ColorKindChars;
constexpr const char* VertexBuffer::MatricesIndicesKindChars;
constexpr const char* VertexBuffer::MatricesWeightsKindChars;
constexpr const char* VertexBuffer::MatricesIndicesExtraKindChars;
constexpr const char* VertexBuffer::MatricesWeightsExtraKindChars;
constexpr const char* VertexBuffer::World0KindChars;
constexpr const char* VertexBuffer::World1KindChars;
constexpr const char* VertexBuffer::World2KindChars;
constexpr const char* VertexBuffer::World3KindChars;
constexpr const char* VertexBuffer::CellInfoKindChars;
constexpr const char* VertexBuffer::OptionsKindChars;
constexpr const char* VertexBuffer::AgeKindChars;
constexpr const char* VertexBuffer::LifeKindChars;
constexpr const char* VertexBuffer::VelocityKindChars;
constexpr const char* VertexBuffer::DirectionKindChars;
constexpr const char* VertexBuffer::InitialDirectionKindChars;
constexpr const char* VertexBuffer::OffsetKindChars;
constexpr const char* VertexBuffer::SeedKindChars;
constexpr const char* VertexBuffer::SizeKindChars;
constexpr const char* VertexBuffer::AngleKindChars;

constexpr const unsigned int VertexBuffer::BYTE;
constexpr const unsigned int VertexBuffer::UNSIGNED_BYTE;
constexpr const unsigned int VertexBuffer::SHORT;
constexpr const unsigned int VertexBuffer::UNSIGNED_SHORT;
constexpr const unsigned int VertexBuffer::INT;
constexpr const unsigned int VertexBuffer::UNSIGNED_INT;
constexpr const unsigned int VertexBuffer::FLOAT;

VertexBuffer::VertexBuffer(
  Engine* engine, const Variant<Float32Array, Buffer*> data, unsigned int kind,
  bool updatable, const nullable_t<bool>& postponeInternalCreation,
  nullable_t<size_t> stride, const nullable_t<bool>& instanced,
  const nullable_t<unsigned int>& offset, const nullable_t<size_t>& size,
  nullable_t<unsigned int> iType, bool iNormalized, bool useBytes)
    : instanceDivisor{this, &VertexBuffer::get_instanceDivisor,
                      &VertexBuffer::set_instanceDivisor}
{
  if (data.is<Buffer*>()) {
    _buffer      = data.get<Buffer*>();
    _ownedBuffer = nullptr;
    _ownsBuffer  = false;
  }
  else {
    _ownedBuffer = ::std::make_unique<Buffer>(
      engine, data.get<Float32Array>(), updatable, stride,
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
    byteOffset = offset.has_value() ? *offset : 0;
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

string_t VertexBuffer::KindAsString(unsigned int kind)
{

  switch (kind) {
    case VertexBuffer::PositionKind:
    default:
      return string_t(VertexBuffer::PositionKindChars);
    case VertexBuffer::NormalKind:
      return string_t(VertexBuffer::NormalKindChars);
    case VertexBuffer::TangentKind:
      return string_t(VertexBuffer::TangentKindChars);
    case VertexBuffer::UVKind:
      return string_t(VertexBuffer::UVKindChars);
    case VertexBuffer::UV2Kind:
      return string_t(VertexBuffer::UV2KindChars);
    case VertexBuffer::UV3Kind:
      return string_t(VertexBuffer::UV3KindChars);
    case VertexBuffer::UV4Kind:
      return string_t(VertexBuffer::UV4KindChars);
    case VertexBuffer::UV5Kind:
      return string_t(VertexBuffer::UV5KindChars);
    case VertexBuffer::UV6Kind:
      return string_t(VertexBuffer::UV6KindChars);
    case VertexBuffer::ColorKind:
      return string_t(VertexBuffer::ColorKindChars);
    case VertexBuffer::MatricesIndicesKind:
      return string_t(VertexBuffer::MatricesIndicesKindChars);
    case VertexBuffer::MatricesWeightsKind:
      return string_t(VertexBuffer::MatricesWeightsKindChars);
    case VertexBuffer::MatricesIndicesExtraKind:
      return string_t(VertexBuffer::MatricesIndicesExtraKindChars);
    case VertexBuffer::MatricesWeightsExtraKind:
      return string_t(VertexBuffer::MatricesWeightsExtraKindChars);
    case VertexBuffer::World0Kind:
      return string_t(VertexBuffer::World0KindChars);
    case VertexBuffer::World1Kind:
      return string_t(VertexBuffer::World1KindChars);
    case VertexBuffer::World2Kind:
      return string_t(VertexBuffer::World2KindChars);
    case VertexBuffer::World3Kind:
      return string_t(VertexBuffer::World3KindChars);
    case VertexBuffer::CellInfoKind:
      return string_t(VertexBuffer::CellInfoKindChars);
    case VertexBuffer::OptionsKind:
      return string_t(VertexBuffer::OptionsKindChars);
    case VertexBuffer::AgeKind:
      return string_t(VertexBuffer::AgeKindChars);
    case VertexBuffer::LifeKind:
      return string_t(VertexBuffer::LifeKindChars);
    case VertexBuffer::VelocityKind:
      return string_t(VertexBuffer::VelocityKindChars);
    case VertexBuffer::DirectionKind:
      return string_t(VertexBuffer::DirectionKindChars);
    case VertexBuffer::InitialDirectionKind:
      return string_t(VertexBuffer::InitialDirectionKindChars);
    case VertexBuffer::OffsetKind:
      return string_t(VertexBuffer::OffsetKindChars);
    case VertexBuffer::SeedKind:
      return string_t(VertexBuffer::SeedKindChars);
    case VertexBuffer::SizeKind:
      return string_t(VertexBuffer::SizeKindChars);
    case VertexBuffer::AngleKind:
      return string_t(VertexBuffer::AngleKindChars);
  }
}

void VertexBuffer::_rebuild()
{
  if (!_buffer) {
    return;
  }

  _buffer->_rebuild();
}

unsigned int VertexBuffer::getKind() const
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
  return _getBuffer()->updateDirectly(data, offset, nullopt_t, useBytes);
}

void VertexBuffer::dispose()
{
  if (_ownsBuffer && _ownedBuffer) {
    _ownedBuffer->dispose();
    _ownedBuffer = nullptr;
  }
}

void VertexBuffer::forEach(
  size_t count,
  const ::std::function<void(float value, size_t index)>& callback)
{
  VertexBuffer::ForEach(_buffer->getData(), byteOffset, byteStride, _size, type,
                        count, normalized, callback);
}

size_t VertexBuffer::DeduceStride(unsigned int kind)
{
  // Deduce stride from kind
  switch (kind) {
    case VertexBuffer::UVKind:
    case VertexBuffer::UV2Kind:
    case VertexBuffer::UV3Kind:
    case VertexBuffer::UV4Kind:
    case VertexBuffer::UV5Kind:
    case VertexBuffer::UV6Kind:
      return 2;
    case VertexBuffer::NormalKind:
    case VertexBuffer::PositionKind:
      return 3;
    case VertexBuffer::ColorKind:
    case VertexBuffer::MatricesIndicesKind:
    case VertexBuffer::MatricesIndicesExtraKind:
    case VertexBuffer::MatricesWeightsKind:
    case VertexBuffer::MatricesWeightsExtraKind:
    case VertexBuffer::TangentKind:
      return 4;
    default:
      throw ::std::runtime_error("Invalid kind '" + ::std::to_string(kind)
                                 + "'");
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
      throw ::std::runtime_error("Invalid type " + ::std::to_string(type));
  }
}

void VertexBuffer::ForEach(
  const Float32Array& data, size_t byteOffset, size_t byteStride,
  size_t componentCount, unsigned int /*componentType*/, size_t count,
  bool /*normalized*/,
  const ::std::function<void(float value, size_t index)>& callback)
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
  const Variant<ArrayBuffer, DataView>& data, size_t byteOffset,
  size_t byteStride, size_t componentCount, unsigned int componentType,
  size_t count, bool normalized,
  const ::std::function<void(float value, size_t index)>& callback)
{
  DataView dataView = data.is<ArrayBuffer>() ?
                        DataView(data.get<ArrayBuffer>()) :
                        DataView(data.get<DataView>());
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
        value = ::std::max(value / 127.f, -1.f);
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
        value = ::std::max(value / 16383.f, -1.f);
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
      throw ::std::runtime_error("Invalid component type "
                                 + ::std::to_string(type));
    }
  }
}

} // end of namespace BABYLON
