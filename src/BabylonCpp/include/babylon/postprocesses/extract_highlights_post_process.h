#ifndef BABYLON_POSTPROCESSES_EXTRACT_HIGHLIGHTS_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_EXTRACT_HIGHLIGHTS_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/engine/constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class ExtractHighlightsPostProcess;
using ExtractHighlightsPostProcessPtr
  = std::shared_ptr<ExtractHighlightsPostProcess>;

/**
 * @brief The extract highlights post process sets all pixels to black except
 * pixels above the specified luminance threshold. Used as the first step for a
 * bloom effect.
 */
class BABYLON_SHARED_EXPORT ExtractHighlightsPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static ExtractHighlightsPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<ExtractHighlightsPostProcess>(
      new ExtractHighlightsPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~ExtractHighlightsPostProcess();

protected:
  ExtractHighlightsPostProcess(
    const std::string& name,
    const std::variant<float, PostProcessOptions>& options,
    const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
    bool reusable            = false,
    unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
    bool blockCompilation    = false);

public:
  /**
   * The luminance threshold, pixels below this value will be set to black.
   */
  float threshold;

  /**
   * Hidden
   */
  float _exposure;

  /**
   * Post process which has the input texture to be used when performing
   * highlight extraction
   * Hidden
   */
  PostProcess* _inputPostProcess;

}; // end of class ExtractHighlightsPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_EXTRACT_HIGHLIGHTS_POST_PROCESS_H
