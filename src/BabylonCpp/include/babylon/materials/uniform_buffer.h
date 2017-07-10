#ifndef BABYLON_MATERIALS_UNIFORM_BUFFER_H
#define BABYLON_MATERIALS_UNIFORM_BUFFER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT UniformBuffer {

public:
  /**
   * @brief Uniform buffer objects.
   *
   * Handles blocks of uniform on the GPU.
   *
   * If WebGL 2 is not available, this class falls back on traditionnal
   * setUniformXXX calls.
   *
   * For more information, please refer to :
   * https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
   */
  UniformBuffer(Engine* engine, const Float32Array& data = Float32Array(),
                bool dynamic          = false);
  UniformBuffer(UniformBuffer& other) = delete;
  ~UniformBuffer();

  // Properties
  /**
   * @brief Indicates if the buffer is using the WebGL2 UBO implementation,
   * or just falling back on setUniformXXX calls.
   */
  bool useUbo() const;

  /**
   * @brief Indicates if the WebGL underlying uniform buffer is in sync
   * with the javascript cache data.
   */
  bool isSync() const;

  /**
   * @brief Indicates if the WebGL underlying uniform buffer is dynamic.
   * Also, a dynamic UniformBuffer will disable cache verification and always
   * update the underlying WebGL uniform buffer to the GPU.
   */
  bool isDynamic() const;

  /**
   * @brief The data cache on JS side.
   */
  Float32Array& getData();

  /**
   * @brief The underlying WebGL Uniform buffer.
   */
  GL::IGLBuffer* getBuffer();

  /**
   * @brief Adds an uniform in the buffer.
   * Warning : the subsequents calls of this function must be in the same order
   * as declared in the shader
   * for the layout to be correct !
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {number|number[]} size Data size, or data directly.
   */
  void addUniform(const std::string& name, size_t size);
  void addUniform(const std::string& name, const Float32Array& size);

  /**
   * @brief Wrapper for addUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Matrix} mat A 4x4 matrix.
   */
  void addMatrix(const std::string& name, const Matrix& mat);

  /**
   * @brief Wrapper for addUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {number} x
   * @param {number} y
   */
  void addFloat2(const std::string& name, float x, float y);

  /**
   * @brief Wrapper for addUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {number} x
   * @param {number} y
   * @param {number} z
   */
  void addFloat3(const std::string& name, float x, float y, float z);

  /**
   * @brief Wrapper for addUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Color3} color
   */
  void addColor3(const std::string& name, const Color3& color);

  /**
   * @brief Wrapper for addUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Color3} color
   * @param {number} alpha
   */
  void addColor4(const std::string& name, const Color3& color, float alpha);

  /**
   * @brief Wrapper for addUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Vector3} vector
   */
  void addVector3(const std::string& name, const Vector3& vector);

  /**
   * @brief Wrapper for addUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   */
  void addMatrix3x3(const std::string& name);

  /**
   * @brief Wrapper for addUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   */
  void addMatrix2x2(const std::string& name);

  /**
   * @brief Effectively creates the WebGL Uniform Buffer, once layout is
   * completed with `addUniform`.
   */
  void create();

  /**
   * @brief Updates the WebGL Uniform Buffer on the GPU.
   * If the `dynamic` flag is set to true, no cache comparison is done.
   * Otherwise, the buffer will be updated only if the cache differs.
   */
  void update();

  /**
   * @brief Updates the value of an uniform. The `update` method must be called
   * afterwards to make it effective in the GPU.
   * @param {string} uniformName Name of the uniform, as used in the uniform
   * block in the shader.
   * @param {number[]|Float32Array} data Flattened data
   * @param {number} size Size of the data.
   */
  void updateUniform(const std::string& uniformName, const Float32Array& data,
                     size_t size);

  /**
   * @brief Sets a sampler uniform on the effect.
   * @param {string} name Name of the sampler.
   * @param {Texture} texture
   */
  void setTexture(const std::string& name, BaseTexture* texture);

  /**
   * @brief Directly updates the value of the uniform in the cache AND on the
   * GPU.
   * @param {string} uniformName Name of the uniform, as used in the uniform
   * block in the shader.
   * @param {number[]|Float32Array} data Flattened data
   */
  void updateUniformDirectly(const std::string& uniformName,
                             const Float32Array& data);

  /**
   * @brief Binds this uniform buffer to an effect.
   * @param {Effect} effect
   * @param {string} name Name of the uniform block in the shader.
   */
  void bindToEffect(Effect* effect, const std::string& name);

  /**
   * @brief Disposes the uniform buffer.
   */
  void dispose();

