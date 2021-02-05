#ifndef BABYLON_POSTPROCESSES_BLUR_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_BLUR_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/maths/vector2.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

FWD_CLASS_SPTR(BlurPostProcess)

/**
 * @brief The Blur Post Process which blurs an image based on a kernel and direction.
 * Can be used twice in x and y directions to perform a gaussian blur in two passes.
 */
class BABYLON_SHARED_EXPORT BlurPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static BlurPostProcessPtr New(Ts&&... args)
  {
    auto postProcess
      = std::shared_ptr<BlurPostProcess>(new BlurPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~BlurPostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class
   * @returns "BlurPostProcess" string
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

  /**
   * @brief Hidden
   */
  static BlurPostProcessPtr _Parse(const json& parsedPostProcess, const CameraPtr& targetCamera,
                                   Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new instance BlurPostProcess.
   * @param name The name of the effect.
   * @param direction The direction in which to blur the image.
   * @param kernel The size of the kernel to be used when computing the blur. eg. Size of 3 will
   * blur the center pixel by 2 pixels surrounding it.
   * @param options The required width/height ratio to downsize to before computing the render pass.
   * (Use 1.0 for full size)
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   * @param textureType Type of textures used when performing the post process. (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in the constructor. The
   * updateEffect method can be used to compile the shader at a later time. (default: false)
   */
  BlurPostProcess(const std::string& name, const Vector2& iDrection, float kernel,
                  const std::variant<float, PostProcessOptions>& options, const CameraPtr& camera,
                  const std::optional<unsigned int>& samplingMode = std::nullopt,
                  Engine* engine = nullptr, bool reusable = false,
                  unsigned int textureType   = Constants::TEXTURETYPE_UNSIGNED_INT,
                  const std::string& defines = "", bool blockCompilation = false);

  void
  _updateParameters(const std::function<void(Effect* effect)>& onCompiled = nullptr,
                    const std::function<void(Effect* effect, const std::string& errors)>& onError
                    = nullptr);

  /**
   * @brief Best kernels are odd numbers that when divided by 2, their integer part is even, so 5, 9
   * or 13. Other odd kernels optimize correctly but require proportionally more samples, even
   * kernels are possible but will produce minor visual artifacts. Since each new kernel requires a
   * new shader we want to minimize kernel changes, having gaps between physical kernels is helpful
   * in that regard. The gaps between physical kernels are compensated for in the weighting of the
   * samples
   * @param idealKernel Ideal blur kernel.
   * @return Nearest best kernel.
   */
  float _nearestBestKernel(float idealKernel) const;

  /**
   * @brief Generates a string that can be used as a floating point number in GLSL.
   * @param x Value to print.
   * @param decimalFigures Number of decimal places to print the number to (excluding trailing 0s).
   * @return GLSL float string.
   */
  float _gaussianWeight(float x) const;

  /**
   * @brief Generates a string that can be used as a floating point number in GLSL.
   * @param x Value to print.
   * @param decimalFigures Number of decimal places to print the number to (excluding trailing 0s).
   * @return GLSL float string.
   */
  std::string _glslFloat(float x, unsigned int decimalFigures = 8) const;

private:
  /**
   * @brief Sets the length in pixels of the blur sample region.
   */
  void set_kernel(float v);

  /**
   * @brief Gets the length in pixels of the blur sample region.
   */
  float get_kernel() const;

  /**
   * @brief Sets whether or not the blur needs to unpack/repack floats.
   */
  void set_packedFloat(bool v);

  /**
   * @brief Gets whether or not the blur is unpacking/repacking floats.
   */
  bool get_packedFloat() const;

public:
  /**
   * The direction in which to blur the image
   */
  Vector2 direction;

  /**
   * Length in pixels of the blur sample region
   */
  Property<BlurPostProcess, float> kernel;

  /**
   * Whether or not the blur needs to unpack/repack floats
   */
  Property<BlurPostProcess, bool> packedFloat;

protected:
  float _kernel;
  float _idealKernel;
  bool _packedFloat;

private:
  std::string _staticDefines;
  bool blockCompilation;

}; // end of class BlurPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_BLUR_POST_PROCESS_H
