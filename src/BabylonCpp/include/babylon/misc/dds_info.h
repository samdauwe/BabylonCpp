#ifndef BABYLON_MISC_DDS_INFO_H
#define BABYLON_MISC_DDS_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/math/spherical_polynomial.h>

namespace BABYLON {

using SphericalPolynomialPtr = std::shared_ptr<SphericalPolynomial>;

/**
 * @brief Direct draw surface info.
 * @see
 * https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dx-graphics-dds-pguide
 */
class BABYLON_SHARED_EXPORT DDSInfo {

public:
  DDSInfo();
  DDSInfo(int width, int height, int mipmapCount, bool isFourCC, bool isRGB,
          bool isLuminance, bool isCube, bool isCompressed, int dxgiFormat,
          unsigned int textureType, SphericalPolynomialPtr sphericalPolynomial);
  DDSInfo(const DDSInfo& other);
  DDSInfo(DDSInfo&& other);
  DDSInfo& operator=(const DDSInfo& other);
  DDSInfo& operator=(DDSInfo&& other);
  ~DDSInfo(); // = default

public:
  /**
   * Width of the texture
   */
  int width;

  /**
   * Height of the texture
   */
  int height;

  /**
   * Number of Mipmaps for the texture
   * @see https://en.wikipedia.org/wiki/Mipmap
   */
  int mipmapCount;

  /**
   * If the textures format is a known fourCC format
   * @see https://www.fourcc.org/
   */
  bool isFourCC;

  /**
   * If the texture is an RGB format eg. DXGI_FORMAT_B8G8R8X8_UNORM format
   */
  bool isRGB;

  /**
   * If the texture is a lumincance format
   */
  bool isLuminance;

  /**
   * If this is a cube texture
   * @see
   * https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dds-file-layout-for-cubic-environment-maps
   */
  bool isCube;

  /**
   * If the texture is a compressed format eg. FOURCC_DXT1
   */
  bool isCompressed;

  /**
   * The dxgiFormat of the texture
   * @see
   * https://docs.microsoft.com/en-us/windows/desktop/api/dxgiformat/ne-dxgiformat-dxgi_format
   */
  int dxgiFormat;

  /**
   * Texture type eg. Engine.TEXTURETYPE_UNSIGNED_INT, Engine.TEXTURETYPE_FLOAT
   */
  unsigned int textureType;

  /**
   * Sphericle polynomial created for the dds texture
   */
  SphericalPolynomialPtr sphericalPolynomial;

}; // end of class DDSInfo

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_DDS_INFO_H
