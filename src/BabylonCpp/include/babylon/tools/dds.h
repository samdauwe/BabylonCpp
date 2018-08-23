#ifndef BABYLON_TOOLS_DDS_H
#define BABYLON_TOOLS_DDS_H

#include <babylon/babylon_global.h>
#include <babylon/math/spherical_polynomial.h>

namespace BABYLON {

// Based on demo done by Brandon Jones -
// http://media.tojicode.com/webgl-samples/dds.html
// All values and structures referenced from:
// http://msdn.microsoft.com/en-us/library/bb943991.aspx/

enum { DDS_MAGIC = 0x20534444 };

enum {
  DDSD_CAPS        = 0x1,
  DDSD_HEIGHT      = 0x2,
  DDSD_WIDTH       = 0x4,
  DDSD_PITCH       = 0x8,
  DDSD_PIXELFORMAT = 0x1000,
  DDSD_MIPMAPCOUNT = 0x20000,
  DDSD_LINEARSIZE  = 0x80000,
  DDSD_DEPTH       = 0x800000
};

enum {
  DDSCAPS_COMPLEX = 0x8,
  DDSCAPS_MIPMAP  = 0x400000,
  DDSCAPS_TEXTURE = 0x1000
};

enum {
  DDSCAPS2_CUBEMAP           = 0x200,
  DDSCAPS2_CUBEMAP_POSITIVEX = 0x400,
  DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800,
  DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,
  DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,
  DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,
  DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000,
  DDSCAPS2_VOLUME            = 0x200000
};

enum {
  DDPF_ALPHAPIXELS = 0x1,
  DDPF_ALPHA       = 0x2,
  DDPF_FOURCC      = 0x4,
  DDPF_RGB         = 0x40,
  DDPF_YUV         = 0x200,
  DDPF_LUMINANCE   = 0x20000
};

template <int a, int b, int c, int d>
struct FourCCToInt32 {
  static const unsigned int value = (((((d << 8) | c) << 8) | b) << 8) | a;
};

inline string_t Int32ToFourCC(int value)
{
  string_t s;
  s += (static_cast<char>(value & 0xff));
  s += (static_cast<char>((value >> 8) & 0xff));
  s += (static_cast<char>((value >> 16) & 0xff));
  s += (static_cast<char>((value >> 24) & 0xff));
  return s;
}

struct DDS {
  static constexpr unsigned int FOURCC_DXT1
    = FourCCToInt32<'D', 'X', 'T', '1'>::value;
  static constexpr int FOURCC_DXT3 = FourCCToInt32<'D', 'X', 'T', '3'>::value;
  static constexpr int FOURCC_DXT5 = FourCCToInt32<'D', 'X', 'T', '5'>::value;
  static constexpr int FOURCC_DX10 = FourCCToInt32<'D', 'X', '1', '0'>::value;
  static constexpr int FOURCC_D3DFMT_R16G16B16A16F = 113;
  static constexpr int FOURCC_D3DFMT_R32G32B32A32F = 116;

  static constexpr int DXGI_FORMAT_R16G16B16A16_FLOAT = 10;
  static constexpr int DXGI_FORMAT_B8G8R8X8_UNORM     = 88;

  // The header length in 32 bit ints
  static constexpr int headerLengthInt = 31;
}; // end of struct DDS

// Offsets into the header array
enum {
  off_magic       = 0,
  off_size        = 1,
  off_flags       = 2,
  off_height      = 3,
  off_width       = 4,
  off_mipmapCount = 7,
  off_pfFlags     = 20,
  off_pfFourCC    = 21,
  off_RGBbpp      = 22,
  off_RMask       = 23,
  off_GMask       = 24,
  off_BMask       = 25,
  off_AMask       = 26,
  off_caps1       = 27,
  off_caps2       = 28,
  off_caps3       = 29,
  off_caps4       = 30,
  off_dxgiFormat  = 32
};

enum PixelFormat {
  AlphaFormat          = 1019,
  RGBFormat            = 1020,
  RGBAFormat           = 1021,
  LuminanceFormat      = 1022,
  LuminanceAlphaFormat = 1023,
  // JD - Added
  BGRFormat  = 1024,
  BGRAFormat = 1025,

  RGB_S3TC_DXT1_Format  = 2001,
  RGBA_S3TC_DXT1_Format = 2002,
  RGBA_S3TC_DXT3_Format = 2003,
  RGBA_S3TC_DXT5_Format = 2004
}; // end of enum PixelFormat

struct DDSInfo {
  int width;
  int height;
  int mipmapCount;
  bool isFourCC;
  bool isRGB;
  bool isLuminance;
  bool isCube;
  bool isCompressed;
  int dxgiFormat;
  unsigned int textureType;
  /** Sphericle polynomial created for the dds texture */
  unique_ptr_t<SphericalPolynomial> sphericalPolynomial = nullptr;
}; // end of struct DDSInfo

class BABYLON_SHARED_EXPORT DDSTools {

private:
  static float _ToHalfFloat(float value);
  static float _FromHalfFloat(uint8_t value);
  static Float32Array
  _GetHalfFloatAsFloatRGBAArrayBuffer(float width, float height,
                                      size_t dataOffset, size_t dataLength,
                                      const Uint8Array& arrayBuffer, float lod);
  static Uint16Array _GetHalfFloatRGBAArrayBuffer(float width, float height,
                                                  size_t dataOffset,
                                                  size_t dataLength,
                                                  const Uint8Array& arrayBuffer,
                                                  float lod);
  static Float32Array _GetFloatRGBAArrayBuffer(float width, float height,
                                               size_t dataOffset,
                                               size_t dataLength,
                                               const Uint8Array& arrayBuffer,
                                               float lod);
  static Float32Array
  _GetFloatAsUIntRGBAArrayBuffer(float width, float height, size_t dataOffset,
                                 size_t dataLength,
                                 const Uint8Array& arrayBuffer, float lod);
  static Float32Array
  _GetHalfFloatAsUIntRGBAArrayBuffer(float width, float height,
                                     size_t dataOffset, size_t dataLength,
                                     const Uint8Array& arrayBuffer, float lod);
  static Uint8Array _GetRGBAArrayBuffer(float width, float height,
                                        size_t dataOffset, size_t dataLength,
                                        const Uint8Array& arrayBuffer,
                                        int rOffset, int gOffset, int bOffset,
                                        int aOffset);
  static int _ExtractLongWordOrder(int value);
  static Uint8Array _GetRGBArrayBuffer(float width, float height,
                                       size_t dataOffset, size_t dataLength,
                                       const Uint8Array& arrayBuffer,
                                       int rOffset, int gOffset, int bOffset);
  static Uint8Array _GetLuminanceArrayBuffer(float width, float height,
                                             size_t dataOffset,
                                             size_t dataLength,
                                             const Uint8Array& arrayBuffer);

public:
  static DDSInfo GetDDSInfo(const Uint8Array& arrayBuffer);

  /**
   * @brief Uploads DDS Levels to a Babylon Texture.
   */
  static void UploadDDSLevels(Engine* engine, InternalTexture* texture,
                              const Uint8Array& arrayBuffer, DDSInfo& info,
                              bool loadMipmaps, unsigned int faces,
                              int lodIndex = -1, int currentFace = -1);

public:
  static bool StoreLODInAlphaChannel;
  static Float32Array _FloatView;
  static Int32Array _Int32View;

}; // end of class DDSTools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_DDS_H
