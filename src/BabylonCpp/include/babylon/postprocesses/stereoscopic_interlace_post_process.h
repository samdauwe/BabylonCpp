#ifndef BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/vector2.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

FWD_CLASS_SPTR(StereoscopicInterlacePostProcess)

/**
 * @brief StereoscopicInterlacePostProcess used to render stereo views from a rigged camera.
 */
class BABYLON_SHARED_EXPORT StereoscopicInterlacePostProcess : public PostProcess {

public:
  template <typename... Ts>
  static StereoscopicInterlacePostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<StereoscopicInterlacePostProcess>(
      new StereoscopicInterlacePostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~StereoscopicInterlacePostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "StereoscopicInterlacePostProcess" string
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Initializes a StereoscopicInterlacePostProcess.
   * @param name The name of the effect.
   * @param rigCameras The rig cameras to be applied to the post process
   * @param isStereoscopicHoriz If the rendered results are horizontal or vertical
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   */
  StereoscopicInterlacePostProcess(const std::string& name,
                                   const std::vector<CameraPtr>& rigCameras,
                                   bool isStereoscopicHoriz,
                                   const std::optional<unsigned int>& samplingMode = std::nullopt,
                                   Engine* engine = nullptr, bool reusable = false);

private:
  Vector2 _stepSize;
  PostProcessPtr _passedProcess;

}; // end of class StereoscopicInterlacePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_H
