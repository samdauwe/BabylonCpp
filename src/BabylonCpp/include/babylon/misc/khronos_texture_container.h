#ifndef BABYLON_MISC_KHRONOS_TEXTUTE_CONTAINER_H
#define BABYLON_MISC_KHRONOS_TEXTUTE_CONTAINER_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class InternalTexture;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

/**
 * @brief KhronosTextureContainer class
 *
 * for description see https://www.khronos.org/opengles/sdk/tools/KTX/
 * for file layout see
 * https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
 */
class BABYLON_SHARED_EXPORT KhronosTextureContainer {

private:
  static constexpr unsigned int HEADER_LEN
    = 12 + (13 * 4); // identifier + header elements (not including key value
                     // meta-data pairs)

  // load types
  static constexpr unsigned int COMPRESSED_2D
    = 0; // uses a gl.compressedTexImage2D()
  static constexpr unsigned int COMPRESSED_3D
    = 1;                                    // uses a gl.compressedTexImage3D()
  static constexpr unsigned int TEX_2D = 2; // uses a gl.texImage2D()
  static constexpr unsigned int TEX_3D = 3; // uses a gl.texImage3D()

public:
  /**
   * @brief Creates a new KhronosTextureContainer.
   * @param arrayBuffer contents of the KTX container file
   * @param facesExpected should be either 1 or 6, based whether a cube texture
   * or or
   * @param threeDExpected provision for indicating that data should be a 3D
   * texture, not implemented
   * @param textureArrayExpected provision for indicating that data should be a
   * texture array, not implemented
   */
  KhronosTextureContainer(const ArrayBuffer& arrayBuffer, int facesExpected,
                          const std::optional<bool>& threeDExpected
                          = std::nullopt,
                          const std::optional<bool>& textureArrayExpected
                          = std::nullopt);
  ~KhronosTextureContainer(); // = default

  /**
   * @brief Uploads KTX content to a Babylon Texture.
   * It is assumed that the texture has already been created & is currently
   * bound
   * Hidden
   */
  void uploadLevels(const InternalTexturePtr& texture, bool loadMipmaps);

private:
  void _upload2DCompressedLevels(const InternalTexturePtr& texture,
                                 bool loadMipmaps);

public:
  /** Contents of the KTX container file */
  ArrayBuffer arrayBuffer;
  int facesExpected;
  std::optional<bool> threeDExpected;
  std::optional<bool> textureArrayExpected;

  // elements of the header
  /**
   * Gets the openGL type
   */
  uint32_t glType;
  /**
   * Gets the openGL type size
   */
  uint32_t glTypeSize;
  /**
   * Gets the openGL format
   */
  uint32_t glFormat;
  /**
   * Gets the openGL internal format
   */
  uint32_t glInternalFormat;
  /**
   * Gets the base internal format
   */
  uint32_t glBaseInternalFormat;
  /**
   * Gets image width in pixel
   */
  uint32_t pixelWidth;
  /**
   * Gets image height in pixel
   */
  uint32_t pixelHeight;
  /**
   * Gets image depth in pixels
   */
  uint32_t pixelDepth;
  /**
   * Gets the number of array elements
   */
  uint32_t numberOfArrayElements;
  /**
   * Gets the number of faces
   */
  uint32_t numberOfFaces;
  /**
   * Gets the number of mipmap levels
   */
  uint32_t numberOfMipmapLevels;
  /**
   * Gets the bytes of key value data
   */
  uint32_t bytesOfKeyValueData;
  /**
   * Gets the load type
   */
  int loadType;
  /**
   * If the container has been made invalid (eg. constructor failed to correctly
   * load array buffer)
   */
  bool isInvalid;

}; // end of class KhronosTextureContainer

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_KHRONOS_TEXTUTE_CONTAINER_H
