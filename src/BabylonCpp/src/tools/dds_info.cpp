#include <babylon/tools/dds_info.h>

namespace BABYLON {

DDSInfo::DDSInfo()
    : width{0}
    , height{0}
    , mipmapCount{0}
    , isFourCC{false}
    , isRGB{false}
    , isLuminance{false}
    , isCube{false}
    , isCompressed{false}
    , dxgiFormat{0}
    , textureType{0}
    , sphericalPolynomial{nullptr}
{
}

DDSInfo::DDSInfo(int iWidth, int iHeight, int iMipmapCount, bool iIsFourCC,
                 bool iIsRGB, bool iIsLuminance, bool iIsCube,
                 bool iIsCompressed, int iDxgiFormat, unsigned int iTextureType,
                 SphericalPolynomialPtr iSphericalPolynomial)
    : width{iWidth}
    , height{iHeight}
    , mipmapCount{iMipmapCount}
    , isFourCC{iIsFourCC}
    , isRGB{iIsRGB}
    , isLuminance{iIsLuminance}
    , isCube{iIsCube}
    , isCompressed{iIsCompressed}
    , dxgiFormat{iDxgiFormat}
    , textureType{iTextureType}
    , sphericalPolynomial{iSphericalPolynomial}
{
}

DDSInfo::DDSInfo(const DDSInfo& other)
    : width{other.width}
    , height{other.height}
    , mipmapCount{other.mipmapCount}
    , isFourCC{other.isFourCC}
    , isRGB{other.isRGB}
    , isLuminance{other.isLuminance}
    , isCube{other.isCube}
    , isCompressed{other.isCompressed}
    , dxgiFormat{other.dxgiFormat}
    , textureType{other.textureType}
    , sphericalPolynomial{other.sphericalPolynomial}
{
}

DDSInfo::DDSInfo(DDSInfo&& other)
    : width{std::move(other.width)}
    , height{std::move(other.height)}
    , mipmapCount{std::move(other.mipmapCount)}
    , isFourCC{std::move(other.isFourCC)}
    , isRGB{std::move(other.isRGB)}
    , isLuminance{std::move(other.isLuminance)}
    , isCube{std::move(other.isCube)}
    , isCompressed{std::move(other.isCompressed)}
    , dxgiFormat{std::move(other.dxgiFormat)}
    , textureType{std::move(other.textureType)}
    , sphericalPolynomial{std::move(other.sphericalPolynomial)}
{
}

DDSInfo& DDSInfo::operator=(const DDSInfo& other)
{
  if (&other != this) {
    width               = other.width;
    height              = other.height;
    mipmapCount         = other.mipmapCount;
    isFourCC            = other.isFourCC;
    isRGB               = other.isRGB;
    isLuminance         = other.isLuminance;
    isCube              = other.isCube;
    isCompressed        = other.isCompressed;
    dxgiFormat          = other.dxgiFormat;
    textureType         = other.textureType;
    sphericalPolynomial = other.sphericalPolynomial;
  }

  return *this;
}

DDSInfo& DDSInfo::operator=(DDSInfo&& other)
{
  if (&other != this) {
    width               = std::move(other.width);
    height              = std::move(other.height);
    mipmapCount         = std::move(other.mipmapCount);
    isFourCC            = std::move(other.isFourCC);
    isRGB               = std::move(other.isRGB);
    isLuminance         = std::move(other.isLuminance);
    isCube              = std::move(other.isCube);
    isCompressed        = std::move(other.isCompressed);
    dxgiFormat          = std::move(other.dxgiFormat);
    textureType         = std::move(other.textureType);
    sphericalPolynomial = std::move(other.sphericalPolynomial);
  }

  return *this;
}

DDSInfo::~DDSInfo()
{
}

} // end of namespace BABYLON
