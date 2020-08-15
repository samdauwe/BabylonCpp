#include <babylon/engines/extensions/dynamic_buffer_extension.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/thin_engine.h>

namespace BABYLON {

DynamicBufferExtension::DynamicBufferExtension(ThinEngine* engine) : _this{engine}
{
}

DynamicBufferExtension::~DynamicBufferExtension() = default;

void DynamicBufferExtension::updateDynamicIndexBuffer(const WebGLDataBufferPtr& indexBuffer,
                                                      const IndicesArray& indices, int /*offset*/)
{
  // Force cache update
  _this->_currentBoundBuffer[GL::ELEMENT_ARRAY_BUFFER] = nullptr;
  _this->bindIndexBuffer(indexBuffer);
  auto arrayBuffer = indices;

  _this->_gl->bufferData(GL::ELEMENT_ARRAY_BUFFER, arrayBuffer, GL::DYNAMIC_DRAW);

  _this->_resetIndexBufferBinding();
}

void DynamicBufferExtension::updateDynamicVertexBuffer(const WebGLDataBufferPtr& vertexBuffer,
                                                       const Float32Array& data, int byteOffset,
                                                       int byteLength)
{
  _this->bindArrayBuffer(vertexBuffer);

  if (byteOffset == -1) {
    byteOffset = 0;
  }

  const auto dataLength = static_cast<int>(data.size() * sizeof(float));

  if (byteLength == -1 || (byteLength >= dataLength && byteOffset == 0)) {
    _this->_gl->bufferSubData(GL::ARRAY_BUFFER, byteOffset, data);
  }
  else {
    auto byteArray = stl_util::to_array<uint8_t>(data, static_cast<size_t>(byteOffset),
                                                 static_cast<size_t>(byteLength));
    _this->_gl->bufferSubData(GL::ARRAY_BUFFER, 0, byteArray);
  }

  _this->_resetVertexBufferBinding();
}

} // end of namespace BABYLON
