#ifndef BABYLON_POSTPROCESSES_BLOOM_MERGE_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_BLOOM_MERGE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

FWD_CLASS_SPTR(BloomMergePostProcess)
FWD_CLASS_SPTR(PostProcess)

/**
 * @brief The BloomMergePostProcess merges blurred images with the original based on the values of
 * the circle of confusion.
 */
class BABYLON_SHARED_EXPORT BloomMergePostProcess : public PostProcess {

public:
  template <typename... Ts>
  static BloomMergePostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<BloomMergePostProcess>(
      new BloomMergePostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~BloomMergePostProcess() override; // = default

protected:
  /**
   * @brief Creates a new instance of @see BloomMergePostProcess.
   * @param name The name of the effect.
   * @param originalFromInput Post process which's input will be used for the merge.
   * @param blurred Blurred highlights post process which's output will be used.
   * @param weight Weight of the bloom to be added to the original input.
   * @param options The required width/height ratio to downsize to before computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   * @param textureType Type of textures used when performing the post process. (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in the constructor. The
   * updateEffect method can be used to compile the shader at a later time. (default: false)
   */
  BloomMergePostProcess(const std::string& name, const PostProcessPtr& originalFromInput,
                        const PostProcessPtr& blurred, float weight,
                        const std::variant<float, PostProcessOptions>& options,
                        const CameraPtr& camera,
                        const std::optional<unsigned int>& samplingMode = std::nullopt,
                        Engine* engine = nullptr, bool reusable = false,
                        unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                        bool blockCompilation    = false);

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "BloomMergePostProcess" string
   */
  std::string getClassName() const override;

public:
  PostProcessPtr originalFromInput;
  PostProcessPtr blurred;

  /**
   * Weight of the bloom to be added to the original input
   */
  float weight;

}; // end of class BloomMergePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_BLOOM_MERGE_POST_PROCESS_H
