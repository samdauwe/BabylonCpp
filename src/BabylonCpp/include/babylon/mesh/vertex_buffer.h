#ifndef BABYLON_MESH_VERTEX_BUFFER_H
#define BABYLON_MESH_VERTEX_BUFFER_H

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

namespace GL {
class IGLBuffer;
} // end of namespace GL

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
  static constexpr unsigned int PositionKind = 1;

  /**
   * Normals
   */
  static constexpr unsigned int NormalKind = 2;

  /**
   * Tangents
   */
  static constexpr unsigned int TangentKind = 3;

  /**
   * Texture coordinates
   */
  static constexpr unsigned int UVKind = 4;

  /**
   * Texture coordinates 2
   */
  static constexpr unsigned int UV2Kind = 5;

  /**
   * Texture coordinates 3
   */
  static constexpr unsigned int UV3Kind = 6;

  /**
   * Texture coordinates 4
   */
  static constexpr unsigned int UV4Kind = 7;

  /**
   * Texture coordinates 5
   */
  static constexpr unsigned int UV5Kind = 8;

  /**
   * Texture coordinates 6
   */
  static constexpr unsigned int UV6Kind = 9;

  /**
   * Colors
   */
  static constexpr unsigned int ColorKind = 10;

  /**
   * Matrix indices (for bones)
   */
  static constexpr unsigned int MatricesIndicesKind = 11;

  /**
   * Matrix weights (for bones)
   */
  static constexpr unsigned int MatricesWeightsKind = 12;

  /**
   * Additional matrix indices (for bones)
   */
  static constexpr unsigned int MatricesIndicesExtraKind = 13;

  /**
   * Additional matrix weights (for bones)
   */
  static constexpr unsigned int MatricesWeightsExtraKind = 14;

  static constexpr unsigned int World0Kind            = 15;
  static constexpr unsigned int World1Kind            = 16;
  static constexpr unsigned int World2Kind            = 17;
  static constexpr unsigned int World3Kind            = 18;
  static constexpr unsigned int CellInfoKind          = 19;
  static constexpr unsigned int CellIndexKind         = 20;
  static constexpr unsigned int OptionsKind           = 21;
  static constexpr unsigned int AgeKind               = 22;
  static constexpr unsigned int LifeKind              = 23;
  static constexpr unsigned int VelocityKind          = 24;
  static constexpr unsigned int DirectionKind         = 25;
  static constexpr unsigned int InitialDirectionKind  = 26;
  static constexpr unsigned int OffsetKind            = 27;
  static constexpr unsigned int SeedKind              = 28;
  static constexpr unsigned int SizeKind              = 29;
  static constexpr unsigned int AngleKind             = 30;
  static constexpr unsigned int CellStartOffsetKind   = 31;
  static constexpr unsigned int NoiseCoordinates1Kind = 32;
  static constexpr unsigned int NoiseCoordinates2Kind = 33;
  static constexpr unsigned int RemapDataKind         = 34;

  static constexpr const char* PositionKindChars        = "position";
  static constexpr const char* NormalKindChars          = "normal";
  static constexpr const char* TangentKindChars         = "tangent";
  static constexpr const char* UVKindChars              = "uv";
  static constexpr const char* UV2KindChars             = "uv2";
  static constexpr const char* UV3KindChars             = "uv3";
  static constexpr const char* UV4KindChars             = "uv4";
  static constexpr const char* UV5KindChars             = "uv5";
  static constexpr const char* UV6KindChars             = "uv6";
  static constexpr const char* ColorKindChars           = "color";
  static constexpr const char* MatricesIndicesKindChars = "matricesIndices";
  static constexpr const char* MatricesWeightsKindChars = "matricesWeights";
  static constexpr const char* MatricesIndicesExtraKindChars
    = "matricesIndicesExtra";
  static constexpr const char* MatricesWeightsExtraKindChars
    = "matricesWeightsExtra";
  static constexpr const char* World0KindChars            = "world0";
  static constexpr const char* World1KindChars            = "world1";
  static constexpr const char* World2KindChars            = "world2";
  static constexpr const char* World3KindChars            = "world3";
  static constexpr const char* CellInfoKindChars          = "cellInfo";
  static constexpr const char* CellIndexKindChars         = "cellIndex";
  static constexpr const char* OptionsKindChars           = "options";
  static constexpr const char* AgeKindChars               = "age";
  static constexpr const char* LifeKindChars              = "life";
  static constexpr const char* VelocityKindChars          = "velocity";
  static constexpr const char* DirectionKindChars         = "direction";
  static constexpr const char* InitialDirectionKindChars  = "initialDirection";
  static constexpr const char* OffsetKindChars            = "offset";
  static constexpr const char* SeedKindChars              = "seed";
  static constexpr const char* SizeKindChars              = "size";
  static constexpr const char* AngleKindChars             = "angle";
  static constexpr const char* CellStartOffsetKindChars   = "cellStartOffset";
  static constexpr const char* NoiseCoordinates1KindChars = "noiseCoordinates1";
  static constexpr const char* NoiseCoordinates2KindChars = "noiseCoordinates2";
  static constexpr const char* RemapDataKindChars         = "remapData";

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
   * @param postponeInternalCreation whether to postpone creating the internal
   * WebGL buffer (optional)
   * @param stride the stride (optional)
   * @param instanced whether the buffer is instanced (optional)
   * @param offset the offset of the data (optional)
   * @param size the number of components (optional)
   * @param type the type of the component (optional)
   * @param normalized whether the data contains normalized data (optional)
   * @param useBytes set to true if stride and offset are in bytes (optional)
   */
  VertexBuffer(Engine* engine, const std::variant<Float32Array, Buffer*> data,
               unsigned int kind, bool updatable,
               const std::optional<bool>& postponeInternalCreation
               = std::nullopt,
               std::optional<size_t> stride         = std::nullopt,
               const std::optional<bool>& instanced = std::nullopt,
               const std::optional<size_t>& offset  = std::nullopt,
               const std::optional<size_t>& size    = std::nullopt,
               std::optional<unsigned int> type     = std::nullopt,
               bool normalized = false, bool useBytes = false);
  virtual ~VertexBuffer();

  /** Statics **/
  static std::string KindAsString(unsigned int kind);

  /**
   * @brief Deduces the stride given a kind.
   * @param kind The kind string to deduce
   * @returns The deduced stride
   */
  static size_t DeduceStride(unsigned int kind);

  /**
   * @brief Hidden
   */
  void _rebuild();

  /** Properties **/

  /**
   * @brief Returns the kind of the VertexBuffer.
   * @returns a string
   */
  unsigned int getKind() const;

  /**
   * @brief Gets a boolean indicating if the VertexBuffer is updatable?
   * @returns true if the buffer is updatable
   */
  bool isUpdatable() const;

  /**
   * @brief Gets current buffer's data.
   * @returns a DataArray or null
   */
  Float32Array& getData();

  /**
   * @brief Gets underlying native buffer.
   * @returns underlying native buffer
   */
  GL::IGLBuffer* getBuffer();

  /**
   * @brief Gets the stride in float32 units (i.e. byte stride / 4).
   * May not be an integer if the byte stride is not divisible by 4.
   * DEPRECATED. Use byteStride instead.
   * @returns the stride in float32 units
   */
  size_t getStrideSize() const;

  /**
   * @brief Returns the offset as a multiple of the type byte length.
   * DEPRECATED. Use byteOffset instead.
   * @returns the offset in bytes
   */
  size_t getOffset() const;

  /**
   * @brief Returns the number of components per vertex attribute (integer).
   * @returns the size in float
   */
  size_t getSize() const;

  /**
   * @brief Gets a boolean indicating is the internal buffer of the VertexBuffer
   * is instanced.
   * @returns true if this buffer is instanced
   */
  bool getIsInstanced() const;

  /**
   * @brief Returns the instancing divisor, zero for non-instanced (integer).
   * @returns a number
   */
  unsigned int getInstanceDivisor() const;

  /** Methods **/

  /**
   * @brief Store data into the buffer. If the buffer was already used it will
   * be either recreated or updated depending on isUpdatable property.
   */
  GL::IGLBuffer* create();

  /**
   * @brief Store data into the buffer. If the buffer was already used it will
   * be either recreated or updated depending on isUpdatable property.
   * @param data defines the data to store
   */
  GL::IGLBuffer* create(const Float32Array& data);

  /**
   * @briefUpdates the underlying buffer according to the passed numeric array
   * or Float32Array. This function will create a new buffer if the current one
   * is not updatable.
   * @param data defines the data to store
   */
  GL::IGLBuffer* update(const Float32Array& data);

  /**
   *@brief  Updates directly the underlying WebGLBuffer according to the passed
   *numeric array or Float32Array. Returns the directly updated WebGLBuffer.
   * @param data the new data
   * @param offset the new offset
   * @param useBytes set to true if the offset is in bytes
   */
  GL::IGLBuffer* updateDirectly(const Float32Array& data, size_t offset,
                                bool useBytes = false);

  /**
   * @brief Disposes the VertexBuffer and the underlying WebGLBuffer.
   */
  void dispose();

  /**
   * @brief Enumerates each value of this vertex buffer as numbers.
   * @param count the number of values to enumerate
   * @param callback the callback function called for each value
   */
  void forEach(size_t count,
               const std::function<void(float value, size_t index)>& callback);

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
   * @param count the total number of components
   * @param normalized whether the data is normalized
   * @param callback the callback function called for each value
   */
  static void
  ForEach(const Float32Array& data, size_t byteOffset, size_t byteStride,
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
  static void
  ForEach(const std::variant<ArrayBuffer, DataView>& data, size_t byteOffset,
          size_t byteStride, size_t componentCount, unsigned int componentType,
          size_t count, bool normalized,
          const std::function<void(float value, size_t index)>& callback);

private:
  /**
   * @brief Gets the instance divisor when in instanced mode
   */
  unsigned int get_instanceDivisor() const;

  /**
   * @brief Sets the instance divisor when in instanced mode
   */
  void set_instanceDivisor(unsigned int value);

  Buffer* _getBuffer() const;

  static float _GetFloatValue(const DataView& dataView, unsigned int type,
                              size_t byteOffset, bool normalized);

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
   * Gets whether integer data values should be normalized into a certain range
   * when being casted to a float.
   */
  bool normalized;

  /**
   * Gets the data type of each component in the array.
   */
  unsigned int type;

private:
  unsigned int _kind;
  size_t _size;
  bool _ownsBuffer;
  bool _instanced;
  unsigned int _instanceDivisor;

}; // end of class VertexBuffer

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_VERTEX_BUFFER_H
