#ifndef BABYLON_POSTPROCESS_FXAA_POST_PROCESS_H
#define BABYLON_POSTPROCESS_FXAA_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

class FxaaPostProcess;
using FxaaPostProcessPtr = std::shared_ptr<FxaaPostProcess>;

/**
 * @brief Fxaa post process.
 * @see https://doc.babylonjs.com/how_to/how_to_use_postprocesses#fxaa
 */
class BABYLON_SHARED_EXPORT FxaaPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static FxaaPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<FxaaPostProcess>(
      new FxaaPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~FxaaPostProcess();

protected:
  FxaaPostProcess(const std::string& _name, float ratio,
                  const CameraPtr& camera = nullptr,
                  unsigned int samplingMode
                  = TextureConstants::BILINEAR_SAMPLINGMODE,
                  Engine* engine = nullptr, bool reusable = false,
                  unsigned int textureType
                  = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

private:
  std::string _getDefines();

public:
  /**
   * Hidden
   */
  float texelWidth;

  /**
   * Hidden
   */
  float texelHeight;

}; // end of class FxaaPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_FXAA_POST_PROCESS_H
