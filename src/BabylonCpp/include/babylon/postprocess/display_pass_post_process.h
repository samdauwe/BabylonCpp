#ifndef BABYLON_POSTPROCESS_DISPLAY_PASS_POST_PROCESS_H
#define BABYLON_POSTPROCESS_DISPLAY_PASS_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT DisplayPassPostProcess : public PostProcess {

public:
  DisplayPassPostProcess(const string_t& name, float ratio,
                         const CameraPtr& camera, unsigned int samplingMode,
                         Engine* engine, bool reusable = false);
  ~DisplayPassPostProcess();

}; // end of class DisplayPassPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_DISPLAY_PASS_POST_PROCESS_H
