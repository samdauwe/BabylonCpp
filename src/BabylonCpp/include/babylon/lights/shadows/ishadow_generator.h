#ifndef BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IShadowGenerator {
  virtual RenderTargetTexture* getShadowMap() = 0;
  virtual bool isReady(SubMesh* subMesh, bool useInstances = false) = 0;
  virtual void dispose() = 0;
}; // end of class IShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H
