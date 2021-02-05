#ifndef BABYLON_POSTPROCESSES_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESSES_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>
#include <babylon/postprocesses/depth_of_field_effect_blur_level.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline.h>

using json = nlohmann::json;

namespace BABYLON {

class Animation;
class ImageProcessingConfiguration;
class Scene;
FWD_CLASS_SPTR(BloomEffect)
FWD_CLASS_SPTR(ChromaticAberrationPostProcess)
FWD_CLASS_SPTR(DefaultRenderingPipeline)
FWD_CLASS_SPTR(DepthOfFieldEffect)
FWD_CLASS_SPTR(FxaaPostProcess)
FWD_CLASS_SPTR(GlowLayer)
FWD_CLASS_SPTR(GrainPostProcess)
FWD_CLASS_SPTR(PostProcess)
FWD_CLASS_SPTR(SharpenPostProcess)

/**
 * @brief The default rendering pipeline can be added to a scene to apply common
 * post processing effects such as anti-aliasing or depth of field. See
 * https://doc.babylonjs.com/how_to/using_default_rendering_pipeline
 */
class BABYLON_SHARED_EXPORT DefaultRenderingPipeline : public PostProcessRenderPipeline {

private:
  /**
   * ID of the sharpen post process,
   */
  static constexpr const char* SharpenPostProcessId = "SharpenPostProcessEffect";

  /**
   * ID of the image processing post process;
   */
  static constexpr const char* ImageProcessingPostProcessId = "ImageProcessingPostProcessEffect";

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
  template <typename... Ts>
  static DefaultRenderingPipelinePtr New(Ts&&... args)
  {
    auto renderingPipeline = std::shared_ptr<DefaultRenderingPipeline>(
      new DefaultRenderingPipeline(std::forward<Ts>(args)...));
    renderingPipeline->addToScene(renderingPipeline);

    return renderingPipeline;
  }
  ~DefaultRenderingPipeline() override; // = default

  void addToScene(const DefaultRenderingPipelinePtr& renderingPipeline);

  /**
   * @brief Gets active scene.
   */
  [[nodiscard]] Scene* scene() const;

  /**
   * @brief Get the class name.
   * @returns "DefaultRenderingPipeline"
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Force the compilation of the entire pipeline.
   */
  void prepare();

  /**
   * @brief Adds a camera to the pipeline.
   * @param camera the camera to be added
   */
  void addCamera(Camera* camera);

  /**
   * @brief Removes a camera from the pipeline.
   * @param camera the camera to remove
   */
  void removeCamera(Camera* camera);

  /**
   * @brief Dispose of the pipeline and stop all post processes.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Serialize the rendering pipeline (Used when exporting)
   * @returns the serialized object
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Parse the serialized pipeline
   * @param source Source pipeline.
   * @param scene The scene to load the pipeline to.
   * @param rootUrl The URL of the serialized pipeline.
   * @returns An instantiated pipeline from the serialized object.
   */
  static std::unique_ptr<DefaultRenderingPipeline> Parse(const json& source, Scene* scene,
                                                         const std::string& rootUrl);

protected:
  /**
   * @brief constructor
   * @param name - The rendering pipeline name (default: "")
   * @param hdr - If high dynamic range textures should be used (default: true)
   * @param scene - The scene linked to this pipeline (default: the last created
   * scene)
   * @param cameras - The array of cameras that the rendering pipeline will be
   * attached to (default: scene.cameras)
   * @param automaticBuild - if false, you will have to manually call prepare()
   * to update the pipeline (default: true)
   */
  DefaultRenderingPipeline(const std::string& name = "", bool hdr = true, Scene* scene = nullptr,
                           const std::unordered_map<std::string, CameraPtr>& cameras = {},
                           bool automaticBuild                                       = true);

private:
  /**
   * @brief Enable or disable the sharpen process from the pipeline.
   */
  void set_sharpenEnabled(bool enabled);
  [[nodiscard]] bool get_sharpenEnabled() const;

  /**
   * @brief Specifies the size of the bloom blur kernel, relative to the final
   * output size.
   */
  void set_bloomKernel(float value);
  [[nodiscard]] float get_bloomKernel() const;

  /**
   * @brief The strength of the bloom.
   */
  void set_bloomWeight(float value);
  [[nodiscard]] float get_bloomWeight() const;

  /**
   * @brief The threshold of the bloom.
   */
  void set_bloomThreshold(float value);
  [[nodiscard]] float get_bloomThreshold() const;

