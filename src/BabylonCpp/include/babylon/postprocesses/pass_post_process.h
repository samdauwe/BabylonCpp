#ifndef BABYLON_POSTPROCESSES_PASS_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_PASS_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine_constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class PassPostProcess;
using PassPostProcessPtr = std::shared_ptr<PassPostProcess>;

/**
 * @brief PassPostProcess which produces an output the same as it's input.
 */
class BABYLON_SHARED_EXPORT PassPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static PassPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<PassPostProcess>(
      new PassPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~PassPostProcess();

protected:
  /**
   * @brief Creates the PassPostProcess.
   * @param name The name of the effect.
   * @param options The required width/height ratio to downsize to before
   * computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass.
   * (default: 0)
   * @param engine The engine which the post process will be applied. (default:
   * current engine)
   * @param reusable If the post process can be reused on the same frame.
   * (default: false)
   * @param textureType The type of texture to be used when performing the post
   * processing.
   * @param blockCompilation If compilation of the shader should not be done in
   * the constructor. The updateEffect method can be used to compile the shader
   * at a later time. (default: false)
   */
  PassPostProcess(const std::string& name, float ratio, const CameraPtr& camera,
                  unsigned int samplingMode, Engine* engine,
                  bool reusable = false,
                  unsigned int textureType
                  = Constants::TEXTURETYPE_UNSIGNED_INT,
                  bool blockCompilation = false);

}; // end of class PassPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_PASS_POST_PROCESS_H
