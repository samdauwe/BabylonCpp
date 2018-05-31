#include <babylon/mesh/buffer.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

Buffer::Buffer(Engine* engine, const Float32Array& data, bool updatable,
               Nullable<size_t> stride, bool postponeInternalCreation,
               bool instanced, bool useBytes)
    : _data{data}
    , _engine{engine ? engine : Engine::LastCreatedEngine()}
    , _buffer{nullptr}
    , _updatable{updatable}
    , _instanced{instanced}
{
  if (!stride) {
    stride = 0ul;
  }

  byteStride = useBytes ? *stride : *stride * sizeof(float);

  if (!postponeInternalCreation) { // by default
    create();
  }
}

Buffer::Buffer(Mesh* mesh, const Float32Array& data, bool updatable,
               Nullable<size_t> stride, bool postponeInternalCreation,
               bool instanced, bool useBytes)
    : _data{data}
    , _engine{mesh->getScene()->getEngine()}
    , _buffer{nullptr}
    , _updatable{updatable}
    , _instanced{instanced}
{
  if (!stride) {
    stride = 0ul;
  }

  byteStride = useBytes ? *stride : *stride * sizeof(float);

  if (!postponeInternalCreation) { // by default
    create();
  }
}

Buffer::~Buffer()
{
}

unique_ptr_t<VertexBuffer> Buffer::createVertexBuffer(unsigned int kind,
                                                      size_t offset, int size,
                                                      Nullable<size_t> stride,
                                                      Nullable<bool> instanced,
                                                      bool useBytes)
{
  const auto _byteOffset = useBytes ? offset : offset * sizeof(float);
  const auto _byteStride
    = stride ? (useBytes ? *stride : *stride * sizeof(float)) : byteStride;

  // a lot of these parameters are ignored as they are overriden by the buffer
  return ::std::make_unique<VertexBuffer>(
    _engine, ToVariant<Float32Array, Buffer*>(this), kind, _updatable, true,
    _byteStride, instanced.isNull() ? _instanced : _byteOffset, offset, size,
    nullptr, false, true);
}

// Properties
bool Buffer::isUpdatable() const
{
  return _updatable;
}

Float32Array& Buffer::getData()
{
  return _data;
}

GL::IGLBuffer* Buffer::getBuffer()
{
  return _buffer.get();
}

size_t Buffer::getStrideSize() const
{
  return byteStride / sizeof(float);
}

// Methods
GL::IGLBuffer* Buffer::create(Float32Array data)
{
  if (_buffer) {
    return nullptr; // nothing to do
  }

  if (data.empty()) {
    data = _data;
  }

  if (data.empty()) {
    return nullptr;
  }

  if (!_buffer) { // create buffer
    if (_updatable) {
      _buffer = _engine->createDynamicVertexBuffer(data);
      _data   = ::std::move(data);
    }
    else {
      _buffer = _engine->createVertexBuffer(data);
    }
  }
  else if (_updatable) { // update buffer
    _engine->updateDynamicVertexBuffer(_buffer, data);
    _data = ::std::move(data);
  }

  return _buffer ? _buffer.get() : nullptr;
}

void Buffer::_rebuild()
{
  _buffer = nullptr;
  create(_data);
}

GL::IGLBuffer* Buffer::update(const Float32Array& data)
{
  return create(data);
}

GL::IGLBuffer* Buffer::updateDirectly(const Float32Array& data, size_t offset,
                                      const Nullable<size_t>& vertexCount,
                                      bool useBytes)
{
  if (!_buffer) {
    return nullptr;
  }

  if (_updatable) { // update buffer
    _engine->updateDynamicVertexBuffer(
      _buffer, data,
      useBytes ? static_cast<int>(offset) :
                 static_cast<int>(offset * sizeof(float)),
      (vertexCount ? static_cast<int>(*vertexCount * byteStride) : -1));
    _data.clear();
  }

  return _buffer ? _buffer.get() : nullptr;
}

void Buffer::dispose()
{
  if (!_buffer) {
    return;
  }
  if (_engine->_releaseBuffer(_buffer.get())) {
    _buffer.reset(nullptr);
  }
}

} // end of namespace BABYLON