  /**
   * @brief The scale of the bloom, lower value will provide better performance.
   */
  void set_bloomScale(float value);
  [[nodiscard]] float get_bloomScale() const;

  /**
   * @brief Enable or disable the bloom from the pipeline.
   */
  void set_bloomEnabled(bool enabled);
  [[nodiscard]] bool get_bloomEnabled() const;

  /**
   * @brief If the depth of field is enabled.
   */
  void set_depthOfFieldEnabled(bool enabled);
  [[nodiscard]] bool get_depthOfFieldEnabled() const;

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
  [[nodiscard]] bool get_fxaaEnabled() const;

  /**
   * @brief MSAA sample count, setting this to 4 will provide 4x anti aliasing.
   * (default: 1)
   */
  void set_samples(unsigned int sampleCount);
  [[nodiscard]] unsigned int get_samples() const;

  /**
   * @brief If image processing is enabled.
   */
  void set_imageProcessingEnabled(bool enabled);
  [[nodiscard]] bool get_imageProcessingEnabled() const;

  /**
   * @brief If glow layer is enabled. (Adds a glow effect to emmissive
   * materials)
   */
  void set_glowLayerEnabled(bool enabled);
  [[nodiscard]] bool get_glowLayerEnabled() const;

  /**
   * @brief Gets the glow layer (or null if not defined).
   */
  GlowLayerPtr& get_glowLayer();

  /**
   * @brief Enable or disable the chromaticAberration process from the pipeline.
   */
  void set_chromaticAberrationEnabled(bool enabled);
  [[nodiscard]] bool get_chromaticAberrationEnabled() const;

  /**
   * @brief Enable or disable the grain process from the pipeline.
   */
  void set_grainEnabled(bool enabled);
  [[nodiscard]] bool get_grainEnabled() const;

  void _rebuildBloom();
  void _setAutoClearAndTextureSharing(const PostProcessPtr& postProcess,
                                      bool skipTextureSharing = false);
  void _buildPipeline();
  void _disposePostProcesses(bool disposeNonRecreated = false);

public:
  // Post-processes
  /**
   * Sharpen post process which will apply a sharpen convolution to enhance
   * edges
   */
  SharpenPostProcessPtr sharpen;
  /**
   * Depth of field effect, applies a blur based on how far away objects are
   * from the focus distance.
   */
  DepthOfFieldEffectPtr depthOfField;
  /**
   * The Fast Approximate Anti-Aliasing post process which attempts to remove aliasing from an
   * image.
   */
  FxaaPostProcessPtr fxaa;
  /**
   * Image post processing pass used to perform operations such as tone mapping
   * or color grading.
   */
  PostProcessPtr imageProcessing; // ImageProcessingPostProcess

  /**
   * Chromatic aberration post process which will shift rgb colors in the image
   */
  ChromaticAberrationPostProcessPtr chromaticAberration;

  /**
   * Grain post process which add noise to the image
   */
  GrainPostProcessPtr grain;

  /**
   * Animations which can be used to tweak settings over a period of time
   */
  std::vector<Animation*> animations;

  /**
   * This is triggered each time the pipeline has been built.
   */
  Observable<DefaultRenderingPipeline> onBuildObservable;

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
  Property<DefaultRenderingPipeline, DepthOfFieldEffectBlurLevel> depthOfFieldBlurLevel;

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
   * If glow layer is enabled. (Adds a glow effect to emmissive materials)
   */
  Property<DefaultRenderingPipeline, bool> glowLayerEnabled;

  /**
   * Gets the glow layer (or null if not defined)
   */
  ReadOnlyProperty<DefaultRenderingPipeline, GlowLayerPtr> glowLayer;

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
  PostProcessRenderEffectPtr _sharpenEffect;
  BloomEffectPtr bloom;
  PostProcessRenderEffectPtr _chromaticAberrationEffect;
  PostProcessRenderEffectPtr _grainEffect;

  /**
   * Glow post process which adds a glow to emissive areas of the image
   */
  GlowLayerPtr _glowLayer;

  Observer<ImageProcessingConfiguration>::Ptr _imageProcessingConfigurationObserver;

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
  std::vector<CameraPtr> _camerasToBeAttached;

  bool _hasCleared;
  PostProcessPtr _prevPostProcess;
  PostProcessPtr _prevPrevPostProcess;

  Observer<Scene>::Ptr _depthOfFieldSceneObserver;

}; // end of class SSAORenderingPipeline

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESSES_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H
