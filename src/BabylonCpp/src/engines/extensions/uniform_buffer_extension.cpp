#include <babylon/engines/extensions/uniform_buffer_extension.h>

#include <babylon/engines/thin_engine.h>
#include <babylon/engines/webgl/webgl_pipeline_context.h>
#include <babylon/meshes/webgl/webgl_data_buffer.h>

namespace BABYLON {

UniformBufferExtension::UniformBufferExtension(ThinEngine* engine) : _this{engine}
{
}

UniformBufferExtension::~UniformBufferExtension() = default;

WebGLDataBufferPtr UniformBufferExtension::createUniformBuffer(const Float32Array& elements)
{
  auto ubo = _this->_gl->createBuffer();

  if (!ubo) {
    throw std::runtime_error("Unable to create uniform buffer");
  }
  auto result = std::make_shared<WebGLDataBuffer>(ubo);

  _this->bindUniformBuffer(result);

  _this->_gl->bufferData(GL::UNIFORM_BUFFER, elements, GL::STATIC_DRAW);

  _this->bindUniformBuffer(nullptr);

  result->references = 1;
  return result;
}

WebGLDataBufferPtr UniformBufferExtension::createDynamicUniformBuffer(const Float32Array& elements)
{
  auto ubo = _this->_gl->createBuffer();

  if (!ubo) {
    throw std::runtime_error("Unable to create dynamic uniform buffer");
  }

  auto result = std::make_shared<WebGLDataBuffer>(ubo);
  _this->bindUniformBuffer(result);

  _this->_gl->bufferData(GL::UNIFORM_BUFFER, elements, GL::DYNAMIC_DRAW);

  _this->bindUniformBuffer(nullptr);

  result->references = 1;
  return result;
}

void UniformBufferExtension::updateUniformBuffer(const WebGLDataBufferPtr& uniformBuffer,
                                                 const Float32Array& elements, int offset,
                                                 int count)
{
  _this->bindUniformBuffer(uniformBuffer);

  if (offset == -1) {
    offset = 0;
  }

  if (count == -1) {
    _this->_gl->bufferSubData(GL::UNIFORM_BUFFER, offset, elements);
  }
  else {
    Float32Array subvector;
    std::copy(elements.begin() + offset, elements.begin() + offset + count,
              std::back_inserter(subvector));
    _this->_gl->bufferSubData(GL::UNIFORM_BUFFER, 0, subvector);
  }

  _this->bindUniformBuffer(nullptr);
}

void UniformBufferExtension::bindUniformBuffer(const WebGLDataBufferPtr& buffer)
{
  _this->_gl->bindBuffer(GL::UNIFORM_BUFFER, buffer ? buffer->underlyingResource().get() : nullptr);
}

void UniformBufferExtension::bindUniformBufferBase(const WebGLDataBufferPtr& buffer,
                                                   unsigned int location,
                                                   const std::string& /*name*/)
{
  _this->_gl->bindBufferBase(GL::UNIFORM_BUFFER, location,
                             buffer ? buffer->underlyingResource().get() : nullptr);
}

void UniformBufferExtension::bindUniformBlock(const IPipelineContextPtr& pipelineContext,
                                              const std::string& blockName, unsigned int index)
{
  auto program = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext)->program.get();

  auto uniformLocation = _this->_gl->getUniformBlockIndex(program, blockName);

  _this->_gl->uniformBlockBinding(program, uniformLocation, index);
}

} // end of namespace BABYLON
