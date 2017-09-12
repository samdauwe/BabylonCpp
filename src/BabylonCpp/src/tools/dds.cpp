#include <babylon/tools/dds.h>

#include <babylon/core/logging.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {
namespace Internals {

DDSInfo DDSTools::GetDDSInfo(const Uint8Array& arrayBuffer)
{
  Int32Array header(
    reinterpret_cast<const int*>(arrayBuffer.data()),
    reinterpret_cast<const int*>(arrayBuffer.data() + DDS::headerLengthInt));

  int mipmapCount = 1;
  if (header[off_flags] & DDSD_MIPMAPCOUNT) {
    mipmapCount = ::std::max(1, header[off_mipmapCount]);
  }

  return {// width
          header[off_width],
          // height
          header[off_height],
          // mipmapCount
          mipmapCount,
          // isFourCC
          (header[off_pfFlags] & DDPF_FOURCC) == DDPF_FOURCC,
          // isRGB
          (header[off_pfFlags] & DDPF_RGB) == DDPF_RGB,
          // isLuminance
          (header[off_pfFlags] & DDPF_LUMINANCE) == DDPF_LUMINANCE,
          // isCube
          (header[off_caps2] & DDSCAPS2_CUBEMAP) == DDSCAPS2_CUBEMAP};
}

Uint8Array DDSTools::GetRGBAArrayBuffer(float width, float height,
                                        size_t dataOffset, size_t dataLength,
                                        const Uint8Array& arrayBuffer)
{
  Uint8Array byteArray(dataLength);
  Uint8Array srcData(arrayBuffer);
  size_t index = 0;
  for (float y = height - 1; y >= 0; --y) {
    for (float x = 0; x < width; ++x) {
      size_t srcPos = dataOffset + static_cast<size_t>(x + y * width) * 4;
      byteArray[index + 2] = srcData[srcPos];
      byteArray[index + 1] = srcData[srcPos + 1];
      byteArray[index]     = srcData[srcPos + 2];
      byteArray[index + 3] = srcData[srcPos + 3];
      index += 4;
    }
  }

  return byteArray;
}

Uint8Array DDSTools::GetRGBArrayBuffer(float width, float height,
                                       size_t dataOffset, size_t dataLength,
                                       const Uint8Array& arrayBuffer)
{
  Uint8Array byteArray(dataLength);
  Uint8Array srcData(arrayBuffer);
  size_t index = 0;
  for (float y = height - 1; y >= 0; --y) {
    for (float x = 0; x < width; ++x) {
      size_t srcPos = dataOffset + static_cast<size_t>(x + y * width) * 3;
      byteArray[index + 2] = srcData[srcPos];
      byteArray[index + 1] = srcData[srcPos + 1];
      byteArray[index]     = srcData[srcPos + 2];
      index += 3;
    }
  }

  return byteArray;
}

Uint8Array DDSTools::GetLuminanceArrayBuffer(float width, float height,
                                             size_t dataOffset,
                                             size_t dataLength,
                                             const Uint8Array& arrayBuffer)
{
  Uint8Array byteArray(dataLength);
  Uint8Array srcData(arrayBuffer);
  size_t index = 0;
  for (float y = height - 1; y >= 0; --y) {
    for (float x = 0; x < width; ++x) {
      size_t srcPos    = dataOffset + static_cast<size_t>(x + y * width);
      byteArray[index] = srcData[srcPos];
      ++index;
    }
  }

  return byteArray;
}

void DDSTools::UploadDDSLevels(GL::IGLRenderingContext* gl,
                               const Uint8Array& arrayBuffer, DDSInfo& info,
                               bool loadMipmaps, unsigned int faces)
{
  Int32Array header(
    reinterpret_cast<const int*>(arrayBuffer.data()),
    reinterpret_cast<const int*>(arrayBuffer.data() + DDS::headerLengthInt));
  Uint8Array byteArray;
  int blockBytes              = 0;
  unsigned int internalFormat = 0;
  float width = 0.f, height = 0.f;
  size_t dataOffset = 0, dataLength = 0;

  if (header[off_magic] != DDS_MAGIC) {
    BABYLON_LOG_ERROR("DDSTools", "Invalid magic number in DDS header");
    return;
  }

  if (!info.isFourCC && !info.isRGB && !info.isLuminance) {
    BABYLON_LOG_ERROR("DDSTools",
                      "Unsupported format, must contain a FourCC code");
    return;
  }

  if (info.isFourCC) {
    const auto fourCC = header[off_pfFourCC];
    switch (fourCC) {
      case DDS::FOURCC_DXT1:
        blockBytes     = 8;
        internalFormat = RGB_S3TC_DXT1_Format;
        break;
      case DDS::FOURCC_DXT3:
        blockBytes     = 16;
        internalFormat = RGBA_S3TC_DXT3_Format;
        break;
      case DDS::FOURCC_DXT5:
        blockBytes     = 16;
        internalFormat = RGBA_S3TC_DXT5_Format;
        break;
      default:
        BABYLON_LOG_ERROR("DDSTools",
                          "Unsupported FourCC code: ", Int32ToFourCC(fourCC));
        return;
    }
  }

  int mipmapCount = 1;
  if (header[off_flags] & DDSD_MIPMAPCOUNT && loadMipmaps) {
    mipmapCount = ::std::max(1, header[off_mipmapCount]);
  }

  auto bpp = header[off_RGBbpp];

  for (unsigned int face = 0; face < faces; ++face) {
    auto sampler = (faces == 1) ? GL::TEXTURE_2D :
                                  (GL::TEXTURE_CUBE_MAP_POSITIVE_X + face);

    width      = header[off_width];
    height     = header[off_height];
    dataOffset = static_cast<size_t>(header[off_size] + 4);

    for (int i = 0; i < mipmapCount; ++i) {
      if (info.isRGB) {
        if (bpp == 24) {
          dataLength = static_cast<size_t>(width * height * 3);
          byteArray  = DDSTools::GetRGBArrayBuffer(width, height, dataOffset,
                                                  dataLength, arrayBuffer);
          gl->texImage2D(sampler, i, GL::RGB, static_cast<int>(width),
                         static_cast<int>(height), 0, GL::RGB,
                         GL::UNSIGNED_BYTE, byteArray);
        }
        else { // 32
          dataLength = static_cast<size_t>(width * height * 4);
          byteArray  = DDSTools::GetRGBAArrayBuffer(width, height, dataOffset,
                                                   dataLength, arrayBuffer);
          gl->texImage2D(sampler, i, GL::RGBA, static_cast<int>(width),
                         static_cast<int>(height), 0, GL::RGBA,
                         GL::UNSIGNED_BYTE, byteArray);
        }
      }
      else if (info.isLuminance) {
        int unpackAlignment   = gl->getParameteri(GL::UNPACK_ALIGNMENT);
        float unpaddedRowSize = width;
        float paddedRowSize
          = ::std::floor((width + unpackAlignment - 1) / unpackAlignment)
            * unpackAlignment;
        dataLength
          = static_cast<size_t>(paddedRowSize * (height - 1) + unpaddedRowSize);

        byteArray = DDSTools::GetLuminanceArrayBuffer(width, height, dataOffset,
                                                      dataLength, arrayBuffer);
        gl->texImage2D(sampler, i, GL::LUMINANCE, static_cast<int>(width),
                       static_cast<int>(height), 0, GL::LUMINANCE,
                       GL::UNSIGNED_BYTE, byteArray);
      }
      else {
        dataLength
          = static_cast<size_t>(::std::max(4.f, width) / 4
                                * ::std::max(4.f, height) / 4 * blockBytes);
        byteArray = Uint8Array(
          reinterpret_cast<const int*>(arrayBuffer.data() + dataOffset),
          reinterpret_cast<const int*>(arrayBuffer.data() + dataLength));
        gl->compressedTexImage2D(sampler, i, internalFormat,
                                 static_cast<int>(width),
                                 static_cast<int>(height), 0, byteArray);
      }
      dataOffset += dataLength;
      width *= 0.5f;
      height *= 0.5f;

      width  = ::std::max(1.f, width);
      height = ::std::max(1.f, height);
    }
  }
}

} // end of namespace Internals
} // end of namespace BABYLON
