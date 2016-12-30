#ifndef BABYLON_POSTPROCESS_STEREOSCOPIC_INTERLACE_POST_PROCESS_H
#define BABYLON_POSTPROCESS_STEREOSCOPIC_INTERLACE_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/math/vector2.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT StereoscopicInterlacePostProcess
  : public PostProcess {

public:
  StereoscopicInterlacePostProcess(const std::string& name,
                                   std::vector<Camera*> rigCameras,
                                   bool isStereoscopicHoriz,
                                   unsigned int samplingMode, Engine* engine,
                                   bool reusable = false);
  virtual ~StereoscopicInterlacePostProcess();

public:
  int width;
  int height;

private:
  Vector2 _stepSize;
  PostProcess* _passedProcess;

}; // end of class StereoscopicInterlacePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_STEREOSCOPIC_INTERLACE_POST_PROCESS_H
