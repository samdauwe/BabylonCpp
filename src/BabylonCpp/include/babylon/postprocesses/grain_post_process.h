#ifndef BABYLON_POSTPROCESSES_GRAIN_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_GRAIN_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

FWD_CLASS_SPTR(GrainPostProcess)

/**
 * @brief The GrainPostProcess adds noise to the image at mid luminance levels.
 */
class BABYLON_SHARED_EXPORT GrainPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static GrainPostProcessPtr New(Ts&&... args)
  {
    auto postProcess
      = std::shared_ptr<GrainPostProcess>(new GrainPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~GrainPostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "GrainPostProcess" string
   */
  std::string getClassName() const override;

  /**
   * @brief Hidden
   */
  static GrainPostProcessPtr _Parse(const json& parsedPostProcess, const CameraPtr& targetCamera,
                                    Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new instance of @see GrainPostProcess
   * @param name The name of the effect.
   * @param options The required width/height ratio to downsize to before computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   * @param textureType Type of textures used when performing the post process. (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in the constructor. The
   * updateEffect method can be used to compile the shader at a later time. (default: false)
   */
  GrainPostProcess(const std::string& name, const std::variant<float, PostProcessOptions>& options,
                   const CameraPtr& camera,
                   const std::optional<unsigned int>& samplingMode = std::nullopt,
                   Engine* engine = nullptr, bool reusable = false,
                   unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                   bool blockCompilation    = false);

public:
  /**
   * The intensity of the grain added (default: 30)
   */
  float intensity;

  /**
   * If the grain should be randomized on every frame
   */
  bool animated;

}; // end of class GrainPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_GRAIN_POST_PROCESS_H
