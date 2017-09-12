#include <babylon/materials/uniform_buffer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

Float32Array UniformBuffer::_tempBuffer{};

UniformBuffer::UniformBuffer(Engine* engine, const Float32Array& data,
                             bool dynamic)
    : _engine{engine}
    , _data{data}
    , _dynamic{dynamic}
    , _uniformLocationPointer{0}
    , _needSync{false}
    , _noUBO{engine->webGLVersion() == 1.f}
{
  if (_noUBO) {
    updateMatrix3x3
      = [this](const std::string& name, const Float32Array& matrix) {
          _updateMatrix3x3ForEffect(name, matrix);
        };
    updateMatrix2x2
      = [this](const std::string& name, const Float32Array& matrix) {
          _updateMatrix2x2ForEffect(name, matrix);
        };
    updateFloat = [this](const std::string& name, float x) {
      _updateFloatForEffect(name, x);
    };
    updateFloat2 = [this](const std::string& name, float x, float y,
                          const std::string& suffix) {
      _updateFloat2ForEffect(name, x, y, suffix);
    };
    updateFloat3 = [this](const std::string& name, float x, float y, float z,
                          const std::string& suffix = "") {
      _updateFloat3ForEffect(name, x, y, z, suffix);
    };
    updateFloat4 = [this](const std::string& name, float x, float y, float z,
                          float w, const std::string& suffix = "") {
      _updateFloat4ForEffect(name, x, y, z, w, suffix);
    };
    updateMatrix = [this](const std::string& name, const Matrix& mat) {
      _updateMatrixForEffect(name, mat);
    };
    updateVector3 = [this](const std::string& name, const Vector3& vector) {
      _updateVector3ForEffect(name, vector);
    };
    updateVector4 = [this](const std::string& name, const Vector4& vector) {
      _updateVector4ForEffect(name, vector);
    };
    updateColor3 = [this](const std::string& name, const Color3& color,
                          const std::string& suffix = "") {
      _updateColor3ForEffect(name, color, suffix);
    };
    updateColor4 = [this](const std::string& name, const Color3& color,
                          float alpha, const std::string& suffix = "") {
      _updateColor4ForEffect(name, color, alpha, suffix);
    };
  }
  else {
    _engine->_uniformBuffers.emplace_back(this);

    updateMatrix3x3
      = [this](const std::string& name, const Float32Array& matrix) {
          _updateMatrix3x3ForUniform(name, matrix);
        };
    updateMatrix2x2
      = [this](const std::string& name, const Float32Array& matrix) {
          _updateMatrix2x2ForUniform(name, matrix);
        };
    updateFloat = [this](const std::string& name, float x) {
      _updateFloatForUniform(name, x);
    };
    updateFloat2 = [this](const std::string& name, float x, float y,
                          const std::string& suffix = "") {
      _updateFloat2ForUniform(name, x, y, suffix);
    };
    updateFloat3 = [this](const std::string& name, float x, float y, float z,
                          const std::string& suffix = "") {
      _updateFloat3ForUniform(name, x, y, z, suffix);
    };
    updateFloat4 = [this](const std::string& name, float x, float y, float z,
                          float w, const std::string& suffix = "") {
      _updateFloat4ForUniform(name, x, y, z, w, suffix);
    };
    updateMatrix = [this](const std::string& name, const Matrix& mat) {
      _updateMatrixForUniform(name, mat);
    };
    updateVector3 = [this](const std::string& name, const Vector3& vector) {
      _updateVector3ForUniform(name, vector);
    };
    updateVector4 = [this](const std::string& name, const Vector4& vector) {
      _updateVector4ForUniform(name, vector);
    };
    updateColor3 = [this](const std::string& name, const Color3& color,
                          const std::string& suffix = "") {
      _updateColor3ForUniform(name, color, suffix);
    };
    updateColor4 = [this](const std::string& name, const Color3& color,
                          float alpha, const std::string& suffix = "") {
      _updateColor4ForUniform(name, color, alpha, suffix);
    };
  }
}

UniformBuffer::~UniformBuffer()
{
}

bool UniformBuffer::useUbo() const
{
  return !_noUBO;
}

bool UniformBuffer::isSync() const
{
  return !_needSync;
}

