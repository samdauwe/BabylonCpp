#include <babylon/mesh/vertex_buffer.h>

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
constexpr unsigned int VertexBuffer::OffsetKind;
constexpr unsigned int VertexBuffer::SeedKind;
constexpr unsigned int VertexBuffer::SizeKind;

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
constexpr const char* VertexBuffer::OffsetKindChars;
constexpr const char* VertexBuffer::SeedKindChars;
constexpr const char* VertexBuffer::SizeKindChars;

VertexBuffer::VertexBuffer(
  Engine* engine, const Variant<Float32Array, Buffer*> data, unsigned int kind,
  bool updatable, const Nullable<bool>& postponeInternalCreation,
  Nullable<int> stride, const Nullable<bool>& instanced,
  const Nullable<unsigned int>& offset, const Nullable<int>& size)
    : instanceDivisor{this, &VertexBuffer::get_instanceDivisor,
                      &VertexBuffer::set_instanceDivisor}
{
  if (data.is<Buffer*>()) {
    if (!stride) {
      stride = data.get<Buffer*>()->getStrideSize();
    }
    _buffer      = data.get<Buffer*>();
    _ownedBuffer = nullptr;
    _ownsBuffer  = false;
  }
  else {
    if (!stride) {
      stride = VertexBuffer::DeduceStride(kind);
    }
    _ownedBuffer = ::std::make_unique<Buffer>(
      engine, data.get<Float32Array>(), updatable, stride,
      postponeInternalCreation ? *postponeInternalCreation : false, instanced);
    _buffer     = nullptr;
    _ownsBuffer = true;
  }

  _stride = stride;

  _instanced       = instanced.hasValue() ? *instanced : false;
  _instanceDivisor = instanced ? 1 : 0;

  _offset = offset ? *offset : 0;
  _size   = size ? size : stride;

  _kind = kind;
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
    case VertexBuffer::OffsetKind:
      return string_t(VertexBuffer::OffsetKindChars);
    case VertexBuffer::SeedKind:
      return string_t(VertexBuffer::SeedKindChars);
    case VertexBuffer::SizeKind:
      return string_t(VertexBuffer::SizeKindChars);
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

int VertexBuffer::getStrideSize() const
{
  return _stride;
}

unsigned int VertexBuffer::getOffset() const
{
  return _offset;
}

int VertexBuffer::getSize() const
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
                                            int offset)
{
  return _getBuffer()->updateDirectly(data, offset);
}

void VertexBuffer::dispose()
{
  if (_ownsBuffer && _ownedBuffer) {
    _ownedBuffer->dispose();
    _ownedBuffer = nullptr;
  }
}

int VertexBuffer::DeduceStride(unsigned int kind)
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
  return -1;
}

} // end of namespace BABYLON
