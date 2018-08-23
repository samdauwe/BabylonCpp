#include <babylon/tools/dds.h>

#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/math/scalar.h>
#include <babylon/tools/hdr/cube_map_to_spherical_polynomial_tools.h>

namespace BABYLON {

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

float DDSTools::_FromHalfFloat(uint8_t value)
{
  auto s = (value & 0x8000) >> 15;
  auto e = (value & 0x7C00) >> 10;
  auto f = value & 0x03FF;

  if (e == 0) {
    auto result = (s ? -1 : 1) * ::std::pow(2, -14) * (f / std::pow(2, 10));
    return static_cast<float>(result);
  }
  else if (e == 0x1F) {
    return f ? 0 : ((s ? -1 : 1) * numeric_limits_t<int>::infinity());
  }

  auto result
    = (s ? -1 : 1) * ::std::pow(2, e - 15) * (1 + (f / ::std::pow(2, 10)));
  return static_cast<float>(result);
}

Float32Array DDSTools::_GetHalfFloatAsFloatRGBAArrayBuffer(
  float width, float height, size_t dataOffset, size_t dataLength,
  const Uint8Array& arrayBuffer, float lod)
{
  Float32Array destArray(dataLength);
  Uint8Array srcData(arrayBuffer);
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      size_t srcPos    = dataOffset + static_cast<size_t>(x + y * width) * 4;
      destArray[index] = DDSTools::_FromHalfFloat(srcData[srcPos]);
      destArray[index + 1] = DDSTools::_FromHalfFloat(srcData[srcPos + 1]);
      destArray[index + 2] = DDSTools::_FromHalfFloat(srcData[srcPos + 2]);
      if (DDSTools::StoreLODInAlphaChannel) {
        destArray[index + 3] = lod;
      }
      else {
        destArray[index + 3] = DDSTools::_FromHalfFloat(srcData[srcPos + 3]);
      }
      index += 4;
    }
  }

  return destArray;
}

Uint16Array
DDSTools::_GetHalfFloatRGBAArrayBuffer(float width, float height,
                                       size_t dataOffset, size_t dataLength,
                                       const Uint8Array& arrayBuffer, float lod)
{
  if (DDSTools::StoreLODInAlphaChannel) {
    Uint16Array destArray(dataLength);
    Uint8Array srcData(arrayBuffer);
    size_t index = 0;
    for (float y = 0; y < height; ++y) {
      for (float x = 0; x < width; ++x) {
        size_t srcPos    = dataOffset + static_cast<size_t>(x + y * width) * 4;
        destArray[index] = srcData[srcPos];
        destArray[index + 1] = srcData[srcPos + 1];
        destArray[index + 2] = srcData[srcPos + 2];
        destArray[index + 3]
          = static_cast<uint8_t>(DDSTools::_ToHalfFloat(lod));
        index += 4;
      }
    }

    return destArray;
  }

  return Uint16Array();
}

Float32Array DDSTools::_GetFloatRGBAArrayBuffer(float width, float height,
                                                size_t dataOffset,
                                                size_t dataLength,
                                                const Uint8Array& arrayBuffer,
                                                float lod)
{
  if (DDSTools::StoreLODInAlphaChannel) {
    Float32Array destArray(dataLength);
    Uint8Array srcData(arrayBuffer);
    size_t index = 0;
    for (float y = 0; y < height; ++y) {
      for (float x = 0; x < width; ++x) {
        size_t srcPos    = dataOffset + static_cast<size_t>(x + y * width) * 4;
        destArray[index] = srcData[srcPos];
        destArray[index + 1] = srcData[srcPos + 1];
        destArray[index + 2] = srcData[srcPos + 2];
        destArray[index + 3] = lod;
        index += 4;
      }
    }

    return destArray;
  }
  return Float32Array();
}

Float32Array DDSTools::_GetFloatAsUIntRGBAArrayBuffer(
  float width, float height, size_t dataOffset, size_t dataLength,
  const Uint8Array& arrayBuffer, float lod)
{
  Float32Array destArray(dataLength);
  Uint8Array srcData(arrayBuffer);
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      size_t srcPos    = dataOffset + static_cast<size_t>(x + y * width) * 4;
      destArray[index] = Scalar::Clamp(srcData[srcPos]) * 255;
      destArray[index + 1] = Scalar::Clamp(srcData[srcPos + 1]) * 255;
      destArray[index + 2] = Scalar::Clamp(srcData[srcPos + 2]) * 255;
      if (DDSTools::StoreLODInAlphaChannel) {
        destArray[index + 3] = lod;
      }
      else {
        destArray[index + 3] = Scalar::Clamp(srcData[srcPos + 3]) * 255;
      }
      index += 4;
    }
  }

  return destArray;
}

