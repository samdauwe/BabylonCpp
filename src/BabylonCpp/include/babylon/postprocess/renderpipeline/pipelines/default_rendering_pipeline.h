#ifndef BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocess/depth_of_field_effect_blur_level.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief The default rendering pipeline can be added to a scene to apply common
 * post processing effects such as anti-aliasing or depth of field. See
 * https://doc.babylonjs.com/how_to/using_default_rendering_pipeline
 */
class BABYLON_SHARED_EXPORT DefaultRenderingPipeline
    : public PostProcessRenderPipeline {

private:
  /**
   * ID of the sharpen post process,
   */
  static constexpr const char* SharpenPostProcessId
    = "SharpenPostProcessEffect";

  /**
   * ID of the image processing post process;
   */
  static constexpr const char* ImageProcessingPostProcessId
    = "ImageProcessingPostProcessEffect";

  /**
   * ID of the Fast Approximate Anti-Aliasing post process;
   */
  static constexpr const char* FxaaPostProcessId = "FxaaPostProcessEffect";

  /**
   * ID of the chromatic aberration post process,
   */
  static constexpr const char* ChromaticAberrationPostProcessId
    = "ChromaticAberrationPostProcessEffect";

  /**
   * ID of the grain post process
   */
  static constexpr const char* GrainPostProcessId = "GrainPostProcessEffect";

public:
  /**
   * @brief constructor
   * @param {string} name - The rendering pipeline name
   * @param {BABYLON.Scene} scene - The scene linked to this pipeline
   * @param {any} ratio - The size of the postprocesses (0.5 means that your
   * postprocess will have a width = canvas.width 0.5 and a height =
   * canvas.height 0.5)
   * @param {BABYLON.Camera[]} cameras - The array of cameras that the rendering
   * pipeline will be attached to
   * @param {boolean} automaticBuild - if false, you will have to manually call
   * prepare() to update the pipeline
   */
  DefaultRenderingPipeline(const string_t& name, bool hdr, Scene* scene,
                           const unordered_map_t<string_t, Camera*>& cameras
                           = {},
                           bool automaticBuild = true);
  virtual ~DefaultRenderingPipeline() override;

  /**
   * @brief Force the compilation of the entire pipeline.
   */
  void prepare();

  /**
   * @brief Dispose of the pipeline and stop all post processes.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Serialize the rendering pipeline (Used when exporting)
   * @returns the serialized object
   */
  Json::object serialize() const;

  /**
   * @brief Parse the serialized pipeline
   * @param source Source pipeline.
   * @param scene The scene to load the pipeline to.
   * @param rootUrl The URL of the serialized pipeline.
   * @returns An instantiated pipeline from the serialized object.
   */
  static unique_ptr_t<DefaultRenderingPipeline>
  Parse(const Json::value& source, Scene* scene, const string_t& rootUrl);

private:
  /**
   * @brief Enable or disable the sharpen process from the pipeline.
   */
  void set_sharpenEnabled(bool enabled);
  bool get_sharpenEnabled() const;

  /**
   * @brief Specifies the size of the bloom blur kernel, relative to the final
   * output size.
   */
  void set_bloomKernel(float value);
  float get_bloomKernel() const;

  /**
   * @brief The strength of the bloom.
   */
  void set_bloomWeight(float value);
  float get_bloomWeight() const;

  /**
   * @brief The threshold of the bloom.
   */
  void set_bloomThreshold(float value);
  float get_bloomThreshold() const;

  /**
   * @brief The scale of the bloom, lower value will provide better performance.
   */
  void set_bloomScale(float value);
  float get_bloomScale() const;

  /**
   * @brief Enable or disable the bloom from the pipeline.
   */
  void set_bloomEnabled(bool enabled);
  bool get_bloomEnabled() const;

  /**
   * @brief If the depth of field is enabled.
   */
  void set_depthOfFieldEnabled(bool enabled);
  bool get_depthOfFieldEnabled() const;

  /**
   * @brief Blur level of the depth of field effect. (Higher blur will effect
   * performance).
   */
  DepthOfFieldEffectBlurLevel& get_depthOfFieldBlurLevel();
  void set_depthOfFieldBlurLevel(const DepthOfFieldEffectBlurLevel& value);

  /**
   * @brief If the anti aliasing is enabled.
   */
  void set_fxaaEnabled(bool enabled);
  bool get_fxaaEnabled() const;

  /**
   * @brief MSAA sample count, setting this to 4 will provide 4x anti aliasing.
   * (default: 1)
   */
  void set_samples(unsigned int sampleCount);
  unsigned int get_samples() const;

  /**
   * @brief If image processing is enabled.
   */
  void set_imageProcessingEnabled(bool enabled);
  bool get_imageProcessingEnabled() const;

  /**
   * @brief Enable or disable the chromaticAberration process from the pipeline.
   */
  void set_chromaticAberrationEnabled(bool enabled);
  bool get_chromaticAberrationEnabled() const;

  /**
   * @brief Enable or disable the grain process from the pipeline.
   */
  void set_grainEnabled(bool enabled);
  bool get_grainEnabled() const;

  void _rebuildBloom();
  void _setAutoClearAndTextureSharing(PostProcess* postProcess,
                                      bool skipTextureSharing = false);
  void _buildPipeline();
  void _disposePostProcesses(bool disposeNonRecreated = false);

public:
  // Post-processes
  /**
   * Sharpen post process which will apply a sharpen convolution to enhance
   * edges
   */
  SharpenPostProcess* sharpen;
  /**
   * Depth of field effect, applies a blur based on how far away objects are
   * from the focus distance.
   */
  DepthOfFieldEffect* depthOfField;
  /**
   * The Fast Approximate Anti-Aliasing post process which attemps to remove
   * aliasing from an image.
   */
  FxaaPostProcess* fxaa;
  /**
   * Image post processing pass used to perform operations such as tone mapping
   * or color grading.
   */
  PostProcess* imageProcessing; // ImageProcessingPostProcess

  /**
   * Chromatic aberration post process which will shift rgb colors in the image
   */
  ChromaticAberrationPostProcess* chromaticAberration;

  /**
   * Grain post process which add noise to the image
   */
  GrainPostProcess* grain;

  /**
   * Animations which can be used to tweak settings over a period of time
   */
  vector_t<Animation*> animations;

  /**
   * Specifies the size of the bloom blur kernel, relative to the final output
   * size
   */
  Property<DefaultRenderingPipeline, float> bloomKernel;

  /**
   * Enable or disable the sharpen process from the pipeline
   */
  Property<DefaultRenderingPipeline, bool> sharpenEnabled;

  /**
   * The strength of the bloom
   */
  Property<DefaultRenderingPipeline, float> bloomWeight;

  /**
   * The threshold of the bloom
   */
  Property<DefaultRenderingPipeline, float> bloomThreshold;

  /**
   * The scale of the bloom, lower value will provide better performance
   */
  Property<DefaultRenderingPipeline, float> bloomScale;

  /**
   * Enable or disable the bloom from the pipeline
   */
  Property<DefaultRenderingPipeline, bool> bloomEnabled;

  /**
   * If the depth of field is enabled
   */
  Property<DefaultRenderingPipeline, bool> depthOfFieldEnabled;

  /**
   * Blur level of the depth of field effect. (Higher blur will effect
   * performance).
   */
  Property<DefaultRenderingPipeline, DepthOfFieldEffectBlurLevel>
    depthOfFieldBlurLevel;

  /**
   * If the anti aliasing is enabled
   */
  Property<DefaultRenderingPipeline, bool> fxaaEnabled;

  /**
   * MSAA sample count, setting this to 4 will provide 4x anti aliasing
   */
  Property<DefaultRenderingPipeline, unsigned int> samples;

  /**
   * If image processing is enabled
   */
  Property<DefaultRenderingPipeline, bool> imageProcessingEnabled;

  /**
   * Enable or disable the chromaticAberration process from the pipeline
   */
  Property<DefaultRenderingPipeline, bool> chromaticAberrationEnabled;

  /**
   * Enable or disable the grain process from the pipeline
   */
  Property<DefaultRenderingPipeline, bool> grainEnabled;

private:
  // Post-processes
  PostProcessRenderEffect* _sharpenEffect;
  BloomEffect* bloom;
  PostProcessRenderEffect* _chromaticAberrationEffect;
  PostProcessRenderEffect* _grainEffect;

  Observer<ImageProcessingConfiguration>::Ptr
    _imageProcessingConfigurationObserver;

  // Values
  bool _sharpenEnabled;
  bool _bloomEnabled;
  bool _depthOfFieldEnabled;
  DepthOfFieldEffectBlurLevel _depthOfFieldBlurLevel;
  bool _fxaaEnabled;
  bool _imageProcessingEnabled;
  unsigned int _defaultPipelineTextureType;
  float _bloomScale;
  bool _chromaticAberrationEnabled;
  bool _grainEnabled;
  bool _buildAllowed;

  Observer<Engine>::Ptr _resizeObserver;
  float _hardwareScaleLevel;
  float _bloomKernel;

  /**
   * Specifies the weight of the bloom in the final rendering
   */
  float _bloomWeight;

  float _bloomThreshold;

  bool _hdr;
  unsigned int _samples;
  Scene* _scene;
  vector_t<Camera*> _originalCameras;

  bool _hasCleared;
  PostProcess* _prevPostProcess;
  PostProcess* _prevPrevPostProcess;

}; // end of class SSAORenderingPipeline

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H
