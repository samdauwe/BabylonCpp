#ifndef BABYLON_POSTPROCESS_ANAGLYPH_POST_PROCESS_H
#define BABYLON_POSTPROCESS_ANAGLYPH_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief Postprocess used to generate anaglyphic rendering.
 */
class BABYLON_SHARED_EXPORT AnaglyphPostProcess : public PostProcess {

public:
  /**
   * @brief Creates a new AnaglyphPostProcess.
   * @param name defines postprocess name
   * @param options defines creation options or target ratio scale
   * @param rigCameras defines cameras using this postprocess
   * @param samplingMode defines required sampling mode
   * (BABYLON.Texture.NEAREST_SAMPLINGMODE by default)
   * @param engine defines hosting engine
   * @param reusable defines if the postprocess will be reused multiple times
   * per frame
   */
  AnaglyphPostProcess(const string_t& name, float ratio,
                      const vector_t<CameraPtr>& rigCameras,
                      unsigned int samplingMode, Engine* engine,
                      bool reusable = false);
  ~AnaglyphPostProcess();

private:
  PostProcess* _passedProcess;

}; // end of class AnaglyphPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_ANAGLYPH_POST_PROCESS_H
