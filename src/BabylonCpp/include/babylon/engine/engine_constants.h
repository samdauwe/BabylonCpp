#ifndef BABYLON_ENGINE_ENGINE_CONSTANTS_H
#define BABYLON_ENGINE_ENGINE_CONSTANTS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT EngineConstants {

  // Alpha

  /**
   * Defines that alpha blending is disabled
   */
  static constexpr unsigned int ALPHA_DISABLE = 0;

  /**
   * Defines that alpha blending to SRC ALPHA * SRC + DEST
   */
  static constexpr unsigned int ALPHA_ADD = 1;

  /**
   * Defines that alpha blending to SRC ALPHA * SRC + (1 - SRC ALPHA) * DEST
   */
  static constexpr unsigned int ALPHA_COMBINE = 2;

  /**
   * Defines that alpha blending to DEST - SRC * DEST
   */
  static constexpr unsigned int ALPHA_SUBTRACT = 3;

  /**
   * Defines that alpha blending to SRC * DEST
   */
  static constexpr unsigned int ALPHA_MULTIPLY = 4;

  /**
   * Defines that alpha blending to SRC ALPHA * SRC + (1 - SRC) * DEST
   */
  static constexpr unsigned int ALPHA_MAXIMIZED = 5;

  /**
   * Defines that alpha blending to SRC + DEST
   */
  static constexpr unsigned int ALPHA_ONEONE = 6;

  /**
   * Defines that alpha blending to SRC + (1 - SRC ALPHA) * DEST
   */
  static constexpr unsigned int ALPHA_PREMULTIPLIED = 7;

  /**
   * Defines that alpha blending to SRC + (1 - SRC ALPHA) * DEST
   * Alpha will be set to (1 - SRC ALPHA) * DEST ALPHA
   */
  static constexpr unsigned int ALPHA_PREMULTIPLIED_PORTERDUFF = 8;

  /**
   * Defines that alpha blending to CST * SRC + (1 - CST) * DEST
   */
  static constexpr unsigned int ALPHA_INTERPOLATE = 9;

  /**
   * Defines that alpha blending to SRC + (1 - SRC) * DEST
   * Alpha will be set to SRC ALPHA + (1 - SRC ALPHA) * DEST ALPHA
   */
  static constexpr unsigned int ALPHA_SCREENMODE = 10;

  // Delays

  /**
   * Defines that the ressource is not delayed
   */
  static constexpr unsigned int DELAYLOADSTATE_NONE = 0;

  /**
   * Defines that the ressource was successfully delay loaded
   */
  static constexpr unsigned int DELAYLOADSTATE_LOADED = 1;

  /**
   * Defines that the ressource is currently delay loading
   */
  static constexpr unsigned int DELAYLOADSTATE_LOADING = 2;

  /**
   * Defines that the ressource is delayed and has not started loading
   */
  static constexpr unsigned int DELAYLOADSTATE_NOTLOADED = 4;

  // Depht or Stencil test Constants.

  /**
   * Depth or Stencil test Constants.
   * Passed to depthFunction or stencilFunction to specify depth or stencil
   * tests will never pass. i.e. Nothing will be drawn.
   */
  static constexpr unsigned int NEVER = 0x0200;

  /**
   * Passed to depthFunction or stencilFunction to specify depth or stencil
   * tests will always pass. i.e. Pixels will be drawn in the order they are
   * drawn.
   */
  static constexpr unsigned int ALWAYS = 0x0207;

  /**
   * Passed to depthFunction or stencilFunction to specify depth or stencil
   * tests will pass if the new depth value is less than the stored value.
   */
  static constexpr unsigned int LESS = 0x0201;

  /**
   * Passed to depthFunction or stencilFunction to specify depth or stencil
   * tests will pass if the new depth value is equals to the stored value.
   */
  static constexpr unsigned int EQUAL = 0x0202;

  /**
   * Passed to depthFunction or stencilFunction to specify depth or stencil
   * tests will pass if the new depth value is less than or equal to the stored
   * value.
   */
  static constexpr unsigned int LEQUAL = 0x0203;

  /**
   * Passed to depthFunction or stencilFunction to specify depth or stencil
   * tests will pass if the new depth value is greater than the stored value.
   */
  static constexpr unsigned int GREATER = 0x0204;

  /**
   * Passed to depthFunction or stencilFunction to specify depth or stencil
   * tests will pass if the new depth value is greater than or equal to the
   * stored value.
   */
  static constexpr unsigned int GEQUAL = 0x0206;

  /**
   * Passed to depthFunction or stencilFunction to specify depth or stencil
   * tests will pass if the new depth value is not equal to the stored value.
   */
  static constexpr unsigned int NOTEQUAL = 0x0205;

  // Stencil Actions Constants.

  /**
   * Passed to stencilOperation to specify that stencil value must be kept
   */
  static constexpr unsigned int KEEP = 0x1E00;

  /**
   * Passed to stencilOperation to specify that stencil value must be replaced
   */
  static constexpr unsigned int REPLACE = 0x1E01;

  /**
   * Passed to stencilOperation to specify that stencil value must be
   * incremented
   */
  static constexpr unsigned int INCR = 0x1E02;

  /**
   * Passed to stencilOperation to specify that stencil value must be
   * decremented
   */
  static constexpr unsigned int DECR = 0x1E03;

  /**
   * Passed to stencilOperation to specify that stencil value must be inverted
   */
  static constexpr unsigned int INVERT = 0x150A;

  /**
   * Passed to stencilOperation to specify that stencil value must be
   * incremented with wrapping
   */
  static constexpr unsigned int INCR_WRAP = 0x8507;

  /**
   * Passed to stencilOperation to specify that stencil value must be
   * decremented with wrapping
   */
  static constexpr unsigned int DECR_WRAP = 0x8508;

  /**
   * Texture is not repeating outside of 0..1 UVs
   */
  static constexpr unsigned int TEXTURE_CLAMP_ADDRESSMODE = 0;

  /**
   * Texture is repeating outside of 0..1 UVs
   */
  static constexpr unsigned int TEXTURE_WRAP_ADDRESSMODE = 1;

  /**
   * Texture is repeating and mirrored
   */
  static constexpr unsigned int TEXTURE_MIRROR_ADDRESSMODE = 2;

  /**
   * ALPHA
   */
  static constexpr unsigned int TEXTUREFORMAT_ALPHA = 0;

  /**
   * LUMINANCE
   */
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE = 1;

  /**
   * LUMINANCE_ALPHA
   */
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE_ALPHA = 2;

  /**
   * RGB
   */
  static constexpr unsigned int TEXTUREFORMAT_RGB = 4;

  /**
   * RGBA
   */
  static constexpr unsigned int TEXTUREFORMAT_RGBA = 5;

  /**
   * R
   */
  static constexpr unsigned int TEXTUREFORMAT_R = 6;

  /**
   * RG
   */
  static constexpr unsigned int TEXTUREFORMAT_RG = 7;

  /**
   * UNSIGNED_INT
   */
  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT = 0;

  /**
   * FLOAT
   */
  static constexpr unsigned int TEXTURETYPE_FLOAT = 1;

  /**
   * HALF_FLOAT
   */
  static constexpr unsigned int TEXTURETYPE_HALF_FLOAT = 2;

  /**
   * nearest is mag = nearest and min = nearest and mip = linear
   */
  static constexpr unsigned int TEXTURE_NEAREST_SAMPLINGMODE = 1;

  /**
   * Bilinear is mag = linear and min = linear and mip = nearest
   */
  static constexpr unsigned int TEXTURE_BILINEAR_SAMPLINGMODE = 2;

  /**
   * Trilinear is mag = linear and min = linear and mip = linear
   */
  static constexpr unsigned int TEXTURE_TRILINEAR_SAMPLINGMODE = 3;

  /**
   * nearest is mag = nearest and min = nearest and mip = linear
   */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST_MIPLINEAR = 1;

  /**
   * Bilinear is mag = linear and min = linear and mip = nearest
   */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR_MIPNEAREST = 2;

  /**
   * Trilinear is mag = linear and min = linear and mip = linear
   */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR_MIPLINEAR = 3;

  /**
   * mag = nearest and min = nearest and mip = nearest
   */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST_MIPNEAREST = 4;

  /**
   * mag = nearest and min = linear and mip = nearest
   */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR_MIPNEAREST = 5;

  /**
   * mag = nearest and min = linear and mip = linear
   */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR_MIPLINEAR = 6;

  /**
   * mag = nearest and min = linear and mip = none
   */
  static constexpr unsigned int TEXTURE_NEAREST_LINEAR = 7;

  /**
   * mag = nearest and min = nearest and mip = none
   */
  static constexpr unsigned int TEXTURE_NEAREST_NEAREST = 8;

  /**
   * mag = linear and min = nearest and mip = nearest
   */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST_MIPNEAREST = 9;

  /**
   * mag = linear and min = nearest and mip = linear
   */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST_MIPLINEAR = 10;

  /**
   * mag = linear and min = linear and mip = none
   */
  static constexpr unsigned int TEXTURE_LINEAR_LINEAR = 11;

  /**
   * mag = linear and min = nearest and mip = none
   */
  static constexpr unsigned int TEXTURE_LINEAR_NEAREST = 12;

  /**
   * Explicit coordinates mode
   */
  static constexpr unsigned int TEXTURE_EXPLICIT_MODE = 0;

  /**
   * Spherical coordinates mode
   */
  static constexpr unsigned int TEXTURE_SPHERICAL_MODE = 1;

  /**
   * Planar coordinates mode
   */
  static constexpr unsigned int TEXTURE_PLANAR_MODE = 2;

  /**
   * Cubic coordinates mode
   */
  static constexpr unsigned int TEXTURE_CUBIC_MODE = 3;

  /**
   * Projection coordinates mode
   */
  static constexpr unsigned int TEXTURE_PROJECTION_MODE = 4;

  /**
   * Skybox coordinates mode
   */
  static constexpr unsigned int TEXTURE_SKYBOX_MODE = 5;

  /**
   * Inverse Cubic coordinates mode
   */
  static constexpr unsigned int TEXTURE_INVCUBIC_MODE = 6;

  /**
   * Equirectangular coordinates mode
   */
  static constexpr unsigned int TEXTURE_EQUIRECTANGULAR_MODE = 7;

  /**
   * Equirectangular Fixed coordinates mode
   */
  static constexpr unsigned int TEXTURE_FIXED_EQUIRECTANGULAR_MODE = 8;

  /**
   * Equirectangular Fixed Mirrored coordinates mode
   */
  static constexpr unsigned int TEXTURE_FIXED_EQUIRECTANGULAR_MIRRORED_MODE = 9;

  // Texture rescaling mode

  /**
   * Defines that texture rescaling will use a floor to find the closer power of
   * 2 size
   */
  static constexpr unsigned int SCALEMODE_FLOOR = 1;

  /**
   * Defines that texture rescaling will look for the nearest power of 2 size
   */
  static constexpr unsigned int SCALEMODE_NEAREST = 2;

  /**
   * Defines that texture rescaling will use a ceil to find the closer power of
   * 2 size
   */
  static constexpr unsigned int SCALEMODE_CEILING = 3;

}; // end of struct EngineConstants

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ENGINE_CONSTANTS_H
