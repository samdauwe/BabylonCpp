#include <babylon/materials/iimage_processing_configuration_defines.h>

#include <sstream>

namespace BABYLON {

std::ostream& operator<<(std::ostream& os,
                         const IImageProcessingConfigurationDefines& ipcd)
{
  if (ipcd.IMAGEPROCESSING) {
    os << "#define IMAGEPROCESSING\n";
  }
  if (ipcd.VIGNETTE) {
    os << "#define VIGNETTE\n";
  }
  if (ipcd.VIGNETTEBLENDMODEMULTIPLY) {
    os << "#define VIGNETTEBLENDMODEMULTIPLY\n";
  }
  if (ipcd.VIGNETTEBLENDMODEOPAQUE) {
    os << "#define VIGNETTEBLENDMODEOPAQUE\n";
  }
  if (ipcd.TONEMAPPING) {
    os << "#define TONEMAPPING\n";
  }
  if (ipcd.TONEMAPPING_ACES) {
    os << "#define TONEMAPPING_ACES\n";
  }
  if (ipcd.CONTRAST) {
    os << "#define CONTRAST\n";
  }
  if (ipcd.EXPOSURE) {
    os << "#define EXPOSURE\n";
  }
  if (ipcd.COLORCURVES) {
    os << "#define COLORCURVES\n";
  }
  if (ipcd.COLORGRADING) {
    os << "#define COLORGRADING\n";
  }
  if (ipcd.COLORGRADING3D) {
    os << "#define COLORGRADING3D\n";
  }
  if (ipcd.FROMLINEARSPACE) {
    os << "#define FROMLINEARSPACE\n";
  }
  if (ipcd.SAMPLER3DGREENDEPTH) {
    os << "#define SAMPLER3DGREENDEPTH\n";
  }
  if (ipcd.SAMPLER3DBGRMAP) {
    os << "#define SAMPLER3DBGRMAP\n";
  }
  if (ipcd.IMAGEPROCESSINGPOSTPROCESS) {
    os << "#define IMAGEPROCESSINGPOSTPROCESS\n";
  }

  return os;
}

std::string IImageProcessingConfigurationDefines::convertToString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

} // end of namespace BABYLON
