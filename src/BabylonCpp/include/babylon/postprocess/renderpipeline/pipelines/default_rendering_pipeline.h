#ifndef BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocess/depth_of_field_effect_blur_level.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

/**
 * @brief The default rendering pipeline can be added to a scene to apply common
 * post processing effects such as anti-aliasing or depth of field. See
 * https://doc.babylonjs.com/how_to/using_default_rendering_pipeline
 */
class BABYLON_SHARED_EXPORT DefaultRenderingPipeline
    : public PostProcessRenderPipeline {

public:
  /**
   * ID of the sharpen post process,
   */
  static constexpr const char* SharpenPostProcessId
    = "SharpenPostProcessEffect";
  /**
   * ID of the pass post process used for bloom,
   */
  static constexpr const char* PassPostProcessId = "PassPostProcessEffect";
  /**
   * ID of the highlight post process used for bloom,
   */
  static constexpr const char* HighLightsPostProcessId
    = "HighLightsPostProcessEffect";
  /**
   * ID of the blurX post process used for bloom,
   */
  static constexpr const char* BlurXPostProcessId = "BlurXPostProcessEffect";
  /**
   * ID of the blurY post process used for bloom,
   */
  static constexpr const char* BlurYPostProcessId = "BlurYPostProcessEffect";
  /**
   * ID of the copy back post process used for bloom,
   */
  static constexpr const char* CopyBackPostProcessId
    = "CopyBackPostProcessEffect";
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
   * ID of the final merge post process;
   */
  static constexpr const char* FinalMergePostProcessId
    = "FinalMergePostProcessEffect";
  /**
   * ID of the chromatic aberration post process,
   */
  static constexpr const char* ChromaticAberrationPostProcessId
    = "ChromaticAberrationPostProcessEffect";

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
   * @brief The strength of the bloom.
   */
  void set_bloomWeight(float value);
  float get_bloomWeight() const;

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
   * @brief If the multisample anti-aliasing is enabled.
   */
  void set_msaaEnabled(bool enabled);
  bool get_msaaEnabled() const;

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
   * First pass of bloom to capture the original image texture for later use.
   */
  PassPostProcess* pass;
  /**
   * Second pass of bloom used to brighten bright portions of the image.
   */
  HighlightsPostProcess* highlights;
  /**
   * BlurX post process used in coordination with blurY to guassian blur the
   * highlighted image.
   */
  BlurPostProcess* blurX;
  /**
   * BlurY post process used in coordination with blurX to guassian blur the
   * highlighted image.
   */
  BlurPostProcess* blurY;
  /**
   * Final pass run for bloom to copy the resulting bloom texture back to
   * screen.
   */
  PassPostProcess* copyBack;
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
   * Final post process to merge results of all previous passes
   */
  PostProcess* finalMerge; // PassPostProcess

  /**
   * Chromatic aberration post process which will shift rgb colors in the image
   */
  ChromaticAberrationPostProcess* chromaticAberration;

  /**
   * Animations which can be used to tweak settings over a period of time
   */
  vector_t<Animation*> animations;

  /**
   * Specifies the size of the bloom blur kernel, relative to the final output
   * size
   */
  float bloomKernel;

  /**
   * Enable or disable the sharpen process from the pipeline
   */
  Property<DefaultRenderingPipeline, bool> sharpenEnabled;

  /**
   * The strength of the bloom.
   */
  Property<DefaultRenderingPipeline, float> bloomWeight;

  /**
   * The scale of the bloom, lower value will provide better performance.
   */
  Property<DefaultRenderingPipeline, float> bloomScale;

  /**
   * Enable or disable the bloom from the pipeline.
   */
  Property<DefaultRenderingPipeline, bool> bloomEnabled;

  /**
   * If the depth of field is enabled.
   */
  Property<DefaultRenderingPipeline, bool> depthOfFieldEnabled;

  /**
   * Blur level of the depth of field effect. (Higher blur will effect
   * performance).
   */
  Property<DefaultRenderingPipeline, DepthOfFieldEffectBlurLevel>
    depthOfFieldBlurLevel;

  /**
   * If the anti aliasing is enabled.
   */
  Property<DefaultRenderingPipeline, bool> fxaaEnabled;

  /**
   * If the multisample anti-aliasing is enabled.
   */
  Property<DefaultRenderingPipeline, bool> msaaEnabled;

  /**
   * If image processing is enabled.
   */
  Property<DefaultRenderingPipeline, bool> imageProcessingEnabled;

  /**
   * Enable or disable the chromaticAberration process from the pipeline
   */
  Property<DefaultRenderingPipeline, bool> chromaticAberrationEnabled;

private:
  // Post-processes
  PostProcessRenderEffect* _sharpenEffect;
  PostProcessRenderEffect* _chromaticAberrationEffect;

  // Values
  bool _sharpenEnabled;
  bool _bloomEnabled;
  bool _depthOfFieldEnabled;
  DepthOfFieldEffectBlurLevel _depthOfFieldBlurLevel;
  bool _fxaaEnabled;
  bool _msaaEnabled;
  bool _imageProcessingEnabled;
  unsigned int _defaultPipelineTextureType;
  float _bloomScale;
  bool _chromaticAberrationEnabled;
  bool _buildAllowed;

  /**
   * Specifies the weight of the bloom in the final rendering
   */
  float _bloomWeight;

  bool _hdr;
  Scene* _scene;
  vector_t<Camera*> _originalCameras;

  PostProcess* _prevPostProcess;
  PostProcess* _prevPrevPostProcess;

}; // end of class SSAORenderingPipeline

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H
