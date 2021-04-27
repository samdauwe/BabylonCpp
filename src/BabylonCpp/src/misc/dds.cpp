#include <babylon/misc/dds.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/maths/scalar.h>
#include <babylon/misc/dds_info.h>
#include <babylon/misc/highdynamicrange/cube_map_to_spherical_polynomial_tools.h>

namespace BABYLON {

bool DDSTools::StoreLODInAlphaChannel = false;
Float32Array DDSTools::_FloatView;
Int32Array DDSTools::_Int32View;

DDSInfo DDSTools::GetDDSInfo(const std::variant<std::string, ArrayBufferView>& iArrayBuffer)
{
  const auto dataBuffer = ToArrayBuffer(iArrayBuffer);
  const auto byteOffset = GetByteOffset(iArrayBuffer);

  auto header = stl_util::to_array<int32_t>(dataBuffer, byteOffset, DDS::headerLengthInt);
  auto extendedHeader
    = stl_util::to_array<int32_t>(dataBuffer, byteOffset, DDS::headerLengthInt + 4);

  auto mipmapCount = 1;
  if (header[off_flags] & DDSD_MIPMAPCOUNT) {
    mipmapCount = std::max(1, header[off_mipmapCount]);
  }

  auto fourCC      = header[off_pfFourCC];
  auto dxgiFormat  = (fourCC == DDS::FOURCC_DX10) ? extendedHeader[off_dxgiFormat] : 0;
  auto textureType = Constants::TEXTURETYPE_UNSIGNED_INT;

  switch (fourCC) {
    case DDS::FOURCC_D3DFMT_R16G16B16A16F:
      textureType = Constants::TEXTURETYPE_HALF_FLOAT;
      break;
    case DDS::FOURCC_D3DFMT_R32G32B32A32F:
      textureType = Constants::TEXTURETYPE_FLOAT;
      break;
    case DDS::FOURCC_DX10: {
      if (dxgiFormat == DDS::DXGI_FORMAT_R16G16B16A16_FLOAT) {
        textureType = Constants::TEXTURETYPE_HALF_FLOAT;
        break;
      }
      if (dxgiFormat == DDS::DXGI_FORMAT_R32G32B32A32_FLOAT) {
        textureType = Constants::TEXTURETYPE_FLOAT;
        break;
      }
    }
  }

  return DDSInfo(
    // width
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
    (fourCC == DDS::FOURCC_DXT1 || fourCC == DDS::FOURCC_DXT3 || fourCC == DDS::FOURCC_DXT5),
    // dxgiFormat
    dxgiFormat,
    // textureType
    textureType,
    // sphericalPolynomial
    nullptr);
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

  /* If zero, or denormal, or exponent underflows too much for a denormal half, return signed zero.
   */
  if (e < 103) {
    return static_cast<float>(bits);
  }

  /* If NaN, return NaN. If Inf or exponent overflow, return Inf. */
  if (e > 142) {
    bits |= 0x7c00;
    /* If exponent was 0xff and one mantissa bit was set, it means NaN, not Inf, so make sure we set
     * one mantissa bit too. */
    bits |= ((e == 255) ? 0 : 1) && (x & 0x007fffff);
    return static_cast<float>(bits);
  }

  /* If exponent underflows but not too much, return a denormal */
  if (e < 113) {
    m |= 0x0800;
    /* Extra rounding may overflow and set mantissa to 0 and exponent to 1, which is OK. */
    bits |= (m >> (114 - e)) + ((m >> (113 - e)) & 1);
    return static_cast<float>(bits);
  }

  bits |= ((e - 112) << 10) | (m >> 1);
  bits += m & 1;
  return static_cast<float>(bits);
}

float DDSTools::_FromHalfFloat(uint16_t value)
{
  const auto s = (value & 0x8000) >> 15;
  const auto e = (value & 0x7C00) >> 10;
  const auto f = value & 0x03FF;

  if (e == 0) {
    auto result = (s ? -1 : 1) * std::pow(2, -14) * (f / std::pow(2, 10));
    return static_cast<float>(result);
  }
  else if (e == 0x1F) {
    return f ? 0.f : ((s ? -1.f : 1.f) * std::numeric_limits<int>::infinity());
  }

  auto result = (s ? -1 : 1) * std::pow(2, e - 15) * (1 + (f / std::pow(2, 10)));
  return static_cast<float>(result);
}

Float32Array DDSTools::_GetHalfFloatAsFloatRGBAArrayBuffer(float width, float height,
                                                           int dataOffset, size_t dataLength,
                                                           const Uint8Array& arrayBuffer, int lod)
{
  Float32Array destArray(dataLength);
  const Uint16Array srcData(
    stl_util::to_array<uint16_t>(arrayBuffer, static_cast<size_t>(dataOffset)));
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      auto srcPos          = static_cast<size_t>((x + y * width) * 4);
      destArray[index]     = DDSTools::_FromHalfFloat(srcData[srcPos]);
      destArray[index + 1] = DDSTools::_FromHalfFloat(srcData[srcPos + 1]);
      destArray[index + 2] = DDSTools::_FromHalfFloat(srcData[srcPos + 2]);
      if (DDSTools::StoreLODInAlphaChannel) {
        destArray[index + 3] = static_cast<float>(lod);
      }
      else {
        destArray[index + 3] = DDSTools::_FromHalfFloat(srcData[srcPos + 3]);
      }
      index += 4;
    }
  }

  return destArray;
}

