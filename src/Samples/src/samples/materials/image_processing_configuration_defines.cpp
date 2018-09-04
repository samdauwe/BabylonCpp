#include <babylon/materials/image_processing_configuration_defines.h>

namespace BABYLON {

ImageProcessingConfigurationDefines::ImageProcessingConfigurationDefines()
    : MaterialDefines{}
{
  IMAGEPROCESSING            = false;
  VIGNETTE                   = false;
  VIGNETTEBLENDMODEMULTIPLY  = false;
  VIGNETTEBLENDMODEOPAQUE    = false;
  TONEMAPPING                = false;
  TONEMAPPING_ACES           = false;
  CONTRAST                   = false;
  COLORCURVES                = false;
  COLORGRADING               = false;
  COLORGRADING3D             = false;
  SAMPLER3DGREENDEPTH        = false;
  SAMPLER3DBGRMAP            = false;
  IMAGEPROCESSINGPOSTPROCESS = false;
  EXPOSURE                   = false;
}

ImageProcessingConfigurationDefines::~ImageProcessingConfigurationDefines()
{
}

} // end of namespace BABYLON
