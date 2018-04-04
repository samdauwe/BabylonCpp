#ifndef BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H
#define BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief The DepthOfFieldMergePostProcess merges blurred images with the
 * original based on the values of the circle of confusion.
 */
class BABYLON_SHARED_EXPORT DepthOfFieldMergePostProcess : public PostProcess {

public:
  /**
   * @brief Creates a new instance of @see CircleOfConfusionPostProcess
   * @param name The name of the effect.
   * @param original The non-blurred image to be modified
   * @param circleOfConfusion The circle of confusion post process that will
   * determine how blurred each pixel should become.
   * @param blurSteps Incrimental bluring post processes.
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
    const string_t& name, PostProcess* original, PostProcess* circleOfConfusion,
    const vector_t<PostProcess*>& blurSteps,
    const Variant<float, PostProcessOptions>& options, Camera* camera,
    unsigned int samplingMode = 0, Engine* engine = nullptr,
    bool reusable            = false,
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
    const string_t& defines = "", const vector_t<string_t>& uniforms = {},
    const vector_t<string_t>& samplers                             = {},
    const unordered_map_t<string_t, unsigned int>& indexParameters = {},
    const ::std::function<void(Effect* effect)>& onCompiled        = nullptr,
    const ::std::function<void(Effect* effect, const string_t& errors)>& onError
    = nullptr) override;

private:
  vector_t<PostProcess*> blurSteps;

}; // end of class DepthOfFieldMergePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_DEPTH_OF_FIELD_MERGE_POST_PROCESS_H
