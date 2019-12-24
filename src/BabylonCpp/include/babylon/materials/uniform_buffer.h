#ifndef BABYLON_MATERIALS_UNIFORM_BUFFER_H
#define BABYLON_MATERIALS_UNIFORM_BUFFER_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class BaseTexture;
class Color3;
class Effect;
class Engine;
class Matrix;
class Vector3;
class Vector4;
class WebGLDataBuffer;
using BaseTexturePtr     = std::shared_ptr<BaseTexture>;
using EffectPtr          = std::shared_ptr<Effect>;
using WebGLDataBufferPtr = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief Uniform buffer objects.
 *
 * Handles blocks of uniform on the GPU.
 *
 * If WebGL 2 is not available, this class falls back on traditionnal setUniformXXX calls.
 *
 * For more information, please refer to :
 * https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
 */
class BABYLON_SHARED_EXPORT UniformBuffer {

public:
  /**
   * @brief Instantiates a new Uniform buffer objects.
   *
   * Handles blocks of uniform on the GPU.
   *
   * If WebGL 2 is not available, this class falls back on traditionnal setUniformXXX calls.
   *
   * For more information, please refer to :
   * @see https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
   * @param engine Define the engine the buffer is associated with
   * @param data Define the data contained in the buffer
   * @param dynamic Define if the buffer is updatable
   */
  UniformBuffer(Engine* engine, const Float32Array& data = Float32Array(), bool dynamic = false);
  UniformBuffer(UniformBuffer& other) = delete;
  ~UniformBuffer(); // = default

  // Properties
  /**
   * @brief Indicates if the buffer is using the WebGL2 UBO implementation, or just falling back on
   * setUniformXXX calls.
   */
  [[nodiscard]] bool useUbo() const;

  /**
   * @brief Indicates if the WebGL underlying uniform buffer is in sync with the javascript cache
   * data.
   */
  [[nodiscard]] bool isSync() const;

  /**
   * @brief Indicates if the WebGL underlying uniform buffer is dynamic.
   * Also, a dynamic UniformBuffer will disable cache verification and always update the underlying
   * WebGL uniform buffer to the GPU.
   * @returns if Dynamic, otherwise false
   */
  [[nodiscard]] bool isDynamic() const;

  /**
   * @brief The data cache on JS side.
   * @returns the underlying data as a float array
   */
  Float32Array& getData();

  /**
   * @brief The underlying WebGL Uniform buffer.
   * @returns the webgl buffer
   */
  WebGLDataBufferPtr& getBuffer();

  /**
   * @brief Adds an uniform in the buffer.
   * Warning : the subsequents calls of this function must be in the same order as declared in the
   * shader for the layout to be correct !
   * @param name Name of the uniform, as used in the uniform block in the
   * shader.
   * @param size Data size, or data directly.
   */
  void addUniform(const std::string& name, const std::variant<int, Float32Array>& size);

  /**
   * @brief Adds a Matrix 4x4 to the uniform buffer.
   * @param name Name of the uniform, as used in the uniform block in the shader.
   * @param mat A 4x4 matrix.
   */
  void addMatrix(const std::string& name, const Matrix& mat);

  /**
   * @brief Adds a vec2 to the uniform buffer.
   * @param name Name of the uniform, as used in the uniform block in the shader.
   * @param x Define the x component value of the vec2
   * @param y Define the y component value of the vec2
   */
  void addFloat2(const std::string& name, float x, float y);

  /**
   * @brief Adds a vec3 to the uniform buffer.
   * @param name Name of the uniform, as used in the uniform block in the shader.
   * @param x Define the x component value of the vec3
   * @param y Define the y component value of the vec3
   * @param z Define the z component value of the vec3
   */
  void addFloat3(const std::string& name, float x, float y, float z);

  /**
   * @brief Adds a vec3 to the uniform buffer.
   * @param name Name of the uniform, as used in the uniform block in the shader.
   * @param color Define the vec3 from a Color
   */
  void addColor3(const std::string& name, const Color3& color);

  /**
   * @brief Adds a vec4 to the uniform buffer.
   * @param name Name of the uniform, as used in the uniform block in the shader.
   * @param color Define the rgb components from a Color
   * @param alpha Define the a component of the vec4
   */
  void addColor4(const std::string& name, const Color3& color, float alpha);

  /**
   * @brief Adds a vec3 to the uniform buffer.
   * @param name Name of the uniform, as used in the uniform block in the shader.
   * @param vector Define the vec3 components from a Vector
   */
  void addVector3(const std::string& name, const Vector3& vector);

  /**
   * @brief Adds a Matrix 3x3 to the uniform buffer.
   * @param name Name of the uniform, as used in the uniform block in the shader.
   */
  void addMatrix3x3(const std::string& name);

  /**
   * @brief Adds a Matrix 2x2 to the uniform buffer.
   * @param name Name of the uniform, as used in the uniform block in the shader.
   */
  void addMatrix2x2(const std::string& name);

  /**
   * @brief Effectively creates the WebGL Uniform Buffer, once layout is completed with
   * `addUniform`.
   */
  void create();

  /**
   * @brief Hidden
   */
  void _rebuild();

  /**
   * @brief Updates the WebGL Uniform Buffer on the GPU.
   * If the `dynamic` flag is set to true, no cache comparison is done.
   * Otherwise, the buffer will be updated only if the cache differs.
   */
  void update();

