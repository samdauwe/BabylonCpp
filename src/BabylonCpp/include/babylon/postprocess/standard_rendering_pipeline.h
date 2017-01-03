#ifndef BABYLON_POSTPROCESS_STANDARD_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESS_STANDARD_RENDERING_PIPLINE_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT StandardRenderingPipeline
  : public PostProcessRenderPipeline,
    public IAnimatable {

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
  StandardRenderingPipeline(const std::string& name, Scene* scene, float ratio,
                            PostProcess* originalPostProcess = nullptr,
                            const std::vector<Camera*>& cameras
                            = std::vector<Camera*>());
  virtual ~StandardRenderingPipeline();

  void setDepthOfFieldEnabled(bool enabled);
  bool depthOfFieldEnabled() const;
  void setLensFlareEnabled(bool enabled);
  bool lensFlareEnabled() const;

private:
  // Down Sample X4 Post-Processs
  void _createDownSampleX4PostProcess(Scene* scene, float ratio);
  // Brightpass Post-Process
  void _createBrightPassPostProcess(Scene* scene, float ratio);
  // Create gaussian blur H&V post-processes
  void _createGaussianBlurPostProcesses(Scene* scene, float ratio,
                                        unsigned int indice);
  // Create texture adder post-process
  void _createTextureAdderPostProcess(Scene* scene, float ratio);
  // Create lens flare post-process
  void _createLensFlarePostProcess(Scene* scene, float ratio);
  // Create depth-of-field post-process
  void _createDepthOfFieldPostProcess(Scene* scene, float ratio);
  // Dispose
  void dispose(bool doNotRecurse = false) override;

public:
  // Post-processes
  PostProcess* originalPostProcess;
  PostProcess* downSampleX4PostProcess;
  PostProcess* brightPassPostProcess;
  std::vector<PostProcess*> gaussianBlurHPostProcesses;
  std::vector<PostProcess*> gaussianBlurVPostProcesses;
  PostProcess* textureAdderPostProcess;
  PostProcess* textureAdderFinalPostProcess;
  PostProcess* lensFlarePostProcess;
  PostProcess* lensFlareComposePostProcess;
  PostProcess* depthOfFieldPostProcess;
  // Values
  float brightThreshold;
  float blurWidth;
  float gaussianCoefficient;
  float gaussianMean;
  float gaussianStandardDeviation;
  float exposure;
  Texture* lensTexture;
  Texture* lensColorTexture;
  float lensFlareStrength;
  float lensFlareGhostDispersal;
  float lensFlareHaloWidth;
  float lensFlareDistortionStrength;
  Texture* lensStarTexture;
  Texture* lensFlareDirtTexture;
  float depthOfFieldDistance;
  // IAnimatable
  std::vector<Animation> animations;

private:
  Scene* _scene;
  DepthRenderer* _depthRenderer;
  bool _depthOfFieldEnabled;
  bool _lensFlareEnabled;

}; // end of class StandardRenderingPipeline

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_STANDARD_RENDERING_PIPLINE_H