Uint16Array DDSTools::_GetHalfFloatRGBAArrayBuffer(float width, float height, int dataOffset,
                                                   size_t dataLength, const Uint8Array& arrayBuffer,
                                                   int lod)
{
  if (DDSTools::StoreLODInAlphaChannel) {
    Uint16Array destArray(dataLength);
    const Uint16Array srcData(
      stl_util::to_array<uint16_t>(arrayBuffer, static_cast<size_t>(dataOffset)));
    size_t index = 0;
    for (float y = 0; y < height; ++y) {
      for (float x = 0; x < width; ++x) {
        auto srcPos          = static_cast<size_t>((x + y * width) * 4);
        destArray[index]     = srcData[srcPos];
        destArray[index + 1] = srcData[srcPos + 1];
        destArray[index + 2] = srcData[srcPos + 2];
        destArray[index + 3]
          = static_cast<uint8_t>(DDSTools::_ToHalfFloat(static_cast<float>(lod)));
        index += 4;
      }
    }

    return destArray;
  }

  return stl_util::to_array<uint16_t>(arrayBuffer, static_cast<size_t>(dataOffset), dataLength);
}

Float32Array DDSTools::_GetFloatRGBAArrayBuffer(float width, float height, int dataOffset,
                                                size_t dataLength, const Uint8Array& arrayBuffer,
                                                int lod)
{
  if (DDSTools::StoreLODInAlphaChannel) {
    Float32Array destArray(dataLength);
    Float32Array srcData(stl_util::to_array<float>(arrayBuffer, static_cast<size_t>(dataOffset)));
    size_t index = 0;
    for (float y = 0; y < height; ++y) {
      for (float x = 0; x < width; ++x) {
        auto srcPos          = static_cast<size_t>((x + y * width) * 4);
        destArray[index]     = srcData[srcPos];
        destArray[index + 1] = srcData[srcPos + 1];
        destArray[index + 2] = srcData[srcPos + 2];
        destArray[index + 3] = static_cast<float>(lod);
        index += 4;
      }
    }

    return destArray;
  }
  return stl_util::to_array<float>(arrayBuffer, static_cast<size_t>(dataOffset), dataLength);
}

