#ifndef BABYLON_POSTPROCESS_EXTRACT_HIGHLIGHTS_POST_PROCESS_H
#define BABYLON_POSTPROCESS_EXTRACT_HIGHLIGHTS_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief The extract highlights post process sets all pixels to black except
 * pixels above the specified luminance threshold. Used as the first step for a
 * bloom effect.
 */
class BABYLON_SHARED_EXPORT ExtractHighlightsPostProcess : public PostProcess {

public:
  ExtractHighlightsPostProcess(
    const string_t& name, const Variant<float, PostProcessOptions>& options,
    const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
    bool reusable            = false,
    unsigned int textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
    bool blockCompilation    = false);
  ~ExtractHighlightsPostProcess();

public:
  /**
   * The luminance threshold, pixels below this value will be set to black.
   */
  float threshold;

  /**
   * Internal
   */
  float _exposure;
  /**
   * Post process which has the input texture to be used when performing
   * highlight extraction
   */
  PostProcess* _inputPostProcess;

}; // end of class ExtractHighlightsPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_EXTRACT_HIGHLIGHTS_POST_PROCESS_H
