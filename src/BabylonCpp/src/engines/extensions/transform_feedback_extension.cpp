#include <babylon/engines/extensions/transform_feedback_extension.h>

#include <babylon/engines/engine.h>
#include <babylon/meshes/webgl/webgl_data_buffer.h>

namespace BABYLON {

TransformFeedbackExtension::TransformFeedbackExtension(Engine* engine) : _this{engine}
{
}

TransformFeedbackExtension::~TransformFeedbackExtension() = default;

WebGLTransformFeedbackPtr TransformFeedbackExtension::createTransformFeedback()
{
  return _this->_gl->createTransformFeedback();
}

void TransformFeedbackExtension::deleteTransformFeedback(const WebGLTransformFeedbackPtr& value)
{
  _this->_gl->deleteTransformFeedback(value.get());
}

void TransformFeedbackExtension::bindTransformFeedback(const WebGLTransformFeedbackPtr& value)
{
  _this->_gl->bindTransformFeedback(GL::TRANSFORM_FEEDBACK, value.get());
}

void TransformFeedbackExtension::beginTransformFeedback(bool usePoints)
{
  _this->_gl->beginTransformFeedback(usePoints ? GL::POINTS : GL::TRIANGLES);
}

void TransformFeedbackExtension::endTransformFeedback()
{
  _this->_gl->endTransformFeedback();
}

void TransformFeedbackExtension::setTranformFeedbackVaryings(const WebGLProgramPtr& program,
                                                             const std::vector<std::string>& value)
{
  _this->_gl->transformFeedbackVaryings(program.get(), value, GL::INTERLEAVED_ATTRIBS);
}

void TransformFeedbackExtension::bindTransformFeedbackBuffer(const WebGLDataBufferPtr& value)
{
  _this->_gl->bindBufferBase(GL::TRANSFORM_FEEDBACK_BUFFER, 0, value->underlyingResource().get());
}

} // end of namespace BABYLON
