#ifndef BABYLON_POSTPROCESSES_BLACK_AND_WHITE_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_BLACK_AND_WHITE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class BlackAndWhitePostProcess;
using BlackAndWhitePostProcessPtr = std::shared_ptr<BlackAndWhitePostProcess>;

/**
 * @brief Post process used to render in black and white.
 */
class BABYLON_SHARED_EXPORT BlackAndWhitePostProcess : public PostProcess {

public:
  template <typename... Ts>
  static BlackAndWhitePostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<BlackAndWhitePostProcess>(
      new BlackAndWhitePostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~BlackAndWhitePostProcess() override; // = default

protected:
  /**
   * @brief Creates a black and white post process.
   * @see
   * https://doc.babylonjs.com/how_to/how_to_use_postprocesses#black-and-white
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
   */
  BlackAndWhitePostProcess(const std::string& name, float ratio, const CameraPtr& camera,
                           const std::optional<unsigned int>& samplingMode = std::nullopt,
                           Engine* engine = nullptr, bool reusable = false);

public:
  /**
   * Linear about to convert he result to black and white (default: 1)
   */
  float degree;

}; // end of class BlackAndWhitePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_BLACK_AND_WHITE_POST_PROCESS_H
