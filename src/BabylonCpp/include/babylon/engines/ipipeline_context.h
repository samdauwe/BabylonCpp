#ifndef BABYLON_ENGINES_IPIPELINE_CONTEXT_H
#define BABYLON_ENGINES_IPIPELINE_CONTEXT_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

FWD_CLASS_SPTR(IPipelineContext)

class Color3;
class Color4;
class Effect;
class Matrix;
class Vector2;
class Vector3;
class Vector4;

namespace GL {
class IGLUniformLocation;
} // end of namespace GL

// GL
using WebGLUniformLocationPtr = std::shared_ptr<GL::IGLUniformLocation>;

/**
 * @brief Class used to store and describe the pipeline context associated with an effect.
 */
class BABYLON_SHARED_EXPORT IPipelineContext {

public:
  IPipelineContext();
  virtual ~IPipelineContext(); // = default

public:
  /**
   * Gets a boolean indicating that this pipeline context is supporting asynchronous creating.
   */
  virtual bool isAsync();

  /**
   * Gets a boolean indicating that the context is ready to be used (like shaders / pipelines are
   * compiled and ready for instance).
   */
  virtual bool isReady();

  /**
   * @brief Hidden
   */
  virtual std::string _getVertexShaderCode() const = 0;

  /**
   * @brief Hidden
   */
  virtual std::string _getFragmentShaderCode() const = 0;

  /**
   * Hidden
   */
  std::function<void(
    const std::function<void(const IPipelineContextPtr& pipelineContext)>& onCompiled)>
    _handlesSpectorRebuildCallback;

  /**
   * @brief Hidden
   **/
  virtual void _fillEffectInformation(
    Effect* effect, const std::unordered_map<std::string, unsigned int>& uniformBuffersNames,
    const std::vector<std::string>& uniformsNames,
    std::unordered_map<std::string, WebGLUniformLocationPtr>& uniforms,
    std::vector<std::string>& samplerList, std::unordered_map<std::string, int>& samplers,
    const std::vector<std::string>& attributesNames, Int32Array& attributes)
    = 0;

  /**
   * @brief Releases the resources associated with the pipeline.
   **/
  virtual void dispose() = 0;

  /**
   * @brief Sets an integer value on a uniform variable.
   * @param uniformName Name of the variable.
   * @param value Value to be set.
   */
  virtual void setInt(const std::string& uniformName, int value) = 0;

  /**
   * @brief Sets an int2 value on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First int in int2.
   * @param y Second int in int2.
   */
  virtual void setInt2(const std::string& uniformName, int x, int y) = 0;

  /**
   * @brief Sets an int3 value on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First int in int3.
   * @param y Second int in int3.
   * @param z Third int in int3.
   */
  virtual void setInt3(const std::string& uniformName, int x, int y, int z) = 0;

  /**
   * @brief Sets an int4 value on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First int in int4.
   * @param y Second int in int4.
   * @param z Third int in int4.
   * @param w Fourth int in int4.
   */
  virtual void setInt4(const std::string& uniformName, int x, int y, int z, int w) = 0;

  /**
   * @brief Sets an int array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  virtual void setIntArray(const std::string& uniformName, const Int32Array& array) = 0;

  /**
   * @brief Sets an int array 2 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4] will result in [[1,2],[3,4]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  virtual void setIntArray2(const std::string& uniformName, const Int32Array& array) = 0;

  /**
   * @brief Sets an int array 3 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  virtual void setIntArray3(const std::string& uniformName, const Int32Array& array) = 0;

  /**
   * @brief Sets an int array 4 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  virtual void setIntArray4(const std::string& uniformName, const Int32Array& array) = 0;

  /**
   * @brief Sets an array on a uniform variable.
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  virtual void setArray(const std::string& uniformName, const Float32Array& array) = 0;

  /**
   * @brief Sets an array 2 on a uniform variable. (Array is specified as single array eg. [1,2,3,4]
   * will result in [[1,2],[3,4]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  virtual void setArray2(const std::string& uniformName, const Float32Array& array) = 0;

  /**
   * @brief Sets an array 3 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6] will result in [[1,2,3],[4,5,6]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  virtual void setArray3(const std::string& uniformName, const Float32Array& array) = 0;

  /**
   * @brief Sets an array 4 on a uniform variable. (Array is specified as single array eg.
   * [1,2,3,4,5,6,7,8] will result in [[1,2,3,4],[5,6,7,8]] in the shader).
   * @param uniformName Name of the variable.
   * @param array array to be set.
   */
  virtual void setArray4(const std::string& uniformName, const Float32Array& array) = 0;

