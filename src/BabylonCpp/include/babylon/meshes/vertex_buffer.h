#ifndef BABYLON_MESHES_VERTEX_BUFFER_H
#define BABYLON_MESHES_VERTEX_BUFFER_H

#include <functional>
#include <memory>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Buffer;
class DataView;
class Engine;
class Scene;
class WebGLDataBuffer;
using WebGLDataBufferPtr = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief Specialized buffer used to store vertex data.
 */
class BABYLON_SHARED_EXPORT VertexBuffer {

  friend Scene;

public:
  // Enums
  /**
   * Positions
   */
  static constexpr const char* PositionKind = "position";
  /**
   * Normals
   */
  static constexpr const char* NormalKind = "normal";
  /**
   * Tangents
   */
  static constexpr const char* TangentKind = "tangent";
  /**
   * Texture coordinates
   */
  static constexpr const char* UVKind = "uv";
  /**
   * Texture coordinates 2
   */
  static constexpr const char* UV2Kind = "uv2";
  /**
   * Texture coordinates 3
   */
  static constexpr const char* UV3Kind = "uv3";
  /**
   * Texture coordinates 4
   */
  static constexpr const char* UV4Kind = "uv4";
  /**
   * Texture coordinates 5
   */
  static constexpr const char* UV5Kind = "uv5";
  /**
   * Texture coordinates 6
   */
  static constexpr const char* UV6Kind = "uv6";
  /**
   * Colors
   */
  static constexpr const char* ColorKind = "color";
  /**
   * Matrix indices (for bones)
   */
  static constexpr const char* MatricesIndicesKind = "matricesIndices";
  /**
   * Matrix weights (for bones)
   */
  static constexpr const char* MatricesWeightsKind = "matricesWeights";
  /**
   * Additional matrix indices (for bones)
   */
  static constexpr const char* MatricesIndicesExtraKind = "matricesIndicesExtra";
  /**
   * Additional matrix weights (for bones)
   */
  static constexpr const char* MatricesWeightsExtraKind = "matricesWeightsExtra";

  static constexpr const char* World0Kind            = "world0";
  static constexpr const char* World1Kind            = "world1";
  static constexpr const char* World2Kind            = "world2";
  static constexpr const char* World3Kind            = "world3";
  static constexpr const char* CellInfoKind          = "cellInfo";
  static constexpr const char* CellIndexKind         = "cellIndex";
  static constexpr const char* OptionsKind           = "options";
  static constexpr const char* AgeKind               = "age";
  static constexpr const char* LifeKind              = "life";
  static constexpr const char* VelocityKind          = "velocity";
  static constexpr const char* DirectionKind         = "direction";
  static constexpr const char* InitialDirectionKind  = "initialDirection";
  static constexpr const char* OffsetKind            = "offset";
  static constexpr const char* SeedKind              = "seed";
  static constexpr const char* SizeKind              = "size";
  static constexpr const char* AngleKind             = "angle";
  static constexpr const char* CellStartOffsetKind   = "cellStartOffset";
  static constexpr const char* NoiseCoordinates1Kind = "noiseCoordinates1";
  static constexpr const char* NoiseCoordinates2Kind = "noiseCoordinates2";
  static constexpr const char* RemapDataKind         = "remapData";
  static constexpr const char* InvertsKind           = "inverts";

  /**
   * The byte type.
   */
  static constexpr const unsigned int BYTE = 5120;

  /**
   * The unsigned byte type.
   */
  static constexpr const unsigned int UNSIGNED_BYTE = 5121;

  /**
   * The short type.
   */
  static constexpr const unsigned int SHORT = 5122;

  /**
   * The unsigned short type.
   */
  static constexpr const unsigned int UNSIGNED_SHORT = 5123;

  /**
   * The integer type.
   */
  static constexpr const unsigned int INT = 5124;

  /**
   * The unsigned integer type.
   */
  static constexpr const unsigned int UNSIGNED_INT = 5125;

  /**
   * The float type.
   */
  static constexpr const unsigned int FLOAT = 5126;

public:
  /**
   * @brief Constructor
   * @param engine the engine
   * @param data the data to use for this vertex buffer
   * @param kind the vertex buffer kind
   * @param updatable whether the data is updatable
   * @param postponeInternalCreation whether to postpone creating the internal WebGL buffer
   * (optional)
   * @param stride the stride (optional)
   * @param instanced whether the buffer is instanced (optional)
   * @param offset the offset of the data (optional)
   * @param size the number of components (optional)
   * @param type the type of the component (optional)
   * @param normalized whether the data contains normalized data (optional)
   * @param useBytes set to true if stride and offset are in bytes (optional)
   * @param divisor defines the instance divisor to use (1 by default)
   */
  VertexBuffer(Engine* engine, const std::variant<Float32Array, Buffer*>& data,
               const std::string& kind, bool updatable,
               const std::optional<bool>& postponeInternalCreation = std::nullopt,
               std::optional<size_t> stride                        = std::nullopt,
               const std::optional<bool>& instanced                = std::nullopt,
               const std::optional<size_t>& offset                 = std::nullopt,
               const std::optional<size_t>& size                   = std::nullopt,
               std::optional<unsigned int> type = std::nullopt, bool normalized = false,
               bool useBytes = false, unsigned int divisor = 1);
  virtual ~VertexBuffer(); // = default

  /**
   * @brief Deduces the stride given a kind.
   * @param kind The kind string to deduce
   * @returns The deduced stride
   */
  static size_t DeduceStride(const std::string& kind);

  /**
   * @brief Hidden
   */
  void _rebuild();

  /** Properties **/

  /**
   * @brief Returns the kind of the VertexBuffer.
   * @returns a string
   */
  [[nodiscard]] const std::string& getKind() const;

