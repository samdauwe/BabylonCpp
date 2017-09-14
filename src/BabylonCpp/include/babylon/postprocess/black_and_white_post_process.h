#ifndef BABYLON_POSTPROCESS_BLACK_AND_WHITE_POST_PROCESS_H
#define BABYLON_POSTPROCESS_BLACK_AND_WHITE_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT BlackAndWhitePostProcess : public PostProcess {

public:
  BlackAndWhitePostProcess(const string_t& name, float ratio, Camera* camera,
                           unsigned int samplingMode, Engine* engine,
                           bool reusable = false);
  ~BlackAndWhitePostProcess();

public:
  float degree;

}; // end of class BlackAndWhitePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_BLACK_AND_WHITE_POST_PROCESS_H
