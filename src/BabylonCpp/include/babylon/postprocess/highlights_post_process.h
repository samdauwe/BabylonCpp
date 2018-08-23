#ifndef BABYLON_POSTPROCESS_HIGHLIGHTS_POST_PROCESS_H
#define BABYLON_POSTPROCESS_HIGHLIGHTS_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT HighlightsPostProcess : public PostProcess {

public:
  HighlightsPostProcess(const string_t& name, float ratio,
                        const CameraPtr& camera, unsigned int samplingMode,
                        Engine* engine, bool reusable = false,
                        unsigned int textureType
                        = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  ~HighlightsPostProcess();

}; // end of class HighlightsPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_HIGHLIGHTS_POST_PROCESS_H
