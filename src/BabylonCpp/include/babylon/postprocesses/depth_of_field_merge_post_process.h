#ifndef BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

/**
 * @brief The DepthOfFieldMergePostProcess merges blurred images with the
 * original based on the values of the circle of confusion.
 */
class BABYLON_SHARED_EXPORT DepthOfFieldMergePostProcess : public PostProcess {

public:
  /**
   * @brief Creates a new instance of DepthOfFieldMergePostProcess.
   * @param name The name of the effect.
   * @param originalFromInput Post process which's input will be used for the
   * merge.
   * @param circleOfConfusion Circle of confusion post process which's output
   * will be used to blur each pixel.
   * @param blurSteps Blur post processes from low to high which will be mixed
   * with the original image.
   * @param options The required width/height ratio to downsize to before
   * computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass.
   * (default: 0)
   * @param engine The engine which the post process will be applied. (default:
   * current engine)
   * @param reusable If the post process can be reused on the same frame.
   * (default: false)
   * @param textureType Type of textures used when performing the post process.
   * (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in
   * the constructor. The updateEffect method can be used to compile the shader
   * at a later time. (default: false)
   */
  DepthOfFieldMergePostProcess(
    const std::string& name, PostProcess* originalFromInput,
    PostProcess* circleOfConfusion, const std::vector<PostProcess*>& blurSteps,
    const std::variant<float, PostProcessOptions>& options,
    const CameraPtr& camera, unsigned int samplingMode = 0,
    Engine* engine = nullptr, bool reusable = false,
    unsigned int textureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
    bool blockCompilation    = false);
  ~DepthOfFieldMergePostProcess() override;

  /**
   * @brief Updates the effect with the current post process compile time values
   * and recompiles the shader.
   * @param defines Define statements that should be added at the beginning of
   * the shader. (default: null)
   * @param uniforms Set of uniform variables that will be passed to the shader.
   * (default: null)
   * @param samplers Set of Texture2D variables that will be passed to the
   * shader. (default: null)
   * @param indexParameters The index parameters to be used for babylons include
   * syntax "#include<kernelBlurVaryingDeclaration>[0..varyingCount]". (default:
   * undefined) See usage in babylon.blurPostProcess.ts and kernelBlur.vertex.fx
   * @param onCompiled Called when the shader has been compiled.
   * @param onError Called if there is an error when compiling a shader.
   */
  void updateEffect(
    const std::string& defines                                           = "",
    const std::vector<std::string>& uniforms                             = {},
    const std::vector<std::string>& samplers                             = {},
    const std::unordered_map<std::string, unsigned int>& indexParameters = {},
    const std::function<void(Effect* effect)>& onCompiled = nullptr,
    const std::function<void(Effect* effect, const std::string& errors)>&
      onError
    = nullptr) override;

private:
  std::vector<PostProcess*> blurSteps;

}; // end of class DepthOfFieldMergePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H
