#ifndef BABYLON_MATERIALS_TEXTURES_TEXTURE_CONSTANTS_H
#define BABYLON_MATERIALS_TEXTURES_TEXTURE_CONSTANTS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TextureConstants {

  static constexpr unsigned int NEAREST_SAMPLINGMODE      = 1;
  static constexpr unsigned int NEAREST_NEAREST_MIPLINEAR = 1;

  static constexpr unsigned int BILINEAR_SAMPLINGMODE    = 2;
  static constexpr unsigned int LINEAR_LINEAR_MIPNEAREST = 2;

  static constexpr unsigned int TRILINEAR_SAMPLINGMODE  = 3;
  static constexpr unsigned int LINEAR_LINEAR_MIPLINEAR = 3;

  static constexpr unsigned int NEAREST_NEAREST_MIPNEAREST = 4;
  static constexpr unsigned int NEAREST_LINEAR_MIPNEAREST  = 5;
  static constexpr unsigned int NEAREST_LINEAR_MIPLINEAR   = 6;
  static constexpr unsigned int NEAREST_LINEAR             = 7;
  static constexpr unsigned int NEAREST_NEAREST            = 8;
  static constexpr unsigned int LINEAR_NEAREST_MIPNEAREST  = 9;
  static constexpr unsigned int LINEAR_NEAREST_MIPLINEAR   = 10;
  static constexpr unsigned int LINEAR_LINEAR              = 11;
  static constexpr unsigned int LINEAR_NEAREST             = 12;

  static constexpr unsigned int EXPLICIT_MODE                       = 0;
  static constexpr unsigned int SPHERICAL_MODE                      = 1;
  static constexpr unsigned int PLANAR_MODE                         = 2;
  static constexpr unsigned int CUBIC_MODE                          = 3;
  static constexpr unsigned int PROJECTION_MODE                     = 4;
  static constexpr unsigned int SKYBOX_MODE                         = 5;
  static constexpr unsigned int INVCUBIC_MODE                       = 6;
  static constexpr unsigned int EQUIRECTANGULAR_MODE                = 7;
  static constexpr unsigned int FIXED_EQUIRECTANGULAR_MODE          = 8;
  static constexpr unsigned int FIXED_EQUIRECTANGULAR_MIRRORED_MODE = 9;

  static constexpr unsigned int CLAMP_ADDRESSMODE  = 0;
  static constexpr unsigned int WRAP_ADDRESSMODE   = 1;
  static constexpr unsigned int MIRROR_ADDRESSMODE = 2;

}; // end of struct TextureConstants

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_TEXTURE_CONSTANTS_H
