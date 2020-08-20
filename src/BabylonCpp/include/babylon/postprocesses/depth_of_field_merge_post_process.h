#ifndef BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class DepthOfFieldMergePostProcess;
using DepthOfFieldMergePostProcessPtr = std::shared_ptr<DepthOfFieldMergePostProcess>;

/**
 * @brief The DepthOfFieldMergePostProcess merges blurred images with the original based on the
 * values of the circle of confusion.
 */
class BABYLON_SHARED_EXPORT DepthOfFieldMergePostProcess : public PostProcess {

public:
  template <typename... Ts>
  static DepthOfFieldMergePostProcessPtr
  New(const std::string& iName, const PostProcessPtr& originalFromInput,
      const PostProcessPtr& circleOfConfusion, const std::vector<PostProcessPtr>& blurSteps,
      const std::variant<float, PostProcessOptions>& options, const CameraPtr& camera,
      const std::optional<unsigned int>& samplingMode = std::nullopt, Engine* engine = nullptr,
      bool reusable = false, unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
      bool blockCompilation = false)
  {
    auto r = std::shared_ptr<DepthOfFieldMergePostProcess>(new DepthOfFieldMergePostProcess(
      iName, originalFromInput, circleOfConfusion, blurSteps, options, camera, samplingMode, engine,
      reusable, textureType, blockCompilation));
    if (!blockCompilation)
      r->updateEffect();
    return r;
  }

  ~DepthOfFieldMergePostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "DepthOfFieldMergePostProcess" string
   */
  std::string getClassName() const override;

  /**
   * @brief Updates the effect with the current post process compile time values and recompiles the
   * shader.
   * @param defines Define statements that should be added at the beginning of the shader. (default:
   * null)
   * @param uniforms Set of uniform variables that will be passed to the shader. (default: null)
   * @param samplers Set of Texture2D variables that will be passed to the shader. (default: null)
   * @param indexParameters The index parameters to be used for babylons include syntax
   * "#include<kernelBlurVaryingDeclaration>[0..varyingCount]". (default: undefined) See usage in
   * babylon.blurPostProcess.ts and kernelBlur.vertex.fx
   * @param onCompiled Called when the shader has been compiled.
   * @param onError Called if there is an error when compiling a shader.
   */
  void updateEffect(const std::string& defines = "", const std::vector<std::string>& uniforms = {},
                    const std::vector<std::string>& samplers                             = {},
                    const std::unordered_map<std::string, unsigned int>& indexParameters = {},
                    const std::function<void(Effect* effect)>& onCompiled                = nullptr,
                    const std::function<void(Effect* effect, const std::string& errors)>& onError
                    = nullptr,
                    const std::string& vertexUrl   = "",
                    const std::string& fragmentUrl = "") override;

protected:
  /**
   * @brief Creates a new instance of DepthOfFieldMergePostProcess.
   * @param name The name of the effect.
   * @param originalFromInput Post process which's input will be used for the merge.
   * @param circleOfConfusion Circle of confusion post process which's output will be used to blur
   * each pixel.
   * @param blurSteps Blur post processes from low to high which will be mixed with the original
   * image.
   * @param options The required width/height ratio to downsize to before computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   * @param textureType Type of textures used when performing the post process. (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in the constructor. The
   * updateEffect method can be used to compile the shader at a later time. (default: false)
   */
  DepthOfFieldMergePostProcess(const std::string& name, const PostProcessPtr& originalFromInput,
                               const PostProcessPtr& circleOfConfusion,
                               const std::vector<PostProcessPtr>& blurSteps,
                               const std::variant<float, PostProcessOptions>& options,
                               const CameraPtr& camera,
                               const std::optional<unsigned int>& samplingMode = std::nullopt,
                               Engine* engine = nullptr, bool reusable = false,
                               unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                               bool blockCompilation    = false);

private:
  std::vector<PostProcessPtr> blurSteps;

}; // end of class DepthOfFieldMergePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H