  /**
   * @brief Sets matrices on a uniform variable.
   * @param uniformName Name of the variable.
   * @param matrices matrices to be set.
   */
  virtual void setMatrices(const std::string& uniformName, const Float32Array& matrices) = 0;

  /**
   * @brief Sets matrix on a uniform variable.
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   */
  virtual void setMatrix(const std::string& uniformName, const Matrix& matrix) = 0;

  /**
   * @brief Sets a 3x3 matrix on a uniform variable. (Specified as [1,2,3,4,5,6,7,8,9] will result
   * in [1,2,3][4,5,6][7,8,9] matrix).
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   */
  virtual void setMatrix3x3(const std::string& uniformName, const Float32Array& matrix) = 0;

  /**
   * @brief Sets a 2x2 matrix on a uniform variable. (Specified as [1,2,3,4] will result in
   * [1,2][3,4] matrix).
   * @param uniformName Name of the variable.
   * @param matrix matrix to be set.
   */
  virtual void setMatrix2x2(const std::string& uniformName, const Float32Array& matrix) = 0;

  /**
   * @brief Sets a float on a uniform variable.
   * @param uniformName Name of the variable.
   * @param value value to be set.
   */
  virtual void setFloat(const std::string& uniformName, float value) = 0;

  /**
   * @brief Sets a Vector2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector2 vector2 to be set.
   */
  virtual void setVector2(const std::string& uniformName, const Vector2& vector2) = 0;

  /**
   * @brief Sets a float2 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float2.
   * @param y Second float in float2.
   */
  virtual void setFloat2(const std::string& uniformName, float x, float y) = 0;

  /**
   * @brief Sets a Vector3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector3 Value to be set.
   */
  virtual void setVector3(const std::string& uniformName, const Vector3& vector3) = 0;

  /**
   * @brief Sets a float3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float3.
   * @param y Second float in float3.
   * @param z Third float in float3.
   */
  virtual void setFloat3(const std::string& uniformName, float x, float y, float z) = 0;

  /**
   * @brief Sets a Vector4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param vector4 Value to be set.
   */
  virtual void setVector4(const std::string& uniformName, const Vector4& vector4) = 0;

  /**
   * @brief Sets a float4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param x First float in float4.
   * @param y Second float in float4.
   * @param z Third float in float4.
   * @param w Fourth float in float4.
   */
  virtual void setFloat4(const std::string& uniformName, float x, float y, float z, float w) = 0;

  /**
   * @brief Sets a Color3 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param color3 Value to be set.
   */
  virtual void setColor3(const std::string& uniformName, const Color3& color3) = 0;

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniformName Name of the variable.
   * @param color3 Value to be set.
   * @param alpha Alpha value to be set.
   */
  virtual void setColor4(const std::string& uniformName, const Color3& color3, float alpha) = 0;

  /**
   * @brief Sets a Color4 on a uniform variable.
   * @param uniformName defines the name of the variable
   * @param color4 defines the value to be set
   */
  virtual void setDirectColor4(const std::string& uniformName, const Color4& color4) = 0;

public:
  /** @hidden */
  std::string _name;

private:
  bool _isAsync;
  bool _isReady;
}; // end of class IPipelineContext

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_IPIPELINE_CONTEXT_H