Float32Array DDSTools::_GetHalfFloatAsUIntRGBAArrayBuffer(
  float width, float height, size_t dataOffset, size_t dataLength,
  const Uint8Array& arrayBuffer, float lod)
{
  Float32Array destArray(dataLength);
  Uint8Array srcData(arrayBuffer);
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      size_t srcPos = dataOffset + static_cast<size_t>(x + y * width) * 4;

      destArray[index]
        = Scalar::Clamp(DDSTools::_FromHalfFloat(srcData[srcPos])) * 255;
      destArray[index + 1]
        = Scalar::Clamp(DDSTools::_FromHalfFloat(srcData[srcPos + 1])) * 255;
      destArray[index + 2]
        = Scalar::Clamp(DDSTools::_FromHalfFloat(srcData[srcPos + 2])) * 255;
      if (DDSTools::StoreLODInAlphaChannel) {
        destArray[index + 3] = lod;
      }
      else {
        destArray[index + 3]
          = Scalar::Clamp(DDSTools::_FromHalfFloat(srcData[srcPos + 3])) * 255;
      }
      index += 4;
    }
  }

  return destArray;
}

Uint8Array DDSTools::_GetRGBAArrayBuffer(float width, float height,
                                         size_t dataOffset, size_t dataLength,
                                         const Uint8Array& arrayBuffer,
                                         int rOffset, int gOffset, int bOffset,
                                         int aOffset)
{
  Uint8Array byteArray(dataLength);
  Uint8Array srcData(arrayBuffer);
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      int srcPos
        = static_cast<int>(dataOffset) + static_cast<int>(x + y * width) * 4;

      byteArray[index]     = srcData[static_cast<size_t>(srcPos + rOffset)];
      byteArray[index + 1] = srcData[static_cast<size_t>(srcPos + gOffset)];
      byteArray[index + 2] = srcData[static_cast<size_t>(srcPos + bOffset)];
      byteArray[index + 3] = srcData[static_cast<size_t>(srcPos + aOffset)];
      index += 4;
    }
  }

  return byteArray;
}

int DDSTools::_ExtractLongWordOrder(int value)
{
  if (value == 0 || value == 255 || value == -16777216) {
    return 0;
  }

  return 1 + DDSTools::_ExtractLongWordOrder(value >> 8);
}

Uint8Array DDSTools::_GetRGBArrayBuffer(float width, float height,
                                        size_t dataOffset, size_t dataLength,
                                        const Uint8Array& arrayBuffer,
                                        int rOffset, int gOffset, int bOffset)
{
  Uint8Array byteArray(dataLength);
  Uint8Array srcData(arrayBuffer);
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      int srcPos
        = static_cast<int>(dataOffset) + static_cast<int>(x + y * width) * 3;

      byteArray[index]     = srcData[static_cast<size_t>(srcPos + rOffset)];
      byteArray[index + 1] = srcData[static_cast<size_t>(srcPos + gOffset)];
      byteArray[index + 2] = srcData[static_cast<size_t>(srcPos + bOffset)];
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
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      size_t srcPos    = dataOffset + static_cast<size_t>(x + y * width);
      byteArray[index] = srcData[srcPos];
      ++index;
    }
  }

  return byteArray;
}

