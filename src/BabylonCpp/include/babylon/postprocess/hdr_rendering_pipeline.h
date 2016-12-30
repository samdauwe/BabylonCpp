#ifndef BABYLON_POSTPROCESS_HDR_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESS_HDR_RENDERING_PIPLINE_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT HDRRenderingPipeline
  : public PostProcessRenderPipeline,
    public IDisposable {

public:
  // Luminance steps
  static constexpr unsigned int LUM_STEPS = 6;

public:
  /**
   * Constructor
   * @param {string} name - The rendering pipeline name
   * @param {BABYLON.Scene} scene - The scene linked to this pipeline
   * @param {any} ratio - The size of the postprocesses (0.5 means that your
   * postprocess will have a width = canvas.width 0.5 and a height =
   * canvas.height 0.5)
   * @param {BABYLON.PostProcess} originalPostProcess - the custom original
   * color post-process. Must be "reusable". Can be null.
   * @param {BABYLON.Camera[]} cameras - The array of cameras that the rendering
   * pipeline will be attached to
   */
  HDRRenderingPipeline(const std::string& name, Scene* scene, float ratio,
                       PostProcess* originalPostProcess,
                       const std::vector<Camera*>& cameras);
  virtual ~HDRRenderingPipeline();

  /**
   * Tells the pipeline to update its post-processes
   */
  void update();

  /**
   * Returns the current calculated luminance
   */
  float getCurrentLuminance() const;

  /**
   * Returns the currently drawn luminance
   */
  float getOutputLuminance() const;

  /**
   * Releases the rendering pipeline and its internal effects. Detaches pipeline
   * from cameras
   */
  void dispose(bool doNotRecurse = false) override;

private:
  /**
   * Creates the HDR post-process and computes the luminance adaptation
   */
  void _createHDRPostProcess(Scene* scene, float ratio);

  /**
   * Texture Adder post-process
   */
  void _createTextureAdderPostProcess(Scene* scene, float ratio);

  /**
   * Down sample X4 post-process
   */
  void _createDownSampleX4PostProcess(Scene* scene, float ratio);

  /**
   * Bright pass post-process
   */
  void _createBrightPassPostProcess(Scene* scene, float ratio);

  /**
   * Luminance generator. Creates the luminance post-process and down sample
   * post-processes
   */
  void _createLuminanceGeneratorPostProcess(Scene* scene);

  /**
   * Gaussian blur post-processes. Horizontal and Vertical
   */
  void _createGaussianBlurPostProcess(Scene* scene, float ratio);

public:
  //** Gaussian Blur **/ /
  // Gaussian blur coefficient
  float gaussCoeff;
  // Gaussian blur mean
  float gaussMean;
  // Gaussian blur standard deviation
  float gaussStandDev;
  // Gaussian blur multiplier. Multiplies the blur effect
  float gaussMultiplier;

  //** HDR **/ /
  // Exposure, controls the overall intensity of the pipeline
  float exposure;
  // Minimum luminance that the post-process can output. Luminance is >= 0
  float minimumLuminance;
  // Maximum luminance that the post-process can output. Must be suprerior to
  // minimumLuminance
  float maximumLuminance;
  // Increase rate for luminance: eye adaptation speed to dark
  float luminanceIncreaserate;
  // Decrease rate for luminance: eye adaptation speed to bright
  float luminanceDecreaseRate;

  //** Bright pass **/ /
  // Minimum luminance needed to compute HDR
  float brightThreshold;

public:
  Scene* _scene;

private:
  // Gaussian blur
  PostProcess* _guassianBlurHPostProcess;
  PostProcess* _guassianBlurVPostProcess;
  // Bright pass
  PostProcess* _brightPassPostProcess;
  // Texture adder
  PostProcess* _textureAdderPostProcess;
  // Down Sampling
  PostProcess* _downSampleX4PostProcess;
  // Original Post-process
  PostProcess* _originalPostProcess;
  // HDR
  PostProcess* _hdrPostProcess;
  float _hdrCurrentLuminance;
  float _hdrOutputLuminance;
  // Luminance generator
  std::vector<PostProcess*> _downSamplePostProcesses;
  // Global
  bool _needUpdate;

}; // end of class HDRRenderingPipeline

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_HDR_RENDERING_PIPLINE_H
