#include <babylon/misc/dds_info.h>

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

DDSInfo::DDSInfo(int iWidth, int iHeight, int iMipmapCount, bool iIsFourCC, bool iIsRGB,
                 bool iIsLuminance, bool iIsCube, bool iIsCompressed, int iDxgiFormat,
                 unsigned int iTextureType, SphericalPolynomialPtr iSphericalPolynomial)
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

DDSInfo::DDSInfo(const DDSInfo& other) = default;
DDSInfo::DDSInfo(DDSInfo&& other)      = default;
DDSInfo& DDSInfo::operator=(const DDSInfo& other) = default;
DDSInfo& DDSInfo::operator=(DDSInfo&& other) = default;

DDSInfo::~DDSInfo() = default;

} // end of namespace BABYLON
