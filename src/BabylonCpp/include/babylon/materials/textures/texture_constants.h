#ifndef BABYLON_MATERIALS_TEXTURES_TEXTURE_CONSTANTS_H
#define BABYLON_MATERIALS_TEXTURES_TEXTURE_CONSTANTS_H

#include <babylon/babylon_api.h>
#include <babylon/engines/engine_constants.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TextureConstants {

  /** nearest is mag = nearest and min = nearest and mip = linear */
  static constexpr unsigned int NEAREST_SAMPLINGMODE
    = EngineConstants::TEXTURE_NEAREST_SAMPLINGMODE;
  /** nearest is mag = nearest and min = nearest and mip = linear */
  static constexpr unsigned int NEAREST_NEAREST_MIPLINEAR
    = EngineConstants::TEXTURE_NEAREST_NEAREST_MIPLINEAR;

  /** Bilinear is mag = linear and min = linear and mip = nearest */
  static constexpr unsigned int BILINEAR_SAMPLINGMODE
    = EngineConstants::TEXTURE_BILINEAR_SAMPLINGMODE;
  /** Bilinear is mag = linear and min = linear and mip = nearest */
  static constexpr unsigned int LINEAR_LINEAR_MIPNEAREST
    = EngineConstants::TEXTURE_LINEAR_LINEAR_MIPNEAREST;

  /** Trilinear is mag = linear and min = linear and mip = linear */
  static constexpr unsigned int TRILINEAR_SAMPLINGMODE
    = EngineConstants::TEXTURE_TRILINEAR_SAMPLINGMODE;
  /** Trilinear is mag = linear and min = linear and mip = linear */
  static constexpr unsigned int LINEAR_LINEAR_MIPLINEAR
    = EngineConstants::TEXTURE_LINEAR_LINEAR_MIPLINEAR;

  /** mag = nearest and min = nearest and mip = nearest */
  static constexpr unsigned int NEAREST_NEAREST_MIPNEAREST
    = EngineConstants::TEXTURE_NEAREST_NEAREST_MIPNEAREST;
  /** mag = nearest and min = linear and mip = nearest */
  static constexpr unsigned int NEAREST_LINEAR_MIPNEAREST
    = EngineConstants::TEXTURE_NEAREST_LINEAR_MIPNEAREST;
  /** mag = nearest and min = linear and mip = linear */
  static constexpr unsigned int NEAREST_LINEAR_MIPLINEAR
    = EngineConstants::TEXTURE_NEAREST_LINEAR_MIPLINEAR;
  /** mag = nearest and min = linear and mip = none */
  static constexpr unsigned int NEAREST_LINEAR
    = EngineConstants::TEXTURE_NEAREST_LINEAR;
  /** mag = nearest and min = nearest and mip = none */
  static constexpr unsigned int NEAREST_NEAREST
    = EngineConstants::TEXTURE_NEAREST_NEAREST;
  /** mag = linear and min = nearest and mip = nearest */
  static constexpr unsigned int LINEAR_NEAREST_MIPNEAREST
    = EngineConstants::TEXTURE_LINEAR_NEAREST_MIPNEAREST;
  /** mag = linear and min = nearest and mip = linear */
  static constexpr unsigned int LINEAR_NEAREST_MIPLINEAR
    = EngineConstants::TEXTURE_LINEAR_NEAREST_MIPLINEAR;
  /** mag = linear and min = linear and mip = none */
  static constexpr unsigned int LINEAR_LINEAR
    = EngineConstants::TEXTURE_LINEAR_LINEAR;
  /** mag = linear and min = nearest and mip = none */
  static constexpr unsigned int LINEAR_NEAREST
    = EngineConstants::TEXTURE_LINEAR_NEAREST;

  /** Explicit coordinates mode */
  static constexpr unsigned int EXPLICIT_MODE
    = EngineConstants::TEXTURE_EXPLICIT_MODE;
  /** Spherical coordinates mode */
  static constexpr unsigned int SPHERICAL_MODE
    = EngineConstants::TEXTURE_SPHERICAL_MODE;
  /** Planar coordinates mode */
  static constexpr unsigned int PLANAR_MODE
    = EngineConstants::TEXTURE_PLANAR_MODE;
  /** Cubic coordinates mode */
  static constexpr unsigned int CUBIC_MODE
    = EngineConstants::TEXTURE_CUBIC_MODE;
  /** Projection coordinates mode */
  static constexpr unsigned int PROJECTION_MODE
    = EngineConstants::TEXTURE_PROJECTION_MODE;
  /** Inverse Cubic coordinates mode */
  static constexpr unsigned int SKYBOX_MODE
    = EngineConstants::TEXTURE_SKYBOX_MODE;
  /** Inverse Cubic coordinates mode */
  static constexpr unsigned int INVCUBIC_MODE
    = EngineConstants::TEXTURE_INVCUBIC_MODE;
  /** Equirectangular coordinates mode */
  static constexpr unsigned int EQUIRECTANGULAR_MODE
    = EngineConstants::TEXTURE_EQUIRECTANGULAR_MODE;
  /** Equirectangular Fixed coordinates mode */
  static constexpr unsigned int FIXED_EQUIRECTANGULAR_MODE
    = EngineConstants::TEXTURE_FIXED_EQUIRECTANGULAR_MODE;
  /** Equirectangular Fixed Mirrored coordinates mode */
  static constexpr unsigned int FIXED_EQUIRECTANGULAR_MIRRORED_MODE
    = EngineConstants::TEXTURE_FIXED_EQUIRECTANGULAR_MIRRORED_MODE;

  /** Texture is not repeating outside of 0..1 UVs */
  static constexpr unsigned int CLAMP_ADDRESSMODE
    = EngineConstants::TEXTURE_CLAMP_ADDRESSMODE;
  /** Texture is repeating outside of 0..1 UVs */
  static constexpr unsigned int WRAP_ADDRESSMODE
    = EngineConstants::TEXTURE_WRAP_ADDRESSMODE;
  /** Texture is repeating and mirrored */
  static constexpr unsigned int MIRROR_ADDRESSMODE
    = EngineConstants::TEXTURE_MIRROR_ADDRESSMODE;

}; // end of struct TextureConstants

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_TEXTURE_CONSTANTS_H