bool UniformBuffer::isDynamic() const
{
  return _dynamic;
}

Float32Array& UniformBuffer::getData()
{
  return _bufferData;
}

GL::IGLBuffer* UniformBuffer::getBuffer()
{
  return _buffer.get();
}

void UniformBuffer::_fillAlignment(size_t size)
{
  // This code has been simplified because we only use floats, vectors of 1, 2,
  // 3, 4 components and 4x4 matrices

  size_t alignment;
  if (size <= 2) {
    alignment = size;
  }
  else {
    alignment = 4;
  }

  if ((_uniformLocationPointer % alignment) != 0) {
    size_t oldPointer = _uniformLocationPointer;
    _uniformLocationPointer
      += alignment - (_uniformLocationPointer % alignment);
    size_t diff = _uniformLocationPointer - oldPointer;

    for (size_t i = 0; i < diff; ++i) {
      _data.emplace_back(0);
    }
  }
}

void UniformBuffer::addUniform(const std::string& name, size_t size)
{
  if (_noUBO) {
    return;
  }

  if (stl_util::contains(_uniformLocations, name)) {
    // Already existing uniform
    return;
  }

  // This function must be called in the order of the shader layout !
  // size can be the size of the uniform, or data directly
  auto data = Float32Array(size, 0);

  _fillAlignment(size);
  _uniformSizes[name]     = size;
  _uniformLocations[name] = _uniformLocationPointer;
  _uniformLocationPointer += size;

  for (size_t i = 0; i < size; ++i) {
    _data.emplace_back(data[i]);
  }

  _needSync = true;
}

void UniformBuffer::addUniform(const std::string& name,
                               const Float32Array& size)
{
  if (_noUBO) {
    return;
  }

  if (stl_util::contains(_uniformLocations, name)) {
    // Already existing uniform
    return;
  }

  // This function must be called in the order of the shader layout !
  // size can be the size of the uniform, or data directly
  auto data    = size;
  size_t _size = data.size();

  _fillAlignment(_size);
  _uniformSizes[name]     = _size;
  _uniformLocations[name] = _uniformLocationPointer;
  _uniformLocationPointer += _size;

  for (size_t i = 0; i < _size; ++i) {
    _data.emplace_back(data[i]);
  }

  _needSync = true;
}

void UniformBuffer::addMatrix(const std::string& name, const Matrix& mat)
{
  addUniform(name, mat.toArray());
}

void UniformBuffer::addFloat2(const std::string& name, float x, float y)
{
  addUniform(name, {x, y});
}

void UniformBuffer::addFloat3(const std::string& name, float x, float y,
                              float z)
{
  addUniform(name, {x, y, z});
}

void UniformBuffer::addColor3(const std::string& name, const Color3& color)
{
  Float32Array temp;
  color.toArray(temp);
  addUniform(name, temp);
}

void UniformBuffer::addColor4(const std::string& name, const Color3& color,
                              float alpha)
{
  Float32Array temp;
  color.toArray(temp);
  temp.emplace_back(alpha);
  addUniform(name, temp);
}

void UniformBuffer::addVector3(const std::string& name, const Vector3& vector)
{
  Float32Array temp;
  vector.toArray(temp);
  addUniform(name, temp);
}

void UniformBuffer::addMatrix3x3(const std::string& name)
{
  addUniform(name, 12);
}

void UniformBuffer::addMatrix2x2(const std::string& name)
{
  addUniform(name, 8);
}

void UniformBuffer::create()
{
  if (_noUBO) {
    return;
  }
  if (_buffer) {
    return; // nothing to do
  }

  // See spec, alignment must be filled as a vec4
  _fillAlignment(4);
  _bufferData = Float32Array(_data);

  _rebuild();

  _needSync = true;
}

void UniformBuffer::_rebuild()
{
  if (_noUBO) {
    return;
  }

  if (_dynamic) {
    _buffer = _engine->createDynamicUniformBuffer(_bufferData);
  }
  else {
    _buffer = _engine->createUniformBuffer(_bufferData);
  }

  _needSync = true;
}

void UniformBuffer::update()
{
  if (!_buffer) {
    create();
    return;
  }

  if (!_dynamic && !_needSync) {
    return;
  }

  _engine->updateUniformBuffer(_buffer.get(), _bufferData);

  _needSync = false;
}

