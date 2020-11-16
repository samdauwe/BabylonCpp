#include <babylon/meshes/buffer.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/webgl/webgl_data_buffer.h>

namespace BABYLON {

Buffer::Buffer(ThinEngine* engine, const Float32Array& data, bool updatable,
               std::optional<size_t> stride, bool postponeInternalCreation, bool instanced,
               bool useBytes, const std::optional<unsigned int>& divisor)
    : _buffer{nullptr}, _isAlreadyOwned{false}
{
  _engine    = engine ? engine : Engine::LastCreatedEngine();
  _updatable = updatable;
  _instanced = instanced;
  _divisor   = divisor.value_or(1);

  _data = data;

  if (!stride.has_value()) {
    stride = 0ull;
  }

  byteStride = useBytes ? *stride : *stride * sizeof(float);

  if (!postponeInternalCreation) { // by default
    create();
  }
}

Buffer::Buffer(Mesh* mesh, const Float32Array& data, bool updatable, std::optional<size_t> stride,
               bool postponeInternalCreation, bool instanced, bool useBytes,
               const std::optional<unsigned int>& divisor)
    : _buffer{nullptr}
{
  _engine    = mesh->getScene()->getEngine();
  _updatable = updatable;
  _instanced = instanced;
  _divisor   = divisor.value_or(1);

  _data = data;

  if (!stride.has_value()) {
    stride = 0ull;
  }

  byteStride = useBytes ? *stride : *stride * sizeof(float);

  if (!postponeInternalCreation) { // by default
    create();
  }
}

Buffer::~Buffer() = default;

std::unique_ptr<VertexBuffer> Buffer::createVertexBuffer(const std::string& kind, size_t offset,
                                                         size_t size, std::optional<size_t> stride,
                                                         std::optional<bool> instanced,
                                                         bool useBytes,
                                                         const std::optional<unsigned int>& divisor)
{
  const auto _byteOffset = useBytes ? offset : offset * sizeof(float);
  const auto _byteStride
    = stride.has_value() ? (useBytes ? *stride : *stride * sizeof(float)) : byteStride;

  // a lot of these parameters are ignored as they are overriden by the buffer
  return std::make_unique<VertexBuffer>(_engine, this, kind, _updatable, true, _byteStride,
                                        !instanced.has_value() ? _instanced : *instanced,
                                        _byteOffset, size, std::nullopt, false, true,
                                        divisor.value_or(_divisor));
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

WebGLDataBufferPtr& Buffer::getBuffer()
{
  return _buffer;
}

size_t Buffer::getStrideSize() const
{
  return byteStride / sizeof(float);
}

// Methods
WebGLDataBufferPtr Buffer::create(Float32Array data)
{
  if (data.empty() && _buffer) {
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
      _data   = std::move(data);
    }
    else {
      _buffer = _engine->createVertexBuffer(data);
    }
  }
  else if (_updatable) { // update buffer
    _engine->updateDynamicVertexBuffer(_buffer, data);
    _data = std::move(data);
  }
  else { // Update data
    _data = std::move(data);
  }

  return _buffer;
}

void Buffer::_rebuild()
{
  _buffer = nullptr;
  create(_data);
}

WebGLDataBufferPtr Buffer::update(const Float32Array& data)
{
  return create(data);
}

WebGLDataBufferPtr Buffer::updateDirectly(const Float32Array& data, size_t offset,
                                          const std::optional<size_t>& vertexCount, bool useBytes)
{
  if (!_buffer) {
    return nullptr;
  }

  if (_updatable) { // update buffer
    _engine->updateDynamicVertexBuffer(
      _buffer, data, useBytes ? static_cast<int>(offset) : static_cast<int>(offset * sizeof(float)),
      (vertexCount.has_value() ? static_cast<int>(*vertexCount * byteStride) : -1));
    _data.clear();
  }

  return _buffer;
}

void Buffer::_increaseReferences()
{
  if (!_buffer) {
    return;
  }

  if (!_isAlreadyOwned) {
    _isAlreadyOwned = true;
    return;
  }

  _buffer->references++;
}

void Buffer::dispose()
{
  if (!_buffer) {
    return;
  }
  if (_engine->_releaseBuffer(_buffer)) {
    _buffer = nullptr;
  }
}

} // end of namespace BABYLON
