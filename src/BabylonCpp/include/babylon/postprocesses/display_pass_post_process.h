#ifndef BABYLON_POSTPROCESSES_DISPLAY_PASS_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_DISPLAY_PASS_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

FWD_CLASS_SPTR(DisplayPassPostProcess)

/**
 * @brief DisplayPassPostProcess which produces an output the same as it's input.
 */
class BABYLON_SHARED_EXPORT DisplayPassPostProcess : public PostProcess {

public:
  /**
   * @brief Creates the DisplayPassPostProcess.
   * @param name The name of the effect.
   * @param options The required width/height ratio to downsize to before computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   */
  DisplayPassPostProcess(const std::string& name,
                         const std::variant<float, PostProcessOptions>& options,
                         const CameraPtr& camera,
                         const std::optional<unsigned int>& samplingMode = std::nullopt,
                         Engine* engine = nullptr, bool reusable = false);
  ~DisplayPassPostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "DisplayPassPostProcess" string
   */
  std::string getClassName() const override;

  /**
   * @brief Hidden
   */
  static DisplayPassPostProcessPtr _Parse(const json& parsedPostProcess,
                                          const CameraPtr& targetCamera, Scene* scene,
                                          const std::string& rootUrl);

}; // end of class DisplayPassPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_DISPLAY_PASS_POST_PROCESS_H
