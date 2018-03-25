#ifndef BABYLON_ENGINE_ENGINE_CONSTANTS_H
#define BABYLON_ENGINE_ENGINE_CONSTANTS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT EngineConstants {

  // Const statics
  static constexpr unsigned int ALPHA_DISABLE                  = 0;
  static constexpr unsigned int ALPHA_ADD                      = 1;
  static constexpr unsigned int ALPHA_COMBINE                  = 2;
  static constexpr unsigned int ALPHA_SUBTRACT                 = 3;
  static constexpr unsigned int ALPHA_MULTIPLY                 = 4;
  static constexpr unsigned int ALPHA_MAXIMIZED                = 5;
  static constexpr unsigned int ALPHA_ONEONE                   = 6;
  static constexpr unsigned int ALPHA_PREMULTIPLIED            = 7;
  static constexpr unsigned int ALPHA_PREMULTIPLIED_PORTERDUFF = 8;
  static constexpr unsigned int ALPHA_INTERPOLATE              = 9;
  static constexpr unsigned int ALPHA_SCREENMODE               = 10;

  static constexpr unsigned int DELAYLOADSTATE_NONE      = 0;
  static constexpr unsigned int DELAYLOADSTATE_LOADED    = 1;
  static constexpr unsigned int DELAYLOADSTATE_LOADING   = 2;
  static constexpr unsigned int DELAYLOADSTATE_NOTLOADED = 4;

  static constexpr unsigned int TEXTUREFORMAT_ALPHA           = 0;
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE       = 1;
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE_ALPHA = 2;
  static constexpr unsigned int TEXTUREFORMAT_RGB             = 4;
  static constexpr unsigned int TEXTUREFORMAT_RGBA            = 5;
  static constexpr unsigned int TEXTUREFORMAT_R32F            = 6;
  static constexpr unsigned int TEXTUREFORMAT_RG32F           = 7;
  static constexpr unsigned int TEXTUREFORMAT_RGB32F          = 8;
  static constexpr unsigned int TEXTUREFORMAT_RGBA32F         = 9;

  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT = 0;
  static constexpr unsigned int TEXTURETYPE_FLOAT        = 1;
  static constexpr unsigned int TEXTURETYPE_HALF_FLOAT   = 2;

  // Depht or Stencil test Constants.
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will never pass. i.e. Nothing will be drawn.
  static constexpr unsigned int NEVER = 0x0200;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will always pass. i.e. Pixels will be drawn in the order they are
  // drawn.
  static constexpr unsigned int ALWAYS = 0x0207;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is less than the stored value.
  static constexpr unsigned int LESS = 0x0201;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is equals to the stored value.
  static constexpr unsigned int EQUAL = 0x0202;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is less than or equal to the stored
  // value.
  static constexpr unsigned int LEQUAL = 0x0203;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is greater than the stored value.
  static constexpr unsigned int GREATER = 0x0204;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is greater than or equal to the
  // stored value.
  static constexpr unsigned int GEQUAL = 0x0206;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is not equal to the stored value.
  static constexpr unsigned int NOTEQUAL = 0x0205;

  // Stencil Actions Constants.
  static constexpr unsigned int KEEP      = 0x1E00;
  static constexpr unsigned int REPLACE   = 0x1E01;
  static constexpr unsigned int INCR      = 0x1E02;
  static constexpr unsigned int DECR      = 0x1E03;
  static constexpr unsigned int INVERT    = 0x150A;
  static constexpr unsigned int INCR_WRAP = 0x8507;
  static constexpr unsigned int DECR_WRAP = 0x8508;

  // Texture rescaling mode
  static constexpr unsigned int SCALEMODE_FLOOR   = 1;
  static constexpr unsigned int SCALEMODE_NEAREST = 2;
  static constexpr unsigned int SCALEMODE_CEILING = 3;

}; // end of struct EngineConstants

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ENGINE_CONSTANTS_H
