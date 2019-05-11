#ifndef BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector2.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

/**
 * @brief StereoscopicInterlacePostProcess used to render stereo views from a
 * rigged camera.
 */
class BABYLON_SHARED_EXPORT StereoscopicInterlacePostProcess
    : public PostProcess {

public:
  /**
   * @brief Initializes a StereoscopicInterlacePostProcess.
   * @param name The name of the effect.
   * @param rigCameras The rig cameras to be appled to the post process
   * @param isStereoscopicHoriz If the rendered results are horizontal or
   * verticle
   * @param samplingMode The sampling mode to be used when computing the pass.
   * (default: 0)
   * @param engine The engine which the post process will be applied. (default:
   * current engine)
   * @param reusable If the post process can be reused on the same frame.
   * (default: false)
   */
  StereoscopicInterlacePostProcess(const std::string& name,
                                   const std::vector<CameraPtr>& rigCameras,
                                   bool isStereoscopicHoriz,
                                   unsigned int samplingMode, Engine* engine,
                                   bool reusable = false);
  virtual ~StereoscopicInterlacePostProcess();

private:
  Vector2 _stepSize;
  PostProcess* _passedProcess;

}; // end of class StereoscopicInterlacePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_STEREOSCOPIC_INTERLACE_POST_PROCESS_H
