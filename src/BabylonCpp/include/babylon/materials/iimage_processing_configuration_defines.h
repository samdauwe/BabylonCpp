#ifndef BABYLON_MATERIALS_IIMAGE_PROCESSING_CONFIGURATION_DEFINES_H
#define BABYLON_MATERIALS_IIMAGE_PROCESSING_CONFIGURATION_DEFINES_H

#include <ostream>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Interface to follow in your material defines to integrate easily the Image proccessing
 * functions.
 */
struct BABYLON_SHARED_EXPORT IImageProcessingConfigurationDefines {
  bool IMAGEPROCESSING            = false;
  bool VIGNETTE                   = false;
  bool VIGNETTEBLENDMODEMULTIPLY  = false;
  bool VIGNETTEBLENDMODEOPAQUE    = false;
  bool TONEMAPPING                = false;
  bool TONEMAPPING_ACES           = false;
  bool CONTRAST                   = false;
  bool EXPOSURE                   = false;
  bool COLORCURVES                = false;
  bool COLORGRADING               = false;
  bool COLORGRADING3D             = false;
  bool FROMLINEARSPACE            = false;
  bool SAMPLER3DGREENDEPTH        = false;
  bool SAMPLER3DBGRMAP            = false;
  bool IMAGEPROCESSINGPOSTPROCESS = false;

  friend std::ostream& operator<<(std::ostream& os,
                                  const IImageProcessingConfigurationDefines& ipcd);

  /**
   * @brief Converts the material define values to a string.
   * @returns String of material define information.
   */
  [[nodiscard]] std::string convertToString() const;

}; // end of struct IImageProcessingConfigurationDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IIMAGE_PROCESSING_CONFIGURATION_DEFINES_H
