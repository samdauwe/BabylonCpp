#ifndef BABYLON_ENGINES_EXTENSIONS_TRANSFORM_FEEDBACK_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_TRANSFORM_FEEDBACK_EXTENSION_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

namespace GL {
class IGLProgram;
class IGLTransformFeedback;
} // end of namespace GL

class Engine;
class WebGLDataBuffer;
using WebGLDataBufferPtr        = std::shared_ptr<WebGLDataBuffer>;
using WebGLProgramPtr           = std::shared_ptr<GL::IGLProgram>;
using WebGLTransformFeedback    = GL::IGLTransformFeedback;
using WebGLTransformFeedbackPtr = std::shared_ptr<WebGLTransformFeedback>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT TransformFeedbackExtension {

public:
  TransformFeedbackExtension(Engine* engine);
  ~TransformFeedbackExtension();

  /**
   * @brief Creates a webGL transform feedback object.
   * Please makes sure to check webGLVersion property to check if you are running webGL 2+
   * @returns the webGL transform feedback object
   */
  WebGLTransformFeedbackPtr createTransformFeedback();

  /**
   * @brief Delete a webGL transform feedback object.
   * @param value defines the webGL transform feedback object to delete
   */
  void deleteTransformFeedback(const WebGLTransformFeedbackPtr& value);

  /**
   * @brief Bind a webGL transform feedback object to the webgl context.
   * @param value defines the webGL transform feedback object to bind
   */
  void bindTransformFeedback(const WebGLTransformFeedbackPtr& value);

  /**
   * @brief Begins a transform feedback operation.
   * @param usePoints defines if points or triangles must be used
   */
  void beginTransformFeedback(bool usePoints = true);

  /**
   * @brief Ends a transform feedback operation.
   */
  void endTransformFeedback();

  /**
   * @brief Specify the varyings to use with transform feedback.
   * @param program defines the associated webGL program
   * @param value defines the list of strings representing the varying names
   */
  void setTranformFeedbackVaryings(const WebGLProgramPtr& program,
                                   const std::vector<std::string>& value);

  /**
   * @brief Bind a webGL buffer for a transform feedback operation.
   * @param value defines the webGL buffer to bind
   */
  void bindTransformFeedbackBuffer(const WebGLDataBufferPtr& value);

private:
  Engine* _this;

}; // end of class TransformFeedbackExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_TRANSFORM_FEEDBACK_EXTENSION_H
