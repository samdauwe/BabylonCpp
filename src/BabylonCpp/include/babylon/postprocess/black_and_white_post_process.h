#ifndef BABYLON_POSTPROCESS_BLACK_AND_WHITE_POST_PROCESS_H
#define BABYLON_POSTPROCESS_BLACK_AND_WHITE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

class BlackAndWhitePostProcess;
using BlackAndWhitePostProcessPtr = std::shared_ptr<BlackAndWhitePostProcess>;

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT BlackAndWhitePostProcess : public PostProcess {

public:
  template <typename... Ts>
  static BlackAndWhitePostProcessPtr New(Ts&&... args)
  {
    return std::shared_ptr<BlackAndWhitePostProcess>(
      new BlackAndWhitePostProcess(std::forward<Ts>(args)...));
  }
  ~BlackAndWhitePostProcess();

protected:
  BlackAndWhitePostProcess(const std::string& name, float ratio,
                           const CameraPtr& camera, unsigned int samplingMode,
                           Engine* engine, bool reusable = false);

public:
  float degree;

}; // end of class BlackAndWhitePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_BLACK_AND_WHITE_POST_PROCESS_H