Uint8Array DDSTools::_GetFloatAsUIntRGBAArrayBuffer(float width, float height, int dataOffset,
                                                    size_t dataLength,
                                                    const Uint8Array& arrayBuffer, int lod)
{
  Uint8Array destArray(dataLength);
  const Float32Array srcData(
    stl_util::to_array<float>(arrayBuffer, static_cast<size_t>(dataOffset)));
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      auto srcPos          = static_cast<size_t>((x + y * width) * 4);
      destArray[index]     = static_cast<uint8_t>(Scalar::Clamp(srcData[srcPos]) * 255);
      destArray[index + 1] = static_cast<uint8_t>(Scalar::Clamp(srcData[srcPos + 1]) * 255);
      destArray[index + 2] = static_cast<uint8_t>(Scalar::Clamp(srcData[srcPos + 2]) * 255);
      if (DDSTools::StoreLODInAlphaChannel) {
        destArray[index + 3] = static_cast<uint8_t>(lod);
      }
      else {
        destArray[index + 3] = static_cast<uint8_t>(Scalar::Clamp(srcData[srcPos + 3]) * 255);
      }
      index += 4;
    }
  }

  return destArray;
}

Uint8Array DDSTools::_GetHalfFloatAsUIntRGBAArrayBuffer(float width, float height, int dataOffset,
                                                        size_t dataLength,
                                                        const Uint8Array& arrayBuffer, int lod)
{
  Uint8Array destArray(dataLength);
  const Uint16Array srcData(
    stl_util::to_array<uint16_t>(arrayBuffer, static_cast<size_t>(dataOffset)));
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      auto srcPos = static_cast<size_t>((x + y * width) * 4);

      destArray[index]
        = static_cast<uint8_t>(Scalar::Clamp(DDSTools::_FromHalfFloat(srcData[srcPos])) * 255);
      destArray[index + 1]
        = static_cast<uint8_t>(Scalar::Clamp(DDSTools::_FromHalfFloat(srcData[srcPos + 1])) * 255);
      destArray[index + 2]
        = static_cast<uint8_t>(Scalar::Clamp(DDSTools::_FromHalfFloat(srcData[srcPos + 2])) * 255);
      if (DDSTools::StoreLODInAlphaChannel) {
        destArray[index + 3] = static_cast<uint8_t>(lod);
      }
      else {
        destArray[index + 3] = static_cast<uint8_t>(
          Scalar::Clamp(DDSTools::_FromHalfFloat(srcData[srcPos + 3])) * 255);
      }
      index += 4;
    }
  }

  return destArray;
}

Uint8Array DDSTools::_GetRGBAArrayBuffer(float width, float height, int dataOffset,
                                         size_t dataLength, const Uint8Array& arrayBuffer,
                                         int rOffset, int gOffset, int bOffset, int aOffset)
{
  Uint8Array byteArray(dataLength);
  const Uint8Array srcData(
    stl_util::to_array<uint8_t>(arrayBuffer, static_cast<size_t>(dataOffset)));
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      auto srcPos = static_cast<int>(x + y * width) * 4;

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

Uint8Array DDSTools::_GetRGBArrayBuffer(float width, float height, int dataOffset,
                                        size_t dataLength, const Uint8Array& arrayBuffer,
                                        int rOffset, int gOffset, int bOffset)
{
  Uint8Array byteArray(dataLength);
  const Uint8Array srcData(
    stl_util::to_array<uint8_t>(arrayBuffer, static_cast<size_t>(dataOffset)));
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      auto srcPos = static_cast<int>(x + y * width) * 3;

      byteArray[index]     = srcData[static_cast<size_t>(srcPos + rOffset)];
      byteArray[index + 1] = srcData[static_cast<size_t>(srcPos + gOffset)];
      byteArray[index + 2] = srcData[static_cast<size_t>(srcPos + bOffset)];
      index += 3;
    }
  }

  return byteArray;
}

