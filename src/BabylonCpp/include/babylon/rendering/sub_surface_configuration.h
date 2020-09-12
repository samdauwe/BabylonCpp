#ifndef BABYLON_RENDERING_SUB_SURFACE_CONFIGURATION_H
#define BABYLON_RENDERING_SUB_SURFACE_CONFIGURATION_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/maths/color3.h>
#include <babylon/rendering/pre_pass_effect_configuration.h>

namespace BABYLON {

class Scene;
class SubSurfaceScatteringPostProcess;
using SubSurfaceScatteringPostProcessPtr = std::shared_ptr<SubSurfaceScatteringPostProcess>;

/**
 * @brief Contains all parameters needed for the prepass to perform screen space subsurface
 * scattering.
 */
class BABYLON_SHARED_EXPORT SubSurfaceConfiguration : public PrePassEffectConfiguration {

public:
  /**
   * @brief Builds a subsurface configuration object.
   * @param scene The scene
   */
  SubSurfaceConfiguration(Scene* scene);
  virtual ~SubSurfaceConfiguration() = default;

  /**
   * @brief Adds a new diffusion profile.
   * Useful for more realistic subsurface scattering on diverse materials.
   * @param color The color of the diffusion profile. Should be the average color of the material.
   * @return The index of the diffusion profile for the material subsurface configuration
   */
  size_t addDiffusionProfile(const Color3& color);

  /**
   * @brief Creates the sss post process.
   * @return The created post process
   */
  PostProcessPtr createPostProcess() override;

  /**
   * @brief Deletes all diffusion profiles.
   * Note that in order to render subsurface scattering, you should have at least 1 diffusion
   * profile.
   */
  void clearAllDiffusionProfiles();

  /**
   * @brief Disposes this object.
   */
  void dispose() override;

  // clang-format off
  /**
   * @hidden
   * https://zero-radiance.github.io/post/sampling-diffusion/
   *
   * Importance sample the normalized diffuse reflectance profile for the computed value of 's'.
   * ------------------------------------------------------------------------------------
   * R[r, phi, s]   = s * (Exp[-r * s] + Exp[-r * s / 3]) / (8 * Pi * r)
   * PDF[r, phi, s] = r * R[r, phi, s]
   * CDF[r, s]      = 1 - 1/4 * Exp[-r * s] - 3/4 * Exp[-r * s / 3]
   * ------------------------------------------------------------------------------------
   * We importance sample the color channel with the widest scattering distance.
   */
  // clang-format on
  float getDiffusionProfileParameters(const Color3& color) const;

protected:
  /**
   * @brief Gets the diffusion profile color for subsurface scattering.
   */
  Float32Array& get_ssDiffusionS();

  /**
   * @brief Gets the diffusion profile max color channel value for subsurface scattering.
   */
  Float32Array& get_ssDiffusionD();

  /**
   * @brief Gets the diffusion profile filter radius for subsurface scattering.
   */
  Float32Array& get_ssFilterRadii();

private:
  /**
   * @brief Performs sampling of a Normalized Burley diffusion profile in polar coordinates.
   * 'u' is the random number (the value of the CDF): [0, 1).
   * rcp(s) = 1 / ShapeParam = ScatteringDistance.
   * Returns the sampled radial distance, s.t. (u = 0 -> r = 0) and (u = 1 -> r = Inf).
   */
  float _sampleBurleyDiffusionProfile(float u, float rcpS) const;

public:
  /**
   * Post process to attach for screen space subsurface scattering
   */
  SubSurfaceScatteringPostProcessPtr postProcess;

  /**
   * Diffusion profile color for subsurface scattering
   */
  ReadOnlyProperty<SubSurfaceConfiguration, Float32Array> ssDiffusionS;

  /**
   * Diffusion profile max color channel value for subsurface scattering
   */
  ReadOnlyProperty<SubSurfaceConfiguration, Float32Array> ssDiffusionD;

  /**
   * Diffusion profile filter radius for subsurface scattering
   */
  ReadOnlyProperty<SubSurfaceConfiguration, Float32Array> ssFilterRadii;

  /**
   * Diffusion profile colors for subsurface scattering
   * You can add one diffusion color using `addDiffusionProfile` on `scene.prePassRenderer`
   * See ...
   * Note that you can only store up to 5 of them
   */
  std::vector<Color3> ssDiffusionProfileColors;

  /**
   * Defines the ratio real world => scene units.
   * Used for subsurface scattering
   */
  float metersPerUnit;

private:
  Float32Array _ssDiffusionS;
  Float32Array _ssFilterRadii;
  Float32Array _ssDiffusionD;

  Scene* _scene;

}; // end of class SubSurfaceConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_SUB_SURFACE_CONFIGURATION_H
