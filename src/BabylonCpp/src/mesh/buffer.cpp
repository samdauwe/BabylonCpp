#include <babylon/mesh/buffer.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

Buffer::Buffer(Engine* engine, const Float32Array& data, bool updatable,
               int stride, bool postponeInternalCreation, bool instanced)
    : _engine{engine}
    , _buffer{nullptr}
    , _data{data}
    , _updatable{updatable}
    , _strideSize{stride}
    , _instanced{instanced}
{
  if (!postponeInternalCreation) { // by default
    create();
  }
}

Buffer::Buffer(Mesh* mesh, const Float32Array& data, bool updatable, int stride,
               bool postponeInternalCreation, bool instanced)
    : _buffer{nullptr}
    , _data{data}
    , _updatable{updatable}
    , _strideSize{stride}
    , _instanced{instanced}
{
  // old versions of BABYLON.VertexBuffer accepted 'mesh' instead of 'engine'
  _engine = mesh->getScene()->getEngine();
  if (!postponeInternalCreation) { // by default
    create();
  }
}

Buffer::~Buffer()
{
}

std::unique_ptr<VertexBuffer> Buffer::createVertexBuffer(unsigned int kind,
                                                         int offset, int size)
{
  // a lot of these parameters are ignored as they are overriden by the buffer
  return std_util::make_unique<VertexBuffer>(_engine, this, kind, _updatable,
                                             true, _strideSize, _instanced,
                                             offset, size);
}

std::unique_ptr<VertexBuffer>
Buffer::createVertexBuffer(unsigned int kind, int offset, int size, int stride)
{
  // a lot of these parameters are ignored as they are overriden by the buffer
  return std_util::make_unique<VertexBuffer>(
    _engine, this, kind, _updatable, true, stride, _instanced, offset, size);
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

int Buffer::getStrideSize() const
{
  return _strideSize;
}

bool Buffer::getIsInstanced() const
{
  return _instanced;
}

// Methods
GL::IGLBuffer* Buffer::create()
{
  if (_buffer) {
    return nullptr; // nothing to do
  }

  if (!_buffer) { // create buffer
    if (_updatable) {
      _buffer = _engine->createDynamicVertexBuffer(_data);
    }
    else {
      _buffer = _engine->createVertexBuffer(_data);
    }
  }
  else if (_updatable) { // update buffer
    _engine->updateDynamicVertexBuffer(_buffer, _data);
  }

  return _buffer ? _buffer.get() : nullptr;
}

GL::IGLBuffer* Buffer::create(const Float32Array& data)
{
  if (data.empty() && _buffer) {
    return nullptr; // nothing to do
  }

  if (!_buffer) { // create buffer
    if (_updatable) {
      _buffer = _engine->createDynamicVertexBuffer(data.empty() ? _data : data);
      if (!data.empty()) {
        _data = data;
      }
    }
    else {
      _buffer = _engine->createVertexBuffer(data.empty() ? _data : data);
    }
  }
  else if (_updatable) { // update buffer
    _engine->updateDynamicVertexBuffer(_buffer, data.empty() ? _data : data);
    if (!data.empty()) {
      _data = data;
    }
  }

  return _buffer ? _buffer.get() : nullptr;
}

GL::IGLBuffer* Buffer::update(const Float32Array& data)
{
  return create(data);
}

GL::IGLBuffer* Buffer::updateDirectly(const Float32Array& data, int offset)
{
  if (!_buffer) {
    return nullptr;
  }

  if (_updatable) { // update buffer
    _engine->updateDynamicVertexBuffer(_buffer, data, offset, -1);
    _data.clear();
  }

  return _buffer ? _buffer.get() : nullptr;
}

GL::IGLBuffer* Buffer::updateDirectly(const Float32Array& data, int offset,
                                      size_t vertexCount)
{
  if (!_buffer) {
    return nullptr;
  }

  if (_updatable) { // update buffer
    _engine->updateDynamicVertexBuffer(
      _buffer, data, offset, static_cast<int>(vertexCount) * getStrideSize());
    _data.clear();
  }

  return _buffer ? _buffer.get() : nullptr;
}

void Buffer::dispose(bool /*doNotRecurse*/)
{
  if (!_buffer) {
    return;
  }
  if (_engine->_releaseBuffer(_buffer.get())) {
    _buffer.reset(nullptr);
  }
}

} // end of namespace BABYLON
