#ifndef BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_STANDARD_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_STANDARD_RENDERING_PIPLINE_H

#include <map>
#include <nlohmann/json_fwd.hpp>
#include <variant>

#include <babylon/babylon_api.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

using json = nlohmann::json;

namespace BABYLON {

class BlurPostProcess;
class FxaaPostProcess;
class Light;
class PostProcess;
class Scene;
class ShadowLight;
class StandardRenderingPipeline;
class Texture;
using BlurPostProcessPtr           = std::shared_ptr<BlurPostProcess>;
using FxaaPostProcessPtr           = std::shared_ptr<FxaaPostProcess>;
using PostProcessPtr               = std::shared_ptr<PostProcess>;
using TexturePtr                   = std::shared_ptr<Texture>;
using ShadowLightPtr               = std::shared_ptr<ShadowLight>;
using StandardRenderingPipelinePtr = std::shared_ptr<StandardRenderingPipeline>;

/**
 * @brief Standard rendering pipeline.
 * Default pipeline should be used going forward but the standard pipeline will
 * be kept for backwards compatibility.
 * @see https://doc.babylonjs.com/how_to/using_standard_rendering_pipeline
 */
class BABYLON_SHARED_EXPORT StandardRenderingPipeline
    : public PostProcessRenderPipeline,
      public IAnimatable {

public:
  /**
   * Luminance steps
   */
  static unsigned int LuminanceSteps;

public:
  template <typename... Ts>
  static StandardRenderingPipelinePtr New(Ts&&... args)
  {
    auto renderingPipeline = std::shared_ptr<StandardRenderingPipeline>(
      new StandardRenderingPipeline(std::forward<Ts>(args)...));
    renderingPipeline->addToScene(renderingPipeline);

    return renderingPipeline;
  }
  virtual ~StandardRenderingPipeline() override;

  void addToScene(const StandardRenderingPipelinePtr& renderingPipeline);
  IReflect::Type type() const override;
  float operator[](const std::string& key) const;

  /**
   * @brief Dispose of the pipeline and stop all post processes.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Serialize the rendering pipeline (Used when exporting).
   * @returns the serialized object
   */
  json serialize() const;

  /**
   * @brief Parse the serialized pipeline.
   * @param source Source pipeline.
   * @param scene The scene to load the pipeline to.
   * @param rootUrl The URL of the serialized pipeline.
   * @returns An instantiated pipeline from the serialized object.
   */
  static std::unique_ptr<StandardRenderingPipeline>
  Parse(const json& source, Scene* scene, const std::string& url);

protected:
  /**
   * @brief Default pipeline should be used going forward but the standard
   * pipeline will be kept for backwards compatibility.
   * @param name The rendering pipeline name
   * @param scene The scene linked to this pipeline
   * @param ratio The size of the postprocesses (0.5 means that your postprocess
   * will have a width = canvas.width 0.5 and a height = canvas.height 0.5)
   * @param originalPostProcess the custom original color post-process. Must be
   * "reusable". Can be null.
   * @param cameras The array of cameras that the rendering pipeline will be
   * attached to
   */
  StandardRenderingPipeline(const std::string& name, Scene* scene, float ratio,
                            PostProcess* originalPostProcess      = nullptr,
                            const std::vector<CameraPtr>& cameras = {});

  bool get_bloomEnabled() const;
  void set_bloomEnabled(bool enabled);
  bool get_depthOfFieldEnabled() const;
  void set_depthOfFieldEnabled(bool enabled);
  bool get_lensFlareEnabled() const;
  void set_lensFlareEnabled(bool enabled);
  bool get_HDREnabled() const;
  void set_HDREnabled(bool enabled);
  bool get_VLSEnabled() const;
  void set_VLSEnabled(bool enabled);
  bool get_motionBlurEnabled() const;
  void set_motionBlurEnabled(bool enabled);
  bool get_fxaaEnabled() const;
  void set_fxaaEnabled(bool enabled);
  float get_volumetricLightStepsCount() const;
  void set_volumetricLightStepsCount(float count);
  float get_motionBlurSamples() const;
  void set_motionBlurSamples(float samples);
  unsigned int get_samples() const;
  void set_samples(unsigned int sampleCount);

private:
  void _buildPipeline();
  // Down Sample X4 Post-Processs
  void _createDownSampleX4PostProcess(Scene* scene, float ratio);
  // Brightpass Post-Process
  void _createBrightPassPostProcess(Scene* scene, float ratio);
  // Create blur H&V post-processes
  void _createBlurPostProcesses(Scene* scene, float ratio, unsigned int indice,
                                const std::string& blurWidthKey = "blurWidth");
  // Create texture adder post-process
  void _createTextureAdderPostProcess(Scene* scene, float ratio);
  void _createVolumetricLightPostProcess(Scene* scene, float ratio);
  // Create luminance
  void _createLuminancePostProcesses(Scene* scene, unsigned int textureType);
  // Create HDR post-process
  void _createHdrPostProcess(Scene* scene, float ratio);
  // Create lens flare post-process
  void _createLensFlarePostProcess(Scene* scene, float ratio);
  // Create depth-of-field post-process
  void _createDepthOfFieldPostProcess(Scene* scene, float ratio);
  // Create motion blur post-process
  void _createMotionBlurPostProcess(Scene* scene, float ratio);
  TexturePtr _getDepthTexture();
  void _disposePostProcesses();

public:
  // Post-processes

  /**
   * Post-process which contains the original scene color before the pipeline
   * applies all the effects
   */
  PostProcessPtr originalPostProcess;
  /**
   * Post-process used to down scale an image x4
   */
  PostProcessPtr downSampleX4PostProcess;
  /**
   * Post-process used to calculate the illuminated surfaces controlled by a
   * threshold
   */
  PostProcessPtr brightPassPostProcess;
  /**
   * Post-process array storing all the horizontal blur post-processes used by
   * the pipeline
   */
  std::vector<PostProcessPtr> blurHPostProcesses;
  /**
   * Post-process array storing all the vertical blur post-processes used by the
   * pipeline
   */
  std::vector<PostProcessPtr> blurVPostProcesses;
  /**
   * Post-process used to add colors of 2 textures (typically brightness + real
   * scene color)
   */
  PostProcessPtr textureAdderPostProcess;

  /**
   * Post-process used to create volumetric lighting effect
   */
  PostProcessPtr volumetricLightPostProcess;
  /**
   * Post-process used to smooth the previous volumetric light post-process on
   * the X axis
   */
  BlurPostProcessPtr volumetricLightSmoothXPostProcess;
  /**
   * Post-process used to smooth the previous volumetric light post-process on
   * the Y axis
   */
  BlurPostProcessPtr volumetricLightSmoothYPostProcess;
  /**
   * Post-process used to merge the volumetric light effect and the real scene
   * color
   */
  PostProcessPtr volumetricLightMergePostProces;
  /**
   * Post-process used to store the final volumetric light post-process
   * (attach/detach for debug purpose)
   */
  PostProcessPtr volumetricLightFinalPostProcess;

  /**
   * Base post-process used to calculate the average luminance of the final
   * image for HDR
   */
  PostProcessPtr luminancePostProcess;
  /**
   * Post-processes used to create down sample post-processes in order to get
   * the average luminance of the final image for HDR
   * Array of length "StandardRenderingPipeline.LuminanceSteps"
   */
  std::vector<PostProcessPtr> luminanceDownSamplePostProcesses;
  /**
   * Post-process used to create a HDR effect (light adaptation)
   */
  PostProcessPtr hdrPostProcess;

  /**
   * Post-process used to store the final texture adder post-process
   * (attach/detach for debug purpose)
   */
  PostProcessPtr textureAdderFinalPostProcess;
  /**
   * Post-process used to store the final lens flare post-process (attach/detach
   * for debug purpose)
   */
  PostProcessPtr lensFlareFinalPostProcess;
  /**
   * Post-process used to merge the final HDR post-process and the real scene
   * color
   */
  PostProcessPtr hdrFinalPostProcess;

  /**
   * Post-process used to create a lens flare effect
   */
  PostProcessPtr lensFlarePostProcess;
  /**
   * Post-process that merges the result of the lens flare post-process and the
   * real scene color
   */
  PostProcessPtr lensFlareComposePostProcess;

  /**
   * Post-process used to create a motion blur effect
   */
  PostProcessPtr motionBlurPostProcess;

  /**
   * Post-process used to create a depth of field effect
   */
  PostProcessPtr depthOfFieldPostProcess;
  /**
   * The Fast Approximate Anti-Aliasing post process which attemps to remove
   * aliasing from an image.
   */
  FxaaPostProcessPtr fxaaPostProcess;

  // Values

  /**
   * Represents the brightness threshold in order to configure the illuminated
   * surfaces
   */
  float brightThreshold;

  /**
   * Configures the blur intensity used for surexposed surfaces are highlighted
   * surfaces (light halo)
   */
  float blurWidth;
  /**
   * Sets if the blur for highlighted surfaces must be only horizontal
   */
  bool horizontalBlur;

  /**
   * Sets the overall exposure used by the pipeline
   */
  float exposure;

  /**
   * Texture used typically to simulate "dirty" on camera lens
   */
  TexturePtr lensTexture;

  /**
   * Represents the offset coefficient based on Rayleigh principle. Typically in
   * interval [-0.2, 0.2]
   */
  float volumetricLightCoefficient;
  /**
   * The overall power of volumetric lights, typically in interval [0, 10]
   * maximum
   */
  float volumetricLightPower;
  /**
   * Used the set the blur intensity to smooth the volumetric lights
   */
  float volumetricLightBlurScale;

  /**
   * Light (spot or directional) used to generate the volumetric lights rays
   * The source light must have a shadow generate so the pipeline can get its
   * depth map
   */
  ShadowLightPtr sourceLight;

  /**
   * For eye adaptation, represents the minimum luminance the eye can see
   */
  float hdrMinimumLuminance;
  /**
   * For eye adaptation, represents the decrease luminance speed
   */
  float hdrDecreaseRate;
  /**
   * For eye adaptation, represents the increase luminance speed
   */
  float hdrIncreaseRate;

  /**
   * Lens color texture used by the lens flare effect. Mandatory if lens flare
   * effect enabled
   */
  TexturePtr lensColorTexture;
  /**
   * The overall strengh for the lens flare effect
   */
  float lensFlareStrength;
  /**
   * Dispersion coefficient for lens flare ghosts
   */
  float lensFlareGhostDispersal;
  /**
   * Main lens flare halo width
   */
  float lensFlareHaloWidth;
  /**
   * Based on the lens distortion effect, defines how much the lens flare result
   * is distorted
   */
  float lensFlareDistortionStrength;
  /**
   * Lens star texture must be used to simulate rays on the flares and is
   * available in the documentation
   */
  TexturePtr lensStarTexture;
  /**
   * As the "lensTexture" (can be the same texture or different), it is used to
   * apply the lens flare effect by taking account of the dirt texture
   */
  TexturePtr lensFlareDirtTexture;

  /**
   * Represents the focal length for the depth of field effect
   */
  float depthOfFieldDistance;

  /**
   * Represents the blur intensity for the blurred part of the depth of field
   * effect
   */
  float depthOfFieldBlurWidth;

  /**
   * For motion blur, defines how much the image is blurred by the movement
   */
  float motionStrength;

  /**
   * List of animations for the pipeline (IAnimatable implementation)
   */
  std::vector<Animation> animations;

  /**
   * Specifies if the bloom pipeline is enabled
   */
  Property<StandardRenderingPipeline, bool> bloomEnabled;

  /**
   * Specifies if the depth of field pipeline is enabed
   */
  Property<StandardRenderingPipeline, bool> depthOfFieldEnabled;

  /**
   * Specifies if the lens flare pipeline is enabed
   */
  Property<StandardRenderingPipeline, bool> lensFlareEnabled;

  /**
   * Specifies if the HDR pipeline is enabled
   */
  Property<StandardRenderingPipeline, bool> HDREnabled;

  /**
   * Specifies if the volumetric lights scattering effect is enabled
   */
  Property<StandardRenderingPipeline, bool> VLSEnabled;

  /**
   * Specifies if the motion blur effect is enabled
   */
  Property<StandardRenderingPipeline, bool> motionBlurEnabled;

  /**
   * Specifies if anti-aliasing is enabled
   */
  Property<StandardRenderingPipeline, bool> fxaaEnabled;

  /**
   * Specifies the number of steps used to calculate the volumetric lights
   * Typically in interval [50, 200]
   */
  Property<StandardRenderingPipeline, float> volumetricLightStepsCount;

  /**
   * Specifies the number of samples used for the motion blur effect
   * Typically in interval [16, 64]
   */
  Property<StandardRenderingPipeline, float> motionBlurSamples;

  /**
   * Specifies MSAA sample count, setting this to 4 will provide 4x anti
   * aliasing. (default: 1)
   */
  Property<StandardRenderingPipeline, unsigned int> samples;

private:
  Scene* _scene;
  PostProcessPtr _currentDepthOfFieldSource;
  PostProcessPtr _basePostProcess;
  float _hdrCurrentLuminance;
  unsigned int _floatTextureType;
  float _ratio;
  // Getters and setters
  bool _bloomEnabled;
  bool _depthOfFieldEnabled;
  bool _vlsEnabled;
  bool _lensFlareEnabled;
  bool _hdrEnabled;
  bool _motionBlurEnabled;
  bool _fxaaEnabled;
  float _motionBlurSamples;
  float _volumetricLightStepsCount;
  unsigned int _samples;

}; // end of class StandardRenderingPipeline

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_STANDARD_RENDERING_PIPLINE_H
