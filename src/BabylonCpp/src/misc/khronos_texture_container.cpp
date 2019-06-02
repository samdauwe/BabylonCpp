#include <babylon/misc/khronos_texture_container.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/data_view.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

KhronosTextureContainer::KhronosTextureContainer(
  const ArrayBuffer& iArrayBuffer, int iFacesExpected,
  const std::optional<bool>& iThreeDExpected,
  const std::optional<bool>& iTextureArrayExpected)
    : arrayBuffer{iArrayBuffer}
    , facesExpected{iFacesExpected}
    , threeDExpected{iThreeDExpected}
    , textureArrayExpected{iTextureArrayExpected}
    , isInvalid{false}
{
  // Test that it is a ktx formatted file, based on the first 12 bytes,
  // character representation is: '�', 'K', 'T', 'X', ' ', '1', '1', '�', '\r',
  // '\n', '\x1A', '\n' 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D,
  // 0x0A, 0x1A, 0x0A
  auto identifier = stl_util::to_array<uint8_t>(arrayBuffer, 0, 12);
  if (identifier[0] != 0xAB || identifier[1] != 0x4B || identifier[2] != 0x54
      || identifier[3] != 0x58 || identifier[4] != 0x20 || identifier[5] != 0x31
      || identifier[6] != 0x31 || identifier[7] != 0xBB || identifier[8] != 0x0D
      || identifier[9] != 0x0A || identifier[10] != 0x1A
      || identifier[11] != 0x0A) {
    isInvalid = true;
    BABYLON_LOG_ERROR("KhronosTextureContainer",
                      "texture missing KTX identifier")
    return;
  }

  // load the reset of the header in native 32 bit uint
  auto dataSize = sizeof(uint32_t); // Uint32Array.BYTES_PER_ELEMENT;
  DataView headerDataView(arrayBuffer, 12, 13 * dataSize);
  auto endianness   = headerDataView.getUint32(0, true);
  auto littleEndian = endianness == 0x04030201;

  glType = headerDataView.getUint32(
    1 * dataSize, littleEndian); // must be 0 for compressed textures
  glTypeSize = headerDataView.getUint32(
    2 * dataSize, littleEndian); // must be 1 for compressed textures
  glFormat = headerDataView.getUint32(
    3 * dataSize, littleEndian); // must be 0 for compressed textures
  glInternalFormat = headerDataView.getUint32(
    4 * dataSize, littleEndian); // the value of arg passed to
                                 // gl.compressedTexImage2D(,,x,,,,)
  glBaseInternalFormat = headerDataView.getUint32(
    5 * dataSize, littleEndian); // specify GL_RGB, GL_RGBA, GL_ALPHA, etc
                                 // (un-compressed only)
  pixelWidth = headerDataView.getUint32(
    6 * dataSize, littleEndian); // level 0 value of arg passed to
                                 // gl.compressedTexImage2D(,,,x,,,)
  pixelHeight = headerDataView.getUint32(
    7 * dataSize, littleEndian); // level 0 value of arg passed to
                                 // gl.compressedTexImage2D(,,,,x,,)
  pixelDepth = headerDataView.getUint32(
    8 * dataSize, littleEndian); // level 0 value of arg passed to
                                 // gl.compressedTexImage3D(,,,,,x,,)
  numberOfArrayElements = headerDataView.getUint32(
    9 * dataSize, littleEndian); // used for texture arrays
  numberOfFaces = headerDataView.getUint32(
    10 * dataSize,
    littleEndian); // used for cubemap textures, should either be 1 or 6
  numberOfMipmapLevels = headerDataView.getUint32(
    11 * dataSize, littleEndian); // number of levels; disregard possibility of
                                  // 0 for compressed textures
  bytesOfKeyValueData = headerDataView.getUint32(
    12 * dataSize,
    littleEndian); // the amount of space after the header for meta-data

  // Make sure we have a compressed type.  Not only reduces work, but probably
  // better to let dev know they are not compressing.
  if (glType != 0) {
    BABYLON_LOG_ERROR("KhronosTextureContainer",
                      "only compressed formats currently supported")
    return;
  }
  else {
    // value of zero is an indication to generate mipmaps @ runtime.  Not
    // usually allowed for compressed, so disregard.
    numberOfMipmapLevels = std::max(1u, numberOfMipmapLevels);
  }

  if (pixelHeight == 0 || pixelDepth != 0) {
    BABYLON_LOG_ERROR("KhronosTextureContainer",
                      "only 2D textures currently supported")
    return;
  }

  if (numberOfArrayElements != 0) {
    BABYLON_LOG_ERROR("KhronosTextureContainer",
                      "texture arrays not currently supported")
    return;
  }

  if (static_cast<int>(numberOfFaces) != facesExpected) {
    BABYLON_LOGF_ERROR("KhronosTextureContainer",
                       "number of faces expected %d, but found %d",
                       facesExpected, numberOfFaces)
    return;
  }

  // we now have a completely validated file, so could use existence of loadType
  // as success would need to make this more elaborate & adjust checks above to
  // support more than one load type
  loadType = KhronosTextureContainer::COMPRESSED_2D;
}

KhronosTextureContainer::~KhronosTextureContainer()
{
}

void KhronosTextureContainer::uploadLevels(const InternalTexturePtr& texture,
                                           bool loadMipmaps)
{
  switch (loadType) {
    case KhronosTextureContainer::COMPRESSED_2D:
      _upload2DCompressedLevels(texture, loadMipmaps);
      break;
    case KhronosTextureContainer::TEX_2D:
    case KhronosTextureContainer::COMPRESSED_3D:
    case KhronosTextureContainer::TEX_3D:
      break;
  }
}

void KhronosTextureContainer::_upload2DCompressedLevels(
  const InternalTexturePtr& texture, bool loadMipmaps)
{
  // initialize width & height for level 1
  auto dataOffset = KhronosTextureContainer::HEADER_LEN + bytesOfKeyValueData;
  auto width      = pixelWidth;
  auto height     = pixelHeight;

  auto mipmapCount = loadMipmaps ? numberOfMipmapLevels : 1;
  for (auto level = 0u; level < mipmapCount; ++level) {
    // size per face, since not supporting array cubemaps
    auto imageSize = stl_util::to_array<int32_t>(arrayBuffer, dataOffset, 1)[0];
    dataOffset += 4; // image data starts from next multiple of 4 offset. Each
                     // face refers to same imagesize field above.
    for (unsigned int face = 0; face < numberOfFaces; face++) {
      auto byteArray = stl_util::to_array<uint8_t>(
        arrayBuffer, dataOffset, static_cast<size_t>(imageSize));

      auto engine = texture->getEngine();
      engine->_uploadCompressedDataToTextureDirectly(
        texture, glInternalFormat, static_cast<float>(width),
        static_cast<float>(height), byteArray, face, static_cast<int>(level));

      dataOffset += static_cast<size_t>(
        imageSize); // add size of the image for the next face/mipmap
      dataOffset
        += 3 - ((imageSize + 3) % 4); // add padding for odd sized image
    }
    width  = static_cast<uint32_t>(std::max(1.f, width * 0.5f));
    height = static_cast<uint32_t>(std::max(1.f, height * 0.5f));
  }
}

} // end of namespace BABYLON
