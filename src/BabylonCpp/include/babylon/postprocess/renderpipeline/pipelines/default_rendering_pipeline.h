#ifndef BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT DefaultRenderingPipeline
  : public PostProcessRenderPipeline {

public:
  static std::string PassPostProcessId;
  static std::string HighLightsPostProcessId;
  static std::string BlurXPostProcessId;
  static std::string BlurYPostProcessId;
  static std::string CopyBackPostProcessId;
  static std::string ImageProcessingPostProcessId;
  static std::string FxaaPostProcessId;
  static std::string FinalMergePostProcessId;

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
   */
  DefaultRenderingPipeline(
    const std::string& name, bool hdr, Scene* scene,
    const std::unordered_map<std::string, Camera*>& cameras = {});
  ~DefaultRenderingPipeline();

  void setBloomWeight(float value);
  float bloomWeight() const;
  void setBloomScale(float value);
  float bloomScale() const;
  void setBloomEnabled(bool enabled);
  bool bloomEnabled() const;
  void setFxaaEnabled(bool enabled);
  bool fxaaEnabled() const;
  void setImageProcessingEnabled(bool enabled);
  bool imageProcessingEnabled() const;

  // Dispose
  void dispose(bool doNotRecurse = false) override;

  // Serialize rendering pipeline
  Json::object serialize() const;

  // Parse serialized pipeline
  static std::unique_ptr<DefaultRenderingPipeline>
  Parse(const Json::value& source, Scene* scene, const std::string& rootUrl);

private:
  void _buildPipeline();
  void _disposePostProcesses();

public:
  // Post-processes
  PassPostProcess* pass;
  HighlightsPostProcess* highlights;
  BlurPostProcess* blurX;
  BlurPostProcess* blurY;
  PassPostProcess* copyBack;
  FxaaPostProcess* fxaa;
  ImageProcessingPostProcess* imageProcessing;
  PassPostProcess* finalMerge;

  // IAnimatable
  std::vector<Animation*> animations;

  /**
   * Specifies the size of the bloom blur kernel, relative to the final output
   * size
   */
  float bloomKernel;

private:
  bool _bloomEnabled;
  bool _fxaaEnabled;
  bool _imageProcessingEnabled;
  unsigned int _defaultPipelineTextureType;
  float _bloomScale;

  /**
   * Specifies the weight of the bloom in the final rendering
   */
  float _bloomWeight;

  bool _hdr;
  Scene* _scene;

}; // end of class SSAORenderingPipeline

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_DEFAULT_RENDERING_PIPLINE_H
