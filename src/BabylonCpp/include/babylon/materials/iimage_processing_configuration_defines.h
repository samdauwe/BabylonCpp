#ifndef BABYLON_MATERIALS_IIMAGE_PROCESSING_CONFIGURATION_DEFINES_H
#define BABYLON_MATERIALS_IIMAGE_PROCESSING_CONFIGURATION_DEFINES_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Interface to follow in your material defines to integrate easily the
 * Image proccessing functions.
 */
struct BABYLON_SHARED_EXPORT IImageProcessingConfigurationDefines {
  bool IMAGEPROCESSING;
  bool VIGNETTE;
  bool VIGNETTEBLENDMODEMULTIPLY;
  bool VIGNETTEBLENDMODEOPAQUE;
  bool TONEMAPPING;
  bool CONTRAST;
  bool EXPOSURE;
  bool COLORCURVES;
  bool COLORGRADING;
  bool SAMPLER3DGREENDEPTH;
  bool SAMPLER3DBGRMAP;
  bool IMAGEPROCESSINGPOSTPROCESS;
}; // end of struct IImageProcessingConfigurationDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IIMAGE_PROCESSING_CONFIGURATION_DEFINES_H
