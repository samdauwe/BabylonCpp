#ifndef BABYLON_POSTPROCESS_ANAGLYPH_POST_PROCESS_H
#define BABYLON_POSTPROCESS_ANAGLYPH_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT AnaglyphPostProcess : public PostProcess {

public:
  AnaglyphPostProcess(const std::string& name, float ratio,
                      std::vector<Camera*> rigCameras,
                      unsigned int samplingMode, Engine* engine,
                      bool reusable = false);
  ~AnaglyphPostProcess();

private:
  PostProcess* _passedProcess;

}; // end of class AnaglyphPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_ANAGLYPH_POST_PROCESS_H
