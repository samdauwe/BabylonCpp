#ifndef BABYLON_MATERIALS_TEXTURES_IRENDER_TARGET_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_IRENDER_TARGET_OPTIONS_H

#include <babylon/babylon_api.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/texture_constants.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IRenderTargetOptions {
  bool generateMipMaps       = false;
  bool generateDepthBuffer   = true;
  bool generateStencilBuffer = false;
  unsigned int type          = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
  unsigned int format        = EngineConstants::TEXTUREFORMAT_RGBA;
  unsigned int samplingMode  = TextureConstants::TRILINEAR_SAMPLINGMODE;
}; // end of struct RenderTargetOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IRENDER_TARGET_OPTIONS_H
