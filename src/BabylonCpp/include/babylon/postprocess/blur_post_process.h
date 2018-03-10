#ifndef BABYLON_POSTPROCESS_BLUR_POST_PROCESS_H
#define BABYLON_POSTPROCESS_BLUR_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/core/variant.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/math/vector2.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief  The Blur Post Process which blurs an image based on a kernel and
 * direction. Can be used twice in x and y directions to perform a guassian blur
 * in two passes.
 */
class BABYLON_SHARED_EXPORT BlurPostProcess : public PostProcess {

public:
  /**
   * @brief Creates a new instance of @see BlurPostProcess
   * @param name The name of the effect.
   * @param direction The direction in which to blur the image.
   * @param kernel The size of the kernel to be used when computing the blur.
   * eg. Size of 3 will blur the center pixel by 2 pixels surrounding it.
   * @param options The required width/height ratio to downsize to before
   * computing the render pass. (Use 1.0 for full size)
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass.
   * (default: 0)
   * @param engine The engine which the post process will be applied. (default:
   * current engine)
   * @param reusable If the post process can be reused on the same frame.
   * (default: false)
   * @param textureType Type of textures used when performing the post process.
   * (default: 0)
   */
  BlurPostProcess(const string_t& name, const Vector2& direction, float kernel,
                  const Variant<float, PostProcessOptions>& options,
                  Camera* camera,
                  unsigned int samplingMode
                  = TextureConstants::BILINEAR_SAMPLINGMODE,
                  Engine* engine = nullptr, bool reusable = false,
                  unsigned int textureType
                  = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
                  const string_t& defines = "");
  ~BlurPostProcess();

protected:
  void _updateParameters();

  /**
   * @brief Best kernels are odd numbers that when divided by 2, their integer
   * part is even, so 5, 9 or 13.
   * Other odd kernels optimize correctly but require proportionally more
   * samples, even kernels are
   * possible but will produce minor visual artifacts. Since each new kernel
   * requires a new shader we
   * want to minimize kernel changes, having gaps between physical kernels is
   * helpful in that regard.
   * The gaps between physical kernels are compensated for in the weighting of
   * the samples
   * @param idealKernel Ideal blur kernel.
   * @return Nearest best kernel.
   */
  float _nearestBestKernel(float idealKernel) const;

  /**
   * @brief Calculates the value of a Gaussian distribution with sigma 3 at a
   * given point.
   * @param x The point on the Gaussian distribution to sample.
   * @return the value of the Gaussian function at x.
   */
  float _gaussianWeight(float x) const;

  /**
   * @brief Generates a string that can be used as a floating point number in
   * GLSL.
   * @param x Value to print.
   * @param decimalFigures Number of decimal places to print the number to
   * (excluding trailing 0s).
   * @return GLSL float string.
   */
  string_t _glslFloat(float x, unsigned int decimalFigures = 8) const;

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
   * Length in pixels of the blur sample region.
   */
  Property<BlurPostProcess, float> kernel;

  /**
   * Wether or not the blur needs to unpack/repack floats.
   */
  Property<BlurPostProcess, bool> packedFloat;

protected:
  float _kernel;
  float _idealKernel;
  bool _packedFloat;

private:
  string_t _staticDefines;

}; // end of class BlurPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_BLUR_POST_PROCESS_H