void UniformBuffer::updateUniform(const std::string& uniformName,
                                  const Float32Array& data, size_t size)
{
  size_t location = _uniformLocations[uniformName];
  if (!stl_util::contains(_uniformLocations, uniformName)) {
    if (_buffer) {
      // Cannot add an uniform if the buffer is already created
      BABYLON_LOG_ERROR("UniformBuffer",
                        "Cannot add an uniform after UBO has been created.");
      return;
    }
    addUniform(uniformName, size);
    location = _uniformLocations[uniformName];
  }

  if (!_buffer) {
    create();
  }

  if (!_dynamic) {
    // Cache for static uniform buffers
    bool changed = false;
    for (size_t i = 0; i < size; ++i) {
      if (!stl_util::almost_equal(_bufferData[location + i], data[i])) {
        changed                   = true;
        _bufferData[location + i] = data[i];
      }
    }

    _needSync = _needSync || changed;
  }
  else {
    // No cache for dynamic
    for (size_t i = 0; i < size; ++i) {
      _bufferData[location + i] = data[i];
    }
  }
}

void UniformBuffer::_updateMatrix3x3ForUniform(const std::string& name,
                                               const Float32Array& matrix)
{
  // To match std140, matrix must be realigned
  for (unsigned int i = 0; i < 3; ++i) {
    UniformBuffer::_tempBuffer[i * 4]     = matrix[i * 3];
    UniformBuffer::_tempBuffer[i * 4 + 1] = matrix[i * 3 + 1];
    UniformBuffer::_tempBuffer[i * 4 + 2] = matrix[i * 3 + 2];
    UniformBuffer::_tempBuffer[i * 4 + 3] = 0.f;
  }

  updateUniform(name, UniformBuffer::_tempBuffer, 12);
}

void UniformBuffer::_updateMatrix3x3ForEffect(const std::string& name,
                                              const Float32Array& matrix)
{
  _currentEffect->setMatrix3x3(name, matrix);
}

void UniformBuffer::_updateMatrix2x2ForEffect(const std::string& name,
                                              const Float32Array& matrix)
{
  _currentEffect->setMatrix2x2(name, matrix);
}

void UniformBuffer::_updateMatrix2x2ForUniform(const std::string& name,
                                               const Float32Array& matrix)
{
  // To match std140, matrix must be realigned
  for (unsigned int i = 0; i < 2; i++) {
    UniformBuffer::_tempBuffer[i * 4]     = matrix[i * 2];
    UniformBuffer::_tempBuffer[i * 4 + 1] = matrix[i * 2 + 1];
    UniformBuffer::_tempBuffer[i * 4 + 2] = 0.f;
    UniformBuffer::_tempBuffer[i * 4 + 3] = 0.f;
  }

  updateUniform(name, UniformBuffer::_tempBuffer, 8);
}

void UniformBuffer::_updateFloatForEffect(const std::string& name, float x)
{
  _currentEffect->setFloat(name, x);
}

void UniformBuffer::_updateFloatForUniform(const std::string& name, float x)
{
  UniformBuffer::_tempBuffer[0] = x;
  updateUniform(name, UniformBuffer::_tempBuffer, 1);
}

void UniformBuffer::_updateFloat2ForEffect(const std::string& name, float x,
                                           float y, const std::string& suffix)
{
  _currentEffect->setFloat2(name + suffix, x, y);
}

void UniformBuffer::_updateFloat2ForUniform(const std::string& name, float x,
                                            float y, const std::string& suffix)
{
  UniformBuffer::_tempBuffer[0] = x;
  UniformBuffer::_tempBuffer[1] = y;
  updateUniform(name + suffix, UniformBuffer::_tempBuffer, 2);
}

void UniformBuffer::_updateFloat3ForEffect(const std::string& name, float x,
                                           float y, float z,
                                           const std::string& suffix)
{
  _currentEffect->setFloat3(name + suffix, x, y, z);
}

void UniformBuffer::_updateFloat3ForUniform(const std::string& name, float x,
                                            float y, float z,
                                            const std::string& suffix)
{
  UniformBuffer::_tempBuffer[0] = x;
  UniformBuffer::_tempBuffer[1] = y;
  UniformBuffer::_tempBuffer[2] = z;
  updateUniform(name + suffix, UniformBuffer::_tempBuffer, 3);
}

