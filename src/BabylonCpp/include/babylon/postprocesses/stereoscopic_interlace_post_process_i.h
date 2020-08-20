#ifndef BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_I_H
#define BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_I_H

#include <babylon/babylon_api.h>
#include <babylon/maths/vector2.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class StereoscopicInterlacePostProcessI;
using StereoscopicInterlacePostProcessIPtr = std::shared_ptr<StereoscopicInterlacePostProcessI>;

/**
 * @brief StereoscopicInterlacePostProcessI used to render stereo views from a rigged camera with
 * support for alternate line interlacing.
 */
class BABYLON_SHARED_EXPORT StereoscopicInterlacePostProcessI : public PostProcess {

public:
  template <typename... Ts>
  static StereoscopicInterlacePostProcessIPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<StereoscopicInterlacePostProcessI>(
      new StereoscopicInterlacePostProcessI(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~StereoscopicInterlacePostProcessI() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "StereoscopicInterlacePostProcessI" string
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Initializes a StereoscopicInterlacePostProcessI.
   * @param name The name of the effect.
   * @param rigCameras The rig cameras to be appled to the post process
   * @param isStereoscopicHoriz If the rendered results are horizontal or vertical
   * @param isStereoscopicInterlaced If the rendered results are alternate line interlaced
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   */
  StereoscopicInterlacePostProcessI(const std::string& name,
                                    const std::vector<CameraPtr>& rigCameras,
                                    bool isStereoscopicHoriz, bool isStereoscopicInterlaced,
                                    const std::optional<unsigned int>& samplingMode = std::nullopt,
                                    Engine* engine = nullptr, bool reusable = false);

private:
  Vector2 _stepSize;
  PostProcessPtr _passedProcess;

}; // end of class StereoscopicInterlacePostProcessI

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_I_H