void DDSTools::UploadDDSLevels(Engine* engine, InternalTexture* texture,
                               const Uint8Array& arrayBuffer, DDSInfo& info,
                               bool loadMipmaps, unsigned int faces,
                               int lodIndex, int currentFace)
{
  bool hasSphericalPolynomialFaces = false;
  vector_t<ArrayBufferView> sphericalPolynomialFaces;
  if (info.sphericalPolynomial) {
    hasSphericalPolynomialFaces = true;
  }
  auto ext = engine->getCaps().s3tc;

  Int32Array header(
    reinterpret_cast<const int*>(arrayBuffer.data()),
    reinterpret_cast<const int*>(arrayBuffer.data() + DDS::headerLengthInt));
  int fourCC = 0;
  Uint8Array byteArray;
  int blockBytes                        = 1;
  unsigned int internalCompressedFormat = 0;
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
        blockBytes               = 8;
        internalCompressedFormat = RGB_S3TC_DXT1_Format;
        break;
      case DDS::FOURCC_DXT3:
        blockBytes               = 16;
        internalCompressedFormat = RGBA_S3TC_DXT3_Format;
        break;
      case DDS::FOURCC_DXT5:
        blockBytes               = 16;
        internalCompressedFormat = RGBA_S3TC_DXT5_Format;
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

  auto rOffset = DDSTools::_ExtractLongWordOrder(header[off_RMask]);
  auto gOffset = DDSTools::_ExtractLongWordOrder(header[off_GMask]);
  auto bOffset = DDSTools::_ExtractLongWordOrder(header[off_BMask]);
  auto aOffset = DDSTools::_ExtractLongWordOrder(header[off_AMask]);

  if (computeFormats) {
    internalCompressedFormat
      = engine->_getRGBABufferInternalSizedFormat(info.textureType);
  }

  int mipmapCount = 1;
  if (header[off_flags] & DDSD_MIPMAPCOUNT && loadMipmaps) {
    mipmapCount = ::std::max(1, header[off_mipmapCount]);
  }

  for (unsigned int face = 0; face < faces; ++face) {
    width      = static_cast<float>(header[off_width]);
    height     = static_cast<float>(header[off_height]);
    dataOffset = static_cast<size_t>(header[off_size] + 4);

    for (int mip = 0; mip < mipmapCount; ++mip) {
      if (lodIndex == -1 || lodIndex == mip) {
        // In case of fixed LOD, if the lod has just been uploaded, early exit.
        const int i = (lodIndex == -1) ? mip : 0;

        if (!info.isCompressed && info.isFourCC) {
          // Not implemeted yet
        }
        else if (info.isRGB) {
          texture->type = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
          if (bpp == 24) {
            texture->format = EngineConstants::TEXTUREFORMAT_RGB;
            dataLength      = static_cast<size_t>(width * height * 3);
            byteArray = DDSTools::_GetRGBArrayBuffer(width, height, dataOffset,
                                                     dataLength, arrayBuffer,
                                                     rOffset, gOffset, bOffset);
            engine->_uploadDataToTextureDirectly(texture, byteArray, face, i);
          }
          else { // 32
            texture->format = EngineConstants::TEXTUREFORMAT_RGBA;
            dataLength      = static_cast<size_t>(width * height * 4);
            byteArray       = DDSTools::_GetRGBAArrayBuffer(
              width, height, dataOffset, dataLength, arrayBuffer, rOffset,
              gOffset, bOffset, aOffset);
            engine->_uploadDataToTextureDirectly(texture, byteArray, face, i);
          }
        }
        else if (info.isLuminance) {
          int unpackAlignment   = engine->_getUnpackAlignement();
          float unpaddedRowSize = width;
          float paddedRowSize
            = ::std::floor((width + unpackAlignment - 1) / unpackAlignment)
              * unpackAlignment;
          dataLength = static_cast<size_t>(paddedRowSize * (height - 1)
                                           + unpaddedRowSize);

          byteArray = DDSTools::_GetLuminanceArrayBuffer(
            width, height, dataOffset, dataLength, arrayBuffer);

          texture->format = EngineConstants::TEXTUREFORMAT_LUMINANCE;
          texture->type   = EngineConstants::TEXTURETYPE_UNSIGNED_INT;

          engine->_uploadDataToTextureDirectly(texture, byteArray, face, i);
        }
        else {
          dataLength
            = static_cast<size_t>(::std::max(4.f, width) / 4
                                  * ::std::max(4.f, height) / 4 * blockBytes);
          byteArray = Uint8Array(
            reinterpret_cast<const int*>(arrayBuffer.data() + dataOffset),
            reinterpret_cast<const int*>(arrayBuffer.data() + dataLength));

          texture->type = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
          engine->_uploadCompressedDataToTextureDirectly(
            texture, internalCompressedFormat, width, height, byteArray, face,
            i);
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
  if (hasSphericalPolynomialFaces && sphericalPolynomialFaces.size() >= 6) {
    CubeMapInfo cubeInfo;
    cubeInfo.size       = static_cast<size_t>(header[off_width]);
    cubeInfo.right      = sphericalPolynomialFaces[0];
    cubeInfo.left       = sphericalPolynomialFaces[1];
    cubeInfo.up         = sphericalPolynomialFaces[2];
    cubeInfo.down       = sphericalPolynomialFaces[3];
    cubeInfo.front      = sphericalPolynomialFaces[4];
    cubeInfo.back       = sphericalPolynomialFaces[5];
    cubeInfo.format     = EngineConstants::TEXTUREFORMAT_RGBA;
    cubeInfo.type       = EngineConstants::TEXTURETYPE_FLOAT;
    cubeInfo.gammaSpace = false;

    info.sphericalPolynomial
      = CubeMapToSphericalPolynomialTools::ConvertCubeMapToSphericalPolynomial(
        cubeInfo);
  }
  else {
    info.sphericalPolynomial = nullptr;
  }
}

} // end of namespace BABYLON