private:
  /**
   * @brief std140 layout specifies how to align data within an UBO structure.
   * See
   * https://khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf#page=159
   * for specs.
   */
  void _fillAlignment(size_t size);

  // Update methods
  void _updateMatrix3x3ForUniform(const std::string& name,
                                  const Float32Array& matrix);
  void _updateMatrix3x3ForEffect(const std::string& name,
                                 const Float32Array& matrix);
  void _updateMatrix2x2ForEffect(const std::string& name,
                                 const Float32Array& matrix);
  void _updateMatrix2x2ForUniform(const std::string& name,
                                  const Float32Array& matrix);
  void _updateFloatForEffect(const std::string& name, float x);
  void _updateFloatForUniform(const std::string& name, float x);
  void _updateFloat2ForEffect(const std::string& name, float x, float y,
                              const std::string& suffix = "");
  void _updateFloat2ForUniform(const std::string& name, float x, float y,
                               const std::string& suffix = "");
  void _updateFloat3ForEffect(const std::string& name, float x, float y,
                              float z, const std::string& suffix = "");
  void _updateFloat3ForUniform(const std::string& name, float x, float y,
                               float z, const std::string& suffix = "");
  void _updateFloat4ForEffect(const std::string& name, float x, float y,
                              float z, float w, const std::string& suffix = "");
  void _updateFloat4ForUniform(const std::string& name, float x, float y,
                               float z, float w,
                               const std::string& suffix = "");
  void _updateMatrixForEffect(const std::string& name, const Matrix& mat);
  void _updateMatrixForUniform(const std::string& name, const Matrix& mat);
  void _updateVector3ForEffect(const std::string& name, const Vector3& vector);
  void _updateVector3ForUniform(const std::string& name, const Vector3& vector);
  void _updateVector4ForEffect(const std::string& name, const Vector4& vector);
  void _updateVector4ForUniform(const std::string& name, const Vector4& vector);
  void _updateColor3ForEffect(const std::string& name, const Color3& color,
                              const std::string& suffix = "");
  void _updateColor3ForUniform(const std::string& name, const Color3& color,
                               const std::string& suffix = "");
  void _updateColor4ForEffect(const std::string& name, const Color3& color,
                              float alpha, const std::string& suffix = "");
  void _updateColor4ForUniform(const std::string& name, const Color3& color,
                               float alpha, const std::string& suffix = "");

public:
  /**
   * @brief Wrapper for updateUniform.
   * @method updateMatrix3x3
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Float32Array} matrix
   */
  std::function<void(const std::string& name, const Float32Array& matrix)>
    updateMatrix3x3;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Float32Array} matrix
   */
  std::function<void(const std::string& name, const Float32Array& matrix)>
    updateMatrix2x2;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {number} x
   */
  std::function<void(const std::string& name, float x)> updateFloat;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {number} x
   * @param {number} y
   * @param {string} [suffix] Suffix to add to the uniform name.
   */
  std::function<void(const std::string& name, float x, float y,
                     const std::string& suffix)>
    updateFloat2;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {number} x
   * @param {number} y
   * @param {number} z
   * @param {string} [suffix] Suffix to add to the uniform name.
   */
  std::function<void(const std::string& name, float x, float y, float z,
                     const std::string& suffix)>
    updateFloat3;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {number} x
   * @param {number} y
   * @param {number} z
   * @param {number} w
   * @param {string} [suffix] Suffix to add to the uniform name.
   */
  std::function<void(const std::string& name, float x, float y, float z,
                     float w, const std::string& suffix)>
    updateFloat4;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Matrix} A 4x4 matrix.
   */
  std::function<void(const std::string& name, const Matrix& mat)> updateMatrix;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Vector3} vector
   */
  std::function<void(const std::string& name, const Vector3& vector)>
    updateVector3;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Vector4} vector
   */
  std::function<void(const std::string& name, const Vector4& vector)>
    updateVector4;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Color3} color
   * @param {string} [suffix] Suffix to add to the uniform name.
   */
  std::function<void(const std::string& name, const Color3& color,
                     const std::string& suffix)>
    updateColor3;

  /**
   * @brief Wrapper for updateUniform.
   * @param {string} name Name of the uniform, as used in the uniform block in
   * the shader.
   * @param {Color3} color
   * @param {number} alpha
   * @param {string} [suffix] Suffix to add to the uniform name.
   */
  std::function<void(const std::string& name, const Color3& color, float alpha,
                     const std::string& suffix)>
    updateColor4;

private:
  Engine* _engine;
  std::unique_ptr<GL::IGLBuffer> _buffer;
  Float32Array _data;
  Float32Array _bufferData;
  bool _dynamic;
  std::string _uniformName;
  std::unordered_map<std::string, size_t> _uniformLocations;
  std::unordered_map<std::string, size_t> _uniformSizes;
  size_t _uniformLocationPointer;
  bool _needSync;
  Float32Array _cache;
  bool _noUBO;
  Effect* _currentEffect;

  // Pool for avoiding memory leaks
  static constexpr unsigned int _MAX_UNIFORM_SIZE = 256;
  static Float32Array _tempBuffer;

}; // end of struct UniformBuffer

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_UNIFORM_BUFFER_H
