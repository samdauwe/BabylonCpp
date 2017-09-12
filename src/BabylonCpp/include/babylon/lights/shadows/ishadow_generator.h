#ifndef BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H
#define BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct ShadowGeneratorCompileOptions {
  bool useInstances = false;
}; // end of struct ShadowGeneratorCompileOptions

/**
 * @brief Interface to implement to create a shadow generator compatible with
 * BJS.
 */
struct BABYLON_SHARED_EXPORT IShadowGenerator {
  virtual RenderTargetTexture* getShadowMap()                       = 0;
  virtual RenderTargetTexture* getShadowMapForRendering()           = 0;
  virtual bool isReady(SubMesh* subMesh, bool useInstances = false) = 0;
  virtual void prepareDefines(MaterialDefines& defines, unsigned int lightIndex)
    = 0;
  virtual void bindShadowLight(const std::string& lightIndex, Effect* effect)
    = 0;
  virtual Matrix getTransformMatrix()    = 0;
  virtual void recreateShadowMap()       = 0;
  virtual Json::object serialize() const = 0;
  virtual void forceCompilation(
    const ::std::function<void(ShadowGenerator* generator)>& onCompiled,
    const ShadowGeneratorCompileOptions& options)
    = 0;
  virtual void dispose() = 0;
}; // end of class IShadowGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_SHADOWS_ISHADOW_GENERATOR_H
