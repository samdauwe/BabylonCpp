#ifndef BABYLON_POSTPROCESS_STEREOSCOPIC_INTERLACE_POST_PROCESS_H
#define BABYLON_POSTPROCESS_STEREOSCOPIC_INTERLACE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector2.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT StereoscopicInterlacePostProcess
    : public PostProcess {

public:
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

#endif // end of BABYLON_POSTPROCESS_STEREOSCOPIC_INTERLACE_POST_PROCESS_H
