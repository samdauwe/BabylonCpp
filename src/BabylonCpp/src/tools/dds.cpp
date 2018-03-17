#include <babylon/tools/dds.h>

#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {
namespace Internals {

bool DDSTools::StoreLODInAlphaChannel = false;
Float32Array DDSTools::_FloatView;
Int32Array DDSTools::_Int32View;

DDSInfo DDSTools::GetDDSInfo(const Uint8Array& arrayBuffer)
{
  Int32Array header(
    reinterpret_cast<const int*>(arrayBuffer.data()),
    reinterpret_cast<const int*>(arrayBuffer.data() + DDS::headerLengthInt));
  Int32Array extendedHeader(reinterpret_cast<const int*>(arrayBuffer.data()),
                            reinterpret_cast<const int*>(
                              arrayBuffer.data() + DDS::headerLengthInt + 4));

  int mipmapCount = 1;
  if (header[off_flags] & DDSD_MIPMAPCOUNT) {
    mipmapCount = ::std::max(1, header[off_mipmapCount]);
  }

  auto fourCC = header[off_pfFourCC];
  auto dxgiFormat
    = (fourCC == DDS::FOURCC_DX10) ? extendedHeader[off_dxgiFormat] : 0;
  auto textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT;

  switch (fourCC) {
    case DDS::FOURCC_D3DFMT_R16G16B16A16F:
      textureType = EngineConstants::TEXTURETYPE_HALF_FLOAT;
      break;
    case DDS::FOURCC_D3DFMT_R32G32B32A32F:
      textureType = EngineConstants::TEXTURETYPE_FLOAT;
      break;
    case DDS::FOURCC_DX10:
      if (dxgiFormat == DDS::DXGI_FORMAT_R16G16B16A16_FLOAT) {
        textureType = EngineConstants::TEXTURETYPE_HALF_FLOAT;
        break;
      }
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
          (header[off_caps2] & DDSCAPS2_CUBEMAP) == DDSCAPS2_CUBEMAP,
          // isCompressed
          (fourCC == DDS::FOURCC_DXT1 || fourCC == DDS::FOURCC_DXT3
           || fourCC == DDS::FOURCC_DXT5),
          // dxgiFormat
          dxgiFormat,
          // textureType
          textureType};
}

float DDSTools::_ToHalfFloat(float value)
{
  if (DDSTools::_FloatView.empty()) {
    DDSTools::_FloatView = Float32Array(1);
    DDSTools::_Int32View = Int32Array(DDSTools::_FloatView.size());
  }

  DDSTools::_FloatView[0] = value;
  auto x                  = DDSTools::_Int32View[0];

  auto bits = (x >> 16) & 0x8000; /* Get the sign */
  auto m    = (x >> 12) & 0x07ff; /* Keep one extra bit for rounding */
  auto e    = (x >> 23) & 0xff;   /* Using int is faster here */

  /* If zero, or denormal, or exponent underflows too much for a denormal
   * half, return signed zero. */
  if (e < 103) {
    return bits;
  }

  /* If NaN, return NaN. If Inf or exponent overflow, return Inf. */
  if (e > 142) {
    bits |= 0x7c00;
    /* If exponent was 0xff and one mantissa bit was set, it means NaN,
     * not Inf, so make sure we set one mantissa bit too. */
    bits |= ((e == 255) ? 0 : 1) && (x & 0x007fffff);
    return bits;
  }

  /* If exponent underflows but not too much, return a denormal */
  if (e < 113) {
    m |= 0x0800;
    /* Extra rounding may overflow and set mantissa to 0 and exponent
     * to 1, which is OK. */
    bits |= (m >> (114 - e)) + ((m >> (113 - e)) & 1);
    return bits;
  }

  bits |= ((e - 112) << 10) | (m >> 1);
  bits += m & 1;
  return bits;
}

Uint8Array DDSTools::_GetRGBAArrayBuffer(float width, float height,
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

Uint8Array DDSTools::_GetRGBArrayBuffer(float width, float height,
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

Uint8Array DDSTools::_GetLuminanceArrayBuffer(float width, float height,
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

void DDSTools::UploadDDSLevels(Engine* engine, GL::IGLRenderingContext* gl,
                               const Uint8Array& arrayBuffer, DDSInfo& info,
                               bool loadMipmaps, unsigned int faces,
                               int lodIndex, int currentFace)
{
  auto ext = engine->getCaps().s3tc;

  Int32Array header(
    reinterpret_cast<const int*>(arrayBuffer.data()),
    reinterpret_cast<const int*>(arrayBuffer.data() + DDS::headerLengthInt));
  int fourCC = 0;
  Uint8Array byteArray;
  int blockBytes      = 1;
  unsigned int format = 0, internalFormat = 0;
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

  if (info.isCompressed && !ext) {
    BABYLON_LOG_ERROR(
      "DDSTools", "Compressed textures are not supported on this platform.");
    return;
  }

  auto bpp   = header[off_RGBbpp];
  dataOffset = static_cast<size_t>(header[off_size]) + 4;

  auto computeFormats = false;

  if (info.isFourCC) {
    fourCC = header[off_pfFourCC];
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
      case DDS::FOURCC_D3DFMT_R16G16B16A16F:
        computeFormats = true;
        break;
      case DDS::FOURCC_D3DFMT_R32G32B32A32F:
        computeFormats = true;
        break;
      case DDS::FOURCC_DX10: {
        // There is an additionnal header so dataOffset need to be changed
        dataOffset += 5 * 4; // 5 uints
        auto supported = false;
        switch (info.dxgiFormat) {
          case DDS::DXGI_FORMAT_R16G16B16A16_FLOAT:
            computeFormats = true;
            supported      = true;
            break;
          case DDS::DXGI_FORMAT_B8G8R8X8_UNORM:
            info.isRGB    = true;
            info.isFourCC = false;
            bpp           = 32;
            supported     = true;
            break;
        }
        if (supported) {
          break;
        }
      } break;
      default:
        BABYLON_LOG_ERROR("DDSTools",
                          "Unsupported FourCC code: ", Int32ToFourCC(fourCC));
        return;
    }
  }

  if (computeFormats) {
    format = engine->_getWebGLTextureType(info.textureType);
    internalFormat
      = engine->_getRGBABufferInternalSizedFormat(info.textureType);
  }

  int mipmapCount = 1;
  if (header[off_flags] & DDSD_MIPMAPCOUNT && loadMipmaps) {
    mipmapCount = ::std::max(1, header[off_mipmapCount]);
  }

  for (unsigned int face = 0; face < faces; ++face) {
    auto sampler
      = (faces == 1) ?
          GL::TEXTURE_2D :
          (GL::TEXTURE_CUBE_MAP_POSITIVE_X + face
           + ((currentFace > 0) ? static_cast<unsigned int>(currentFace) : 0));

    width      = static_cast<float>(header[off_width]);
    height     = static_cast<float>(header[off_height]);
    dataOffset = static_cast<size_t>(header[off_size] + 4);

    for (int mip = 0; mip < mipmapCount; ++mip) {
      if (lodIndex == -1 || lodIndex == mip) {
        // In case of fixed LOD, if the lod has just been uploaded, early exit.
        const int i = (lodIndex == -1) ? mip : 0;

        if (!info.isCompressed && info.isFourCC) {
          // Not implemeted yet
          ArrayBufferView buffer;

          if (!buffer.uint8Array.empty()) {
            engine->_uploadDataToTexture(
              sampler, i, static_cast<int>(internalFormat),
              static_cast<int>(width), static_cast<int>(height), GL::RGBA,
              format, buffer.uint8Array);
          }
        }
        else if (info.isRGB) {
          if (bpp == 24) {
            dataLength = static_cast<size_t>(width * height * 3);
            byteArray  = DDSTools::_GetRGBArrayBuffer(width, height, dataOffset,
                                                     dataLength, arrayBuffer);
            gl->texImage2D(sampler, i, GL::RGB, static_cast<int>(width),
                           static_cast<int>(height), 0, GL::RGB,
                           GL::UNSIGNED_BYTE, byteArray);
          }
          else { // 32
            dataLength = static_cast<size_t>(width * height * 4);
            byteArray = DDSTools::_GetRGBAArrayBuffer(width, height, dataOffset,
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
          dataLength = static_cast<size_t>(paddedRowSize * (height - 1)
                                           + unpaddedRowSize);

          byteArray = DDSTools::_GetLuminanceArrayBuffer(
            width, height, dataOffset, dataLength, arrayBuffer);
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
      }
      dataOffset += bpp ? (width * height * (bpp / 8)) : dataLength;
      width *= 0.5f;
      height *= 0.5f;

      width  = ::std::max(1.f, width);
      height = ::std::max(1.f, height);
    }

    if (currentFace != -1) {
      // Loading a single face
      break;
    }
  }
}

} // end of namespace Internals
} // end of namespace BABYLON