void UniformBuffer::_updateFloat4ForEffect(const std::string& name, float x,
                                           float y, float z, float w,
                                           const std::string& suffix)
{
  _currentEffect->setFloat4(name + suffix, x, y, z, w);
}

void UniformBuffer::_updateFloat4ForUniform(const std::string& name, float x,
                                            float y, float z, float w,
                                            const std::string& suffix)
{
  UniformBuffer::_tempBuffer[0] = x;
  UniformBuffer::_tempBuffer[1] = y;
  UniformBuffer::_tempBuffer[2] = z;
  UniformBuffer::_tempBuffer[3] = w;
  updateUniform(name + suffix, UniformBuffer::_tempBuffer, 4);
}

void UniformBuffer::_updateMatrixForEffect(const std::string& name,
                                           const Matrix& mat)
{
  _currentEffect->setMatrix(name, mat);
}

void UniformBuffer::_updateMatrixForUniform(const std::string& name,
                                            const Matrix& mat)
{
  updateUniform(name, mat.toArray(), 16);
}

void UniformBuffer::_updateVector3ForEffect(const std::string& name,
                                            const Vector3& vector)
{
  _currentEffect->setVector3(name, vector);
}

void UniformBuffer::_updateVector3ForUniform(const std::string& name,
                                             const Vector3& vector)
{
  vector.toArray(UniformBuffer::_tempBuffer);
  updateUniform(name, UniformBuffer::_tempBuffer, 3);
}

void UniformBuffer::_updateVector4ForEffect(const std::string& name,
                                            const Vector4& vector)
{
  _currentEffect->setVector4(name, vector);
}

void UniformBuffer::_updateVector4ForUniform(const std::string& name,
                                             const Vector4& vector)
{
  vector.toArray(UniformBuffer::_tempBuffer);
  updateUniform(name, UniformBuffer::_tempBuffer, 4);
}

void UniformBuffer::_updateColor3ForEffect(const std::string& name,
                                           const Color3& color,
                                           const std::string& suffix)
{
  _currentEffect->setColor3(name + suffix, color);
}

void UniformBuffer::_updateColor3ForUniform(const std::string& name,
                                            const Color3& color,
                                            const std::string& suffix)
{
  color.toArray(UniformBuffer::_tempBuffer);
  updateUniform(name + suffix, UniformBuffer::_tempBuffer, 3);
}

void UniformBuffer::_updateColor4ForEffect(const std::string& name,
                                           const Color3& color, float alpha,
                                           const std::string& suffix)
{
  _currentEffect->setColor4(name + suffix, color, alpha);
}

void UniformBuffer::_updateColor4ForUniform(const std::string& name,
                                            const Color3& color, float alpha,
                                            const std::string& suffix)
{
  color.toArray(UniformBuffer::_tempBuffer);
  UniformBuffer::_tempBuffer[3] = alpha;
  updateUniform(name + suffix, UniformBuffer::_tempBuffer, 4);
}

void UniformBuffer::setTexture(const std::string& name, BaseTexture* texture)
{
  _currentEffect->setTexture(name, texture);
}

void UniformBuffer::updateUniformDirectly(const std::string& uniformName,
                                          const Float32Array& data)
{
  updateUniform(uniformName, data, data.size());
  update();
}

void UniformBuffer::bindToEffect(Effect* effect, const std::string& name)
{
  _currentEffect = effect;

  if (_noUBO) {
    return;
  }

  effect->bindUniformBuffer(_buffer.get(), name);
}

void UniformBuffer::dispose()
{
  if (_noUBO) {
    return;
  }

  _engine->_uniformBuffers.erase(
    ::std::remove_if(_engine->_uniformBuffers.begin(),
                     _engine->_uniformBuffers.end(),
                     [this](const UniformBuffer* uniformBuffer) {
                       return uniformBuffer == this;
                     }),
    _engine->_uniformBuffers.end());

  if (!_buffer) {
    return;
  }
  if (_engine->_releaseBuffer(_buffer.get())) {
    _buffer = nullptr;
  }
}

} // end of namespace BABYLON