  /**
   * @brief Updates the value of an uniform. The `update` method must be called afterwards to make
   * it effective in the GPU.
   * @param uniformName Define the name of the uniform, as used in the uniform block in the shader.
   * @param data Define the flattened data
   * @param size Define the size of the data.
   */
  void updateUniform(const std::string& uniformName, const Float32Array& data, size_t size);

  /**
   * @brief Sets a sampler uniform on the effect.
   * @param name Define the name of the sampler.
   * @param texture Define the texture to set in the sampler
   */
  void setTexture(const std::string& name, const BaseTexturePtr& texture);

  /**
   * @brief Directly updates the value of the uniform in the cache AND on the GPU.
   * @param uniformName Define the name of the uniform, as used in the uniform
   * block in the shader.
   * @param data Define the flattened data
   */
  void updateUniformDirectly(const std::string& uniformName, const Float32Array& data);

  /**
   * @brief Binds this uniform buffer to an effect.
   * @param effect Define the effect to bind the buffer to
   * @param name Name of the uniform block in the shader.
   */
  void bindToEffect(Effect* effect, const std::string& name);

  /**
   * @brief Disposes the uniform buffer.
   */
  void dispose();

private:
  /**
   * @brief std140 layout specifies how to align data within an UBO structure.
   * @see https://khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf#page=159
   * for specs.
   */
  void _fillAlignment(size_t size);

  // Matrix cache
  bool _cacheMatrix(const std::string& name, const Matrix& matrix);

  // Update methods
  void _updateMatrix3x3ForUniform(const std::string& name, const Float32Array& matrix);
  void _updateMatrix3x3ForEffect(const std::string& name, const Float32Array& matrix);
  void _updateMatrix2x2ForEffect(const std::string& name, const Float32Array& matrix);
  void _updateMatrix2x2ForUniform(const std::string& name, const Float32Array& matrix);
  void _updateFloatForEffect(const std::string& name, float x);
  void _updateFloatForUniform(const std::string& name, float x);
  void _updateFloat2ForEffect(const std::string& name, float x, float y,
                              const std::string& suffix = "");
  void _updateFloat2ForUniform(const std::string& name, float x, float y);
  void _updateFloat3ForEffect(const std::string& name, float x, float y, float z,
                              const std::string& suffix = "");
  void _updateFloat3ForUniform(const std::string& name, float x, float y, float z);
  void _updateFloat4ForEffect(const std::string& name, float x, float y, float z, float w,
                              const std::string& suffix = "");
  void _updateFloat4ForUniform(const std::string& name, float x, float y, float z, float w);
  void _updateMatrixForEffect(const std::string& name, const Matrix& mat);
  void _updateMatrixForUniform(const std::string& name, const Matrix& mat);
  void _updateVector3ForEffect(const std::string& name, const Vector3& vector);
  void _updateVector3ForUniform(const std::string& name, const Vector3& vector);
  void _updateVector4ForEffect(const std::string& name, const Vector4& vector);
  void _updateVector4ForUniform(const std::string& name, const Vector4& vector);
  void _updateColor3ForEffect(const std::string& name, const Color3& color,
                              const std::string& suffix = "");
  void _updateColor3ForUniform(const std::string& name, const Color3& color);
  void _updateColor4ForEffect(const std::string& name, const Color3& color, float alpha,
                              const std::string& suffix = "");
  void _updateColor4ForUniform(const std::string& name, const Color3& color, float alpha);

public:
  /**
   * Hidden
   */
  bool _alreadyBound;

  /**
   * Lambda to Update a 3x3 Matrix in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, const Float32Array& matrix)> updateMatrix3x3;

  /**
   * Lambda to Update a 2x2 Matrix in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, const Float32Array& matrix)> updateMatrix2x2;

  /**
   * Lambda to Update a single float in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, float x)> updateFloat;

  /**
   * Lambda to Update a vec2 of float in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, float x, float y, const std::string& suffix)>
    updateFloat2;

  /**
   * Lambda to Update a vec3 of float in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, float x, float y, float z, const std::string& suffix)>
    updateFloat3;

  /**
   * Lambda to Update a vec4 of float in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, float x, float y, float z, float w,
                     const std::string& suffix)>
    updateFloat4;

  /**
   * Lambda to Update a 4x4 Matrix in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, const Matrix& mat)> updateMatrix;

  /**
   * Lambda to Update vec3 of float from a Vector in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, const Vector3& vector)> updateVector3;

  /**
   * Lambda to Update vec4 of float from a Vector in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, const Vector4& vector)> updateVector4;

  /**
   * Lambda to Update vec3 of float from a Color in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, const Color3& color, const std::string& suffix)>
    updateColor3;

  /**
   * Lambda to Update vec4 of float from a Color in a uniform buffer.
   * This is dynamic to allow compat with webgl 1 and 2.
   * You will need to pass the name of the uniform as well as the value.
   */
  std::function<void(const std::string& name, const Color3& color, float alpha,
                     const std::string& suffix)>
    updateColor4;

private:
  Engine* _engine;
  WebGLDataBufferPtr _buffer;
  Float32Array _data;
  Float32Array _bufferData;
  bool _dynamic;
  std::unordered_map<std::string, size_t> _uniformLocations;
  std::unordered_map<std::string, size_t> _uniformSizes;
  size_t _uniformLocationPointer;
  bool _needSync;
  bool _noUBO;
  Effect* _currentEffect;

  // Matrix cache
  std::unordered_map<std::string, int> _valueCache;

  // Pool for avoiding memory leaks
  static constexpr unsigned int _MAX_UNIFORM_SIZE = 256;
  static Float32Array _tempBuffer;

}; // end of struct UniformBuffer

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_UNIFORM_BUFFER_H
