#include <babylon/tools/hdr/hdr_tools.h>

#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/tools/hdr/panorama_to_cube_map_tools.h>

namespace BABYLON {
namespace Internals {

float HDRTools::Ldexp(float mantissa, float exponent)
{
  if (exponent > 1023.f) {
    return mantissa * ::std::pow(2.f, 1023.f)
           * ::std::pow(2.f, exponent - 1023.f);
  }

  if (exponent < -1074.f) {
    return mantissa * ::std::pow(2.f, -1074.f)
           * ::std::pow(2.f, exponent + 1074.f);
  }

  return mantissa * ::std::pow(2.f, exponent);
}

void HDRTools::Rgbe2float(Float32Array& float32array, float red, float green,
                          float blue, float exponent, size_t index)
{
  if (exponent > 0.f) { /*nonzero pixel*/
    exponent = Ldexp(1.f, exponent - (128.f + 8.f));

    float32array[index + 0] = red * exponent;
    float32array[index + 1] = green * exponent;
    float32array[index + 2] = blue * exponent;
  }
  else {
    float32array[index + 0] = 0.f;
    float32array[index + 1] = 0.f;
    float32array[index + 2] = 0.f;
  }
}

string_t HDRTools::readStringLine(const Uint8Array& uint8array,
                                  size_t startIndex)
{
  std::ostringstream line;
  string_t character;

  for (size_t i = startIndex; i < uint8array.size() - startIndex; ++i) {
    character = String::fromCharCode(uint8array[i]);

    if (character == "\n") {
      break;
    }

    line << character;
  }

  return line.str();
}

HDRInfo HDRTools::RGBE_ReadHeader(const Uint8Array& uint8array)
{
  HDRInfo headerInfo;

  size_t height = 0;
  size_t width  = 0;

  string_t line = readStringLine(uint8array, 0);
  if (line.at(0) != '#' || line.at(1) != '?') {
    headerInfo.errorMessage = "Bad HDR Format.";
    return headerInfo;
  }

  bool endOfHeader = false;
  bool findFormat  = false;
  size_t lineIndex = 0;

  do {
    lineIndex += (line.size() + 1);
    line = readStringLine(uint8array, lineIndex);

    if (line == "FORMAT=32-bit_rle_rgbe") {
      findFormat = true;
    }
    else if (line.empty()) {
      endOfHeader = true;
    }
  } while (!endOfHeader);

  if (!findFormat) {
    headerInfo.errorMessage = "HDR Bad header format, unsupported FORMAT";
    return headerInfo;
  }

  lineIndex += (line.size() + 1);
  line = readStringLine(uint8array, lineIndex);

  const ::std::regex sizeRegexp("-Y (.*) +X (.*)$/g", ::std::regex::optimize);
  ::std::smatch match;

  if (::std::regex_search(line, match, sizeRegexp) && (match.size() == 3)) {
    // Shader include found
    width  = ::std::stoul(match.str(2));
    height = ::std::stoul(match.str(1));
  }
  else {
    headerInfo.errorMessage = "HDR Bad header format, no size";
    return headerInfo;
  }

  if (width < 8 || width > 0x7fff) {
    headerInfo.errorMessage = "HDR Bad header format, unsupported size";
    return headerInfo;
  }

  lineIndex += (line.size() + 1);

  headerInfo.height       = height;
  headerInfo.width        = width;
  headerInfo.dataPosition = lineIndex;
  headerInfo.isValid      = true;

  return headerInfo;
}

CubeMapInfo HDRTools::GetCubeMapTextureData(const Uint8Array& buffer,
                                            size_t size)
{
  HDRInfo hdrInfo   = RGBE_ReadHeader(buffer);
  Float32Array data = RGBE_ReadPixels_RLE(buffer, hdrInfo);

  return PanoramaToCubeMapTools::ConvertPanoramaToCubemap(data, hdrInfo.width,
                                                          hdrInfo.height, size);
}

Float32Array HDRTools::RGBE_ReadPixels(const Uint8Array& uint8array,
                                       const HDRInfo& hdrInfo)
{
  // Keep for multi format supports.
  return RGBE_ReadPixels_RLE(uint8array, hdrInfo);
}

Float32Array HDRTools::RGBE_ReadPixels_RLE(const Uint8Array& uint8array,
                                           const HDRInfo& hdrInfo)
{
  size_t num_scanlines  = hdrInfo.height;
  size_t scanline_width = hdrInfo.width;

  std::uint8_t a, b, c, d, count;
  size_t dataIndex = hdrInfo.dataPosition;
  size_t index = 0, endIndex = 0, i = 0;

  Uint8Array scanLineArray(scanline_width * 4); // four channel R G B E

  // 3 channels of 4 bytes per pixel in float.
  Float32Array resultArray(hdrInfo.width * hdrInfo.height * 4 * 3);

  // read in each successive scanline
  while (num_scanlines > 0) {
    a = uint8array[dataIndex++];
    b = uint8array[dataIndex++];
    c = uint8array[dataIndex++];
    d = uint8array[dataIndex++];

    if (a != 2 || b != 2 || (c & 0x80)) {
      // this file is not run length encoded
      BABYLON_LOG_ERROR("HDRTools", "HDR Bad header format, not RLE");
      return resultArray;
    }

    if (((c << 8) | d) != static_cast<std::uint8_t>(scanline_width)) {
      BABYLON_LOG_ERROR("HDRTools",
                        "HDR Bad header format, wrong scan line width");
      return resultArray;
    }

    index = 0;

    // read each of the four channels for the scanline into the buffer
    for (i = 0; i < 4; i++) {
      endIndex = (i + 1) * scanline_width;

      while (index < endIndex) {
        a = uint8array[dataIndex++];
        b = uint8array[dataIndex++];

        if (a > 128) {
          // a run of the same value
          count = static_cast<std::uint8_t>(a - 128);
          if ((count == 0) || (count > endIndex - index)) {
            BABYLON_LOG_ERROR("HDRTools",
                              "HDR Bad Format, bad scanline data (run)");
            return resultArray;
          }

          while (count-- > 0) {
            scanLineArray[index++] = b;
          }
        }
        else {
          // a non-run
          count = a;
          if ((count == 0) || (count > endIndex - index)) {
            BABYLON_LOG_ERROR("HDRTools",
                              "HDR Bad Format, bad scanline data (non-run)");
            return resultArray;
          }

          scanLineArray[index++] = b;
          if (--count > 0) {
            for (size_t j = 0; j < count; j++) {
              scanLineArray[index++] = uint8array[dataIndex++];
            }
          }
        }
      }
    }

    // now convert data from buffer into floats
    for (i = 0; i < scanline_width; i++) {
      a = scanLineArray[i];
      b = scanLineArray[i + scanline_width];
      c = scanLineArray[i + 2 * scanline_width];
      d = scanLineArray[i + 3 * scanline_width];

      Rgbe2float(resultArray, a, b, c, d,
                 (hdrInfo.height - num_scanlines) * scanline_width * 3 + i * 3);
    }

    --num_scanlines;
  }

  return resultArray;
}

} // end of namespace Internals
} // end of namespace BABYLON