Uint8Array DDSTools::_GetLuminanceArrayBuffer(float width, float height, int dataOffset,
                                              size_t dataLength, const Uint8Array& arrayBuffer)
{
  Uint8Array byteArray(dataLength);
  const Uint8Array srcData(
    stl_util::to_array<uint8_t>(arrayBuffer, static_cast<size_t>(dataOffset)));
  size_t index = 0;
  for (float y = 0; y < height; ++y) {
    for (float x = 0; x < width; ++x) {
      auto srcPos      = static_cast<size_t>(x + y * width);
      byteArray[index] = srcData[srcPos];
      ++index;
    }
  }

  return byteArray;
}

void DDSTools::UploadDDSLevels(ThinEngine* engine, const InternalTexturePtr& texture,
                               const std::variant<std::string, ArrayBufferView>& iArrayBuffer,
                               DDSInfo& info, bool loadMipmaps, unsigned int faces, int lodIndex,
                               int currentFace)
{
  auto hasSphericalPolynomialFaces = false;
  std::vector<ArrayBufferView> sphericalPolynomialFaces;
  if (info.sphericalPolynomial) {
    hasSphericalPolynomialFaces = true;
  }
  const auto ext = !!engine->getCaps().s3tc;

  // TODO WEBGPU Once generateMipMaps is split into generateMipMaps + hasMipMaps in InternalTexture
  // this line can be removed
  texture->generateMipMaps = loadMipmaps;

  const auto dataBuffer = ToArrayBuffer(iArrayBuffer);
  const auto byteOffset = static_cast<int>(GetByteOffset(iArrayBuffer));
  Int32Array header     = stl_util::to_array<int32_t>(dataBuffer, static_cast<size_t>(byteOffset),
                                                  DDS::headerLengthInt);
  int fourCC            = 0;
  Uint8Array byteArray;
  int blockBytes                        = 1;
  unsigned int internalCompressedFormat = 0;
  float width = 0.f, height = 0.f;
  int dataOffset    = 0;
  size_t dataLength = 0;

  if (header[off_magic] != DDS_MAGIC) {
    BABYLON_LOG_ERROR("DDSTools", "Invalid magic number in DDS header")
    return;
  }

  if (!info.isFourCC && !info.isRGB && !info.isLuminance) {
    BABYLON_LOG_ERROR("DDSTools", "Unsupported format, must contain a FourCC code")
    return;
  }

  if (info.isCompressed && !ext) {
    BABYLON_LOG_ERROR("DDSTools", "Compressed textures are not supported on this platform.")
    return;
  }

  auto bpp   = header[off_RGBbpp];
  dataOffset = header[off_size] + 4;

  auto computeFormats = false;

  if (info.isFourCC) {
    fourCC = header[off_pfFourCC];
    switch (fourCC) {
      case DDS::FOURCC_DXT1:
        blockBytes               = 8;
        internalCompressedFormat = Constants::TEXTUREFORMAT_COMPRESSED_RGBA_S3TC_DXT1;
        break;
      case DDS::FOURCC_DXT3:
        blockBytes               = 16;
        internalCompressedFormat = Constants::TEXTUREFORMAT_COMPRESSED_RGBA_S3TC_DXT3;
        break;
      case DDS::FOURCC_DXT5:
        blockBytes               = 16;
        internalCompressedFormat = Constants::TEXTUREFORMAT_COMPRESSED_RGBA_S3TC_DXT5;
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
          case DDS::DXGI_FORMAT_R32G32B32A32_FLOAT:
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
        BABYLON_LOG_ERROR("DDSTools", "Unsupported FourCC code: ", Int32ToFourCC(fourCC))
        return;
    }
  }

  auto rOffset = DDSTools::_ExtractLongWordOrder(header[off_RMask]);
  auto gOffset = DDSTools::_ExtractLongWordOrder(header[off_GMask]);
  auto bOffset = DDSTools::_ExtractLongWordOrder(header[off_BMask]);
  auto aOffset = DDSTools::_ExtractLongWordOrder(header[off_AMask]);

  if (computeFormats) {
    internalCompressedFormat = engine->_getRGBABufferInternalSizedFormat(info.textureType);
  }

  auto mipmapCount = 1;
  if (header[off_flags] & DDSD_MIPMAPCOUNT && loadMipmaps) {
    mipmapCount = std::max(1, header[off_mipmapCount]);
  }

  const auto startFace = currentFace >= 0 ? static_cast<unsigned>(currentFace) : 0u;
  for (unsigned int face = startFace; face < faces; ++face) {
    width  = static_cast<float>(header[off_width]);
    height = static_cast<float>(header[off_height]);

    for (int mip = 0; mip < mipmapCount; ++mip) {
      if (lodIndex == -1 || lodIndex == mip) {
        // In case of fixed LOD, if the lod has just been uploaded, early exit.
        const int i = (lodIndex == -1) ? mip : 0;

        if (!info.isCompressed && info.isFourCC) {
          texture->format = Constants::TEXTUREFORMAT_RGBA;
          dataLength      = static_cast<size_t>(width * height * 4);
          ArrayBufferView floatArray;

          if (engine->_badOS || engine->_badDesktopOS
              || (!engine->getCaps().textureHalfFloat
                  && !engine->getCaps().textureFloat)) { // Required because iOS has many
                                                         // issues with float and half float
                                                         // generation
            if (bpp == 128) {
              floatArray = DDSTools::_GetFloatAsUIntRGBAArrayBuffer(
                width, height, byteOffset + dataOffset, dataLength, dataBuffer, i);
              if (i == 0) {
                sphericalPolynomialFaces.emplace_back(DDSTools::_GetFloatRGBAArrayBuffer(
                  width, height, byteOffset + dataOffset, dataLength, dataBuffer, i));
              }
            }
            else if (bpp == 64) {
              floatArray = DDSTools::_GetHalfFloatAsUIntRGBAArrayBuffer(
                width, height, byteOffset + dataOffset, dataLength, dataBuffer, i);
              if (i == 0) {
                sphericalPolynomialFaces.emplace_back(DDSTools::_GetHalfFloatAsFloatRGBAArrayBuffer(
                  width, height, byteOffset + dataOffset, dataLength, dataBuffer, i));
              }
            }

            texture->type = Constants::TEXTURETYPE_UNSIGNED_INT;
          }
          else {
            if (bpp == 128) {
              texture->type = Constants::TEXTURETYPE_FLOAT;
              floatArray    = DDSTools::_GetFloatRGBAArrayBuffer(
                width, height, byteOffset + dataOffset, dataLength, dataBuffer, i);
              if (i == 0) {
                sphericalPolynomialFaces.emplace_back(floatArray);
              }
            }
            else if (bpp == 64 && !engine->getCaps().textureHalfFloat) {
              texture->type = Constants::TEXTURETYPE_FLOAT;
              floatArray    = DDSTools::_GetHalfFloatAsFloatRGBAArrayBuffer(
                width, height, byteOffset + dataOffset, dataLength, dataBuffer, i);
              if (i == 0) {
                sphericalPolynomialFaces.emplace_back(floatArray);
              }
            }
            else { // 64
              texture->type = Constants::TEXTURETYPE_HALF_FLOAT;
              floatArray    = DDSTools::_GetHalfFloatRGBAArrayBuffer(
                width, height, byteOffset + dataOffset, dataLength, dataBuffer, i);
              if (i == 0) {
                sphericalPolynomialFaces.emplace_back(DDSTools::_GetHalfFloatAsFloatRGBAArrayBuffer(
                  width, height, byteOffset + dataOffset, dataLength, dataBuffer, i));
              }
            }
          }

          if (floatArray) {
            engine->_uploadDataToTextureDirectly(texture, floatArray, face, i);
          }
        }
        else if (info.isRGB) {
          texture->type = Constants::TEXTURETYPE_UNSIGNED_INT;
          if (bpp == 24) {
            texture->format = Constants::TEXTUREFORMAT_RGB;
            dataLength      = static_cast<size_t>(width * height * 3);
            byteArray
              = DDSTools::_GetRGBArrayBuffer(width, height, byteOffset + dataOffset, dataLength,
                                             dataBuffer, rOffset, gOffset, bOffset);
            engine->_uploadDataToTextureDirectly(texture, byteArray, face, i);
          }
          else { // 32
            texture->format = Constants::TEXTUREFORMAT_RGBA;
            dataLength      = static_cast<size_t>(width * height * 4);
            byteArray
              = DDSTools::_GetRGBAArrayBuffer(width, height, byteOffset + dataOffset, dataLength,
                                              dataBuffer, rOffset, gOffset, bOffset, aOffset);
            engine->_uploadDataToTextureDirectly(texture, byteArray, face, i);
          }
        }
        else if (info.isLuminance) {
          int unpackAlignment   = engine->_getUnpackAlignement();
          float unpaddedRowSize = width;
          float paddedRowSize
            = std::floor((width + unpackAlignment - 1) / unpackAlignment) * unpackAlignment;
          dataLength = static_cast<size_t>(paddedRowSize * (height - 1) + unpaddedRowSize);

          byteArray = DDSTools::_GetLuminanceArrayBuffer(width, height, byteOffset + dataOffset,
                                                         dataLength, dataBuffer);

          texture->format = Constants::TEXTUREFORMAT_LUMINANCE;
          texture->type   = Constants::TEXTURETYPE_UNSIGNED_INT;

          engine->_uploadDataToTextureDirectly(texture, byteArray, face, i);
        }
        else {
          dataLength = static_cast<size_t>(std::max(4.f, width) / 4 * std::max(4.f, height) / 4
                                           * blockBytes);
          byteArray  = stl_util::to_array<uint8_t>(
            dataBuffer, static_cast<size_t>(byteOffset + dataOffset), dataLength);

          texture->type = Constants::TEXTURETYPE_UNSIGNED_INT;
          engine->_uploadCompressedDataToTextureDirectly(
            texture, internalCompressedFormat, static_cast<int>(width), static_cast<int>(height),
            byteArray, face, i);
        }
      }
      dataOffset
        += bpp ? static_cast<int>(width * height * (bpp / 8)) : static_cast<int>(dataLength);
      width *= 0.5f;
      height *= 0.5f;

      width  = std::max(1.f, width);
      height = std::max(1.f, height);
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
    cubeInfo.format     = Constants::TEXTUREFORMAT_RGBA;
    cubeInfo.type       = Constants::TEXTURETYPE_FLOAT;
    cubeInfo.gammaSpace = false;

    info.sphericalPolynomial
      = CubeMapToSphericalPolynomialTools::ConvertCubeMapToSphericalPolynomial(cubeInfo);
  }
  else {
    info.sphericalPolynomial = nullptr;
  }
}

ArrayBuffer DDSTools::ToArrayBuffer(const std::variant<std::string, ArrayBufferView>& arrayBuffer)
{
  ArrayBuffer byteArray;
  if (std::holds_alternative<std::string>(arrayBuffer)) {
    auto charArray = StringTools::toCharArray(std::get<std::string>(arrayBuffer));
    byteArray      = stl_util::to_array<uint8_t>(charArray);
  }
  else {
    byteArray = std::get<ArrayBufferView>(arrayBuffer).uint8Array();
  }
  return byteArray;
}

size_t DDSTools::GetByteOffset(const std::variant<std::string, ArrayBufferView>& arrayBuffer)
{
  size_t byteOffset = 0;
  if (std::holds_alternative<ArrayBufferView>(arrayBuffer)) {
    byteOffset = std::get<ArrayBufferView>(arrayBuffer).byteOffset;
  }
  return byteOffset;
}

} // end of namespace BABYLON
