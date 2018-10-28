#ifndef BABYLON_TOOLS_DDS_INFO_H
#define BABYLON_TOOLS_DDS_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/math/spherical_polynomial.h>

namespace BABYLON {

using SphericalPolynomialPtr = std::shared_ptr<SphericalPolynomial>;

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
  ~DDSInfo();

public:
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
  SphericalPolynomialPtr sphericalPolynomial;
}; // end of class DDSInfo

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_DDS_INFO_H
