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

VertexBuffer::VertexBuffer(Engine* engine, const Float32Array& data,
                           unsigned int kind, bool updatable,
                           bool postponeInternalCreation, int stride,
                           bool instanced, int offset, int size)
    : _ownedBuffer{nullptr}, _buffer{nullptr}, _kind{kind}, _ownsBuffer{true}
{
  // Deduce stride from kind
  _stride = (stride == -1) ? VertexBuffer::KindToStride(kind) : stride;
  _stride = (_stride == -1) ? 3 : _stride;

  _ownedBuffer = ::std::make_unique<Buffer>(
    engine, data, updatable, _stride, postponeInternalCreation, instanced);

  _offset = (offset != -1) ? static_cast<unsigned int>(offset) : 0;
  _size   = (size != -1) ? size : _stride;
}

VertexBuffer::VertexBuffer(Engine* /*engine*/, Buffer* buffer,
                           unsigned int kind, bool /*updatable*/,
                           bool /*postponeInternalCreation*/, int stride,
                           bool /*instanced*/, int offset, int size)
    : _ownedBuffer{nullptr}, _buffer{buffer}, _kind{kind}, _ownsBuffer{false}
{
  // Deduce stride from kind
  _stride = (stride == -1) ? VertexBuffer::KindToStride(kind) : stride;
  _stride = (_stride == -1) ? buffer->getStrideSize() : _stride;

  _offset = (offset != -1) ? static_cast<unsigned int>(offset) : 0;
  _size   = (size != -1) ? size : _stride;
}

VertexBuffer::~VertexBuffer()
{
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
  }
}

int VertexBuffer::KindToStride(unsigned int kind)
{
  int stride = -1;
  // Deduce stride from kind
  switch (kind) {
    case VertexBuffer::PositionKind:
      stride = 3;
      break;
    case VertexBuffer::NormalKind:
      stride = 3;
      break;
    case VertexBuffer::UVKind:
    case VertexBuffer::UV2Kind:
    case VertexBuffer::UV3Kind:
    case VertexBuffer::UV4Kind:
    case VertexBuffer::UV5Kind:
    case VertexBuffer::UV6Kind:
      stride = 2;
      break;
    case VertexBuffer::TangentKind:
    case VertexBuffer::ColorKind:
      stride = 4;
      break;
    case VertexBuffer::MatricesIndicesKind:
    case VertexBuffer::MatricesIndicesExtraKind:
      stride = 4;
      break;
    case VertexBuffer::MatricesWeightsKind:
    case VertexBuffer::MatricesWeightsExtraKind:
      stride = 4;
      break;
    default:
      break;
  }
  return stride;
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
  return _getBuffer()->getIsInstanced();
}

unsigned int VertexBuffer::getInstanceDivisor() const
{
  return _getBuffer()->instanceDivisor();
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

void VertexBuffer::dispose(bool /*doNotRecurse*/)
{
  if (_ownsBuffer && _ownedBuffer) {
    _ownedBuffer->dispose();
    _ownedBuffer.reset(nullptr);
  }
}

} // end of namespace BABYLON
