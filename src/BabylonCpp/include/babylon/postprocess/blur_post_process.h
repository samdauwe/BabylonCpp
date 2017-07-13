#ifndef BABYLON_POSTPROCESS_BLUR_POST_PROCESS_H
#define BABYLON_POSTPROCESS_BLUR_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/math/vector2.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT BlurPostProcess : public PostProcess {

public:
  BlurPostProcess(const std::string& name, const Vector2& direction,
                  float kernel, float ratio, Camera* camera,
                  unsigned int samplingMode
                  = TextureConstants::BILINEAR_SAMPLINGMODE,
                  Engine* engine = nullptr, bool reusable = false,
                  unsigned int textureType
                  = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  ~BlurPostProcess();

  /**
   * @brief Sets the length in pixels of the blur sample region.
   */
  void setKernel(float v);

  /**
   * @brief Gets the length in pixels of the blur sample region.
   */
  float kernel() const;

  /**
   * @brief Sets whether or not the blur needs to unpack/repack floats.
   */
  void setPackedFloat(bool v);

  /**
   * @brief Gets whether or not the blur is unpacking/repacking floats.
   */
  bool packedFloat() const;

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
  std::string _glslFloat(float x, unsigned int decimalFigures = 8) const;

protected:
  float _kernel;
  float _idealKernel;
  bool _packedFloat;

}; // end of class BlurPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_BLUR_POST_PROCESS_H