  /**
   * @brief Gets a boolean indicating if the VertexBuffer is updatable?
   * @returns true if the buffer is updatable
   */
  [[nodiscard]] bool isUpdatable() const;

  /**
   * @brief Gets current buffer's data.
   * @returns a DataArray or null
   */
  Float32Array& getData();

  /**
   * @brief Gets underlying native buffer.
   * @returns underlying native buffer
   */
  WebGLDataBufferPtr& getBuffer();

  /**
   * @brief Gets the stride in float32 units (i.e. byte stride / 4).
   * May not be an integer if the byte stride is not divisible by 4.
   * @returns the stride in float32 units
   * @deprecated Please use byteStride instead.
   */
  [[nodiscard]] size_t getStrideSize() const;

  /**
   * @brief Returns the offset as a multiple of the type byte length.
   * @returns the offset in bytes
   * @deprecated Please use byteOffset instead.
   */
  [[nodiscard]] size_t getOffset() const;

  /**
   * @brief Returns the number of components per vertex attribute (integer).
   * @returns the size in float
   */
  [[nodiscard]] size_t getSize() const;

  /**
   * @brief Gets a boolean indicating is the internal buffer of the VertexBuffer is instanced.
   * @returns true if this buffer is instanced
   */
  [[nodiscard]] bool getIsInstanced() const;

  /**
   * @brief Returns the instancing divisor, zero for non-instanced (integer).
   * @returns a number
   */
  [[nodiscard]] unsigned int getInstanceDivisor() const;

  /** Methods **/

  /**
   * @brief Store data into the buffer. If the buffer was already used it will be either recreated
   * or updated depending on isUpdatable property.
   */
  WebGLDataBufferPtr create();

  /**
   * @brief Store data into the buffer. If the buffer was already used it will be either recreated
   * or updated depending on isUpdatable property.
   * @param data defines the data to store
   */
  WebGLDataBufferPtr create(const Float32Array& data);

  /**
   * @briefUpdates the underlying buffer according to the passed numeric array or Float32Array. This
   * function will create a new buffer if the current one is not updatable.
   * @param data defines the data to store
   */
  WebGLDataBufferPtr update(const Float32Array& data);

  /**
   *@brief  Updates directly the underlying WebGLBuffer according to the passed numeric array or
   *Float32Array. Returns the directly updated WebGLBuffer.
   * @param data the new data
   * @param offset the new offset
   * @param useBytes set to true if the offset is in bytes
   */
  WebGLDataBufferPtr updateDirectly(const Float32Array& data, size_t offset, bool useBytes = false);

  /**
   * @brief Disposes the VertexBuffer and the underlying WebGLBuffer.
   */
  void dispose();

  /**
   * @brief Enumerates each value of this vertex buffer as numbers.
   * @param count the number of values to enumerate
   * @param callback the callback function called for each value
   */
  void forEach(size_t count, const std::function<void(float value, size_t index)>& callback);

  /**
   * @brief Gets the byte length of the given type.
   * @param type the type
   * @returns the number of bytes
   */
  static unsigned int GetTypeByteLength(unsigned int type);

  /**
   * @brief Enumerates each value of the given parameters as numbers.
   * @param data the data to enumerate
   * @param byteOffset the byte offset of the data
   * @param byteStride the byte stride of the data
   * @param componentCount the number of components per element
   * @param componentType the type of the component
   * @param count the number of values to enumerate
   * @param normalized whether the data is normalized
   * @param callback the callback function called for each value
   */
  static void ForEach(const Float32Array& data, size_t byteOffset, size_t byteStride,
                      size_t componentCount, unsigned int componentType, size_t count,
                      bool normalized,
                      const std::function<void(float value, size_t index)>& callback);

  /**
   * @brief Enumerates each value of the given parameters as numbers.
   * @param data the data to enumerate
   * @param byteOffset the byte offset of the data
   * @param byteStride the byte stride of the data
   * @param componentCount the number of components per element
   * @param componentType the type of the component
   * @param count the total number of components
   * @param normalized whether the data is normalized
   * @param callback the callback function called for each value
   */
  static void ForEach(const std::variant<ArrayBuffer, DataView>& data, size_t byteOffset,
                      size_t byteStride, size_t componentCount, unsigned int componentType,
                      size_t count, bool normalized,
                      const std::function<void(float value, size_t index)>& callback);

private:
  /**
   * @brief Gets the instance divisor when in instanced mode
   */
  [[nodiscard]] unsigned int get_instanceDivisor() const;

  /**
   * @brief Sets the instance divisor when in instanced mode
   */
  void set_instanceDivisor(unsigned int value);

  [[nodiscard]] Buffer* _getBuffer() const;

  static float _GetFloatValue(const DataView& dataView, unsigned int type, size_t byteOffset,
                              bool normalized);

public:
  /**
   * Hidden
   */
  std::unique_ptr<Buffer> _ownedBuffer;

  /**
   * Hidden
   */
  Buffer* _buffer;

  /**
   * Instance divisor when in instanced mode
   */
  Property<VertexBuffer, unsigned int> instanceDivisor;

  /**
   * Gets the byte stride.
   */
  size_t byteStride;

  /**
   * Gets the byte offset.
   */
  size_t byteOffset;

  /**
   * Gets whether integer data values should be normalized into a certain range when being casted to
   * a float.
   */
  bool normalized;

  /**
   * Gets the data type of each component in the array.
   */
  unsigned int type;

private:
  std::string _kind;
  size_t _size;
  bool _ownsBuffer;
  bool _instanced;
  unsigned int _instanceDivisor;

}; // end of class VertexBuffer

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_VERTEX_BUFFER_H
