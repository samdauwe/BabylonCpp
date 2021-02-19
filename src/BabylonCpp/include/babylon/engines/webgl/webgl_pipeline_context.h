#ifndef BABYLON_ENGINES_WEBGL_WEBGL_PIPELINE_CONTEXT_H
#define BABYLON_ENGINES_WEBGL_WEBGL_PIPELINE_CONTEXT_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/engines/ipipeline_context.h>

namespace BABYLON {

class ThinEngine;

namespace GL {
class IGLProgram;
class IGLRenderingContext;
class IGLShader;
class IGLTransformFeedback;
} // end of namespace GL

// GL
using WebGLProgramPtr           = std::shared_ptr<GL::IGLProgram>;
using WebGLRenderingContext     = GL::IGLRenderingContext;
using WebGLShaderPtr            = std::shared_ptr<GL::IGLShader>;
using WebGLTransformFeedbackPtr = std::shared_ptr<GL::IGLTransformFeedback>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT WebGLPipelineContext : public IPipelineContext {

public:
  WebGLPipelineContext();
  ~WebGLPipelineContext() override; // = default

  /**
   * @brief Gets a boolean indicating that this pipeline context is supporting asynchronous
   * creating.
   */
  bool isAsync() override;

  /**
   * @brief Gets a boolean indicating that the context is ready to be used (like shaders / pipelines
   * are compiled and ready for instance).
   */
  bool isReady() override;

  /**
   * @brief Hidden
   **/
  void _handlesSpectorRebuildCallback(
    const std::function<void(const WebGLProgramPtr& program)>& onCompiled);

  /**
   * @brief Hidden
   **/
  void _fillEffectInformation(
    Effect* effect, const std::unordered_map<std::string, unsigned int>& uniformBuffersNames,
    const std::vector<std::string>& uniformsNames,
    std::unordered_map<std::string, WebGLUniformLocationPtr>& uniforms,
    std::vector<std::string>& samplerList, std::unordered_map<std::string, int>& samplers,
    const std::vector<std::string>& attributesNames, Int32Array& attributes) override;

  /**
   * @brief Release all associated resources.
   **/
  void dispose() override;

  /**
   * @brief Returns the attribute based on the name of the variable.
   * @param uniformName of the uniform to look up.
   * @returns the location of the uniform.
   */
  WebGLUniformLocationPtr getUniform(const std::string& uniformName);

  /**
   * @brief Hidden
   **/
  bool _cacheMatrix(const std::string& uniformName, const Matrix& matrix);

  /**
   * @brief Hidden
   **/
  bool _cacheFloat2(const std::string& uniformName, float x, float y);

  /**
   * @brief Hidden
   **/
  bool _cacheFloat3(const std::string& uniformName, float x, float y, float z);

  /**
   * @brief Hidden
   **/
  bool _cacheFloat4(const std::string& uniformName, float x, float y, float z, float w);

  /**
   * @brief Sets an integer value on a uniform variable.
   * @param uniformName Name of the variable.
   * @param value Value to be set.
   */
  void setInt(const std::string& uniformName, int value) override;

  /**
   * @brief Sets a int2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First int in int2.
   * @param y Second int in int2.
   */
  void setInt2(const std::string& uniformName, int x, int y) override;

  /**
   * @brief Sets a int3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First int in int3.
   * @param y Second int in int3.
   * @param y Third int in int3.
   */
  void setInt3(const std::string& uniformName, int x, int y, int z) override;

  /**
   * @brief Sets a int4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First int in int4.
   * @param y Second int in int4.
   * @param y Third int in int4.
   * @param w Fourth int in int4.
   */
  void setInt4(const std::string& uniformName, int x, int y, int z, int w) override;

  /**
   * @brief Sets an int array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  void setIntArray(const std::string& uniformName, const Int32Array& array) override;

  /**
   * @brief Sets an int array 2 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4] will result in [[1,2],[3,4]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  void setIntArray2(const std::string& uniformName, const Int32Array& array) override;

  /**
   * @brief Sets an int array 3 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  void setIntArray3(const std::string& uniformName, const Int32Array& array) override;

  /**
   * @brief Sets an int array 4 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  void setIntArray4(const std::string& uniformName, const Int32Array& array) override;

  /**
   * @brief Sets an array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  void setArray(const std::string& uniformName, const Float32Array& array) override;

  /**
   * @brief Sets an array 2 on a uniform variable. (Array is specified as single array eg. [1,2,3,4]
   * will result in [[1,2],[3,4]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  void setArray2(const std::string& uniformName, const Float32Array& array) override;

  /**
   * @brief Sets an array 3 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   * @returns this effect.
   */
  void setArray3(const std::string& uniformName, const Float32Array& array) override;

  /**
   * @brief Sets an array 4 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  void setArray4(const std::string& uniformName, const Float32Array& array) override;

  /**
   * @brief Sets matrices on a uniform variable.
   * @param uniformName Name of the variable.
   * @param matrices matrices to be set.
   */
  void setMatrices(const std::string& uniformName, const Float32Array& matrices) override;

  /**
   * @brief Sets matrix on a uniform variable.
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   */
  void setMatrix(const std::string& uniformName, const Matrix& matrix) override;

  /**
   * @brief Sets a 3x3 matrix on a uniform variable. (Specified as [1,2,3,4,5,6,7,8,9] will result
   * in [1,2,3][4,5,6][7,8,9] matrix).
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   */
  void setMatrix3x3(const std::string& uniformName, const Float32Array& matrix) override;

  /**
   * @brief Sets a 2x2 matrix on a uniform variable. (Specified as [1,2,3,4] will result in
   * [1,2][3,4] matrix).
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   */
  void setMatrix2x2(const std::string& uniformName, const Float32Array& matrix) override;

  /**
   * @brief Sets a float on a uniform variable.
   * @param uniformName Name of the variable.
   * @param value value to be set.
   * @returns this effect.
   */
  void setFloat(const std::string& uniformName, float value) override;

  /**
   * @brief Sets a Vector2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector2 vector2 to be set.
   */
  void setVector2(const std::string& uniformName, const Vector2& vector2) override;

  /**
   * @brief Sets a float2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float2.
   * @param y Second float in float2.
   */
  void setFloat2(const std::string& uniformName, float x, float y) override;

  /**
   * @brief Sets a Vector3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector3 Value to be set.
   */
  void setVector3(const std::string& uniformName, const Vector3& vector3) override;

  /**
   * @brief Sets a float3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float3.
   * @param y Second float in float3.
   * @param z Third float in float3.
   */
  void setFloat3(const std::string& uniformName, float x, float y, float z) override;

  /**
   * @brief Sets a Vector4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector4 Value to be set.
   */
  void setVector4(const std::string& uniformName, const Vector4& vector4) override;

  /**
   * @brief Sets a float4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float4.
   * @param y Second float in float4.
   * @param z Third float in float4.
   * @param w Fourth float in float4.
   * @returns this effect.
   */
  void setFloat4(const std::string& uniformName, float x, float y, float z, float w) override;

  /**
   * @brief Sets a Color3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param color3 Value to be set.
   */
  void setColor3(const std::string& uniformName, const Color3& color3) override;

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param color3 Value to be set.
   * @param alpha Alpha value to be set.
   */
  void setColor4(const std::string& uniformName, const Color3& color3, float alpha) override;

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniformName defines the name of the variable
   * @param color4 defines the value to be set
   */
  void setDirectColor4(const std::string& uniformName, const Color4& color4) override;

  /**
   * @brief Hidden
   */
  std::string _getVertexShaderCode() const override;

  /**
   * @brief Hidden
   */
  std::string _getFragmentShaderCode() const override;

public:
  std::unordered_map<std::string, Float32Array> _valueCache;
  std::unordered_map<std::string, WebGLUniformLocationPtr> _uniforms;
  ThinEngine* engine;
  WebGLProgramPtr program;
  WebGLRenderingContext* context;
  WebGLShaderPtr vertexShader;
  WebGLShaderPtr fragmentShader;
  bool isParallelCompiled;
  std::function<void()> onCompiled;
  WebGLTransformFeedbackPtr transformFeedback;

  std::string vertexCompilationError;
  std::string fragmentCompilationError;
  std::string programLinkError;
  std::string programValidationError;

}; // end of class WebGLPipelineContext

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_WEBGL_WEBGL_PIPELINE_CONTEXT_H
