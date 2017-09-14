#ifndef BABYLON_POSTPROCESS_FXAA_POST_PROCESS_H
#define BABYLON_POSTPROCESS_FXAA_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT FxaaPostProcess : public PostProcess {

public:
  FxaaPostProcess(const string_t& _name, float ratio, Camera* camera,
                  unsigned int samplingMode
                  = TextureConstants::BILINEAR_SAMPLINGMODE,
                  Engine* engine = nullptr, bool reusable = false,
                  unsigned int textureType
                  = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  ~FxaaPostProcess();

public:
  float texelWidth;
  float texelHeight;

}; // end of class FxaaPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_FXAA_POST_PROCESS_H
