#ifndef BABYLON_POSTPROCESSES_RENDER_PIPELINE_PIPELINES_SSAO_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESSES_RENDER_PIPELINE_PIPELINES_SSAO_RENDERING_PIPLINE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(BlurPostProcess)
FWD_CLASS_SPTR(DynamicTexture)
FWD_CLASS_SPTR(PassPostProcess)
FWD_CLASS_SPTR(PostProcess)
FWD_CLASS_SPTR(RenderTargetTexture)
FWD_CLASS_SPTR(SSAORenderingPipeline)

struct SSARatio {
  float ssaoRatio;
  float combineRatio;
}; // end of struct SSARatio

/**
 * @brief Render pipeline to produce ssao effect.
 */
class BABYLON_SHARED_EXPORT SSAORenderingPipeline : public PostProcessRenderPipeline {

public:
  /**
   * The PassPostProcess id in the pipeline that contains the original scene
   * color
   */
  static constexpr const char* SSAOOriginalSceneColorEffect = "SSAOOriginalSceneColorEffect";
  /**
   * The SSAO PostProcess id in the pipeline
   */
  static constexpr const char* SSAORenderEffect = "SSAORenderEffect";
  /**
   * The horizontal blur PostProcess id in the pipeline
   */
  static constexpr const char* SSAOBlurHRenderEffect = "SSAOBlurHRenderEffect";
  /**
   * The vertical blur PostProcess id in the pipeline
   */
  static constexpr const char* SSAOBlurVRenderEffect = "SSAOBlurVRenderEffect";
  /**
   * The PostProcess id in the pipeline that combines the SSAO-Blur output with
   * the original scene color (SSAOOriginalSceneColorEffect)
   */
  static constexpr const char* SSAOCombineRenderEffect = "SSAOCombineRenderEffect";

public:
  template <typename... Ts>
  static SSAORenderingPipelinePtr New(Ts&&... args)
  {
    auto renderingPipeline = std::shared_ptr<SSAORenderingPipeline>(
      new SSAORenderingPipeline(std::forward<Ts>(args)...));
    renderingPipeline->addToScene(renderingPipeline);

    return renderingPipeline;
  }
  ~SSAORenderingPipeline() override; // = default

  /** Methods */
  void addToScene(const SSAORenderingPipelinePtr& ssao2RenderingPipeline);

  /**
   * @brief Gets active scene.
   */
  [[nodiscard]] Scene* scene() const;

  /**
   * @brief Get the class name.
   * @returns "SSAORenderingPipeline"
   */
  [[nodiscard]] std::string getClassName() const override;

  /** Hidden */
  void _rebuild() override;

  /**
   * Removes the internal pipeline assets and detatches the pipeline from the
   * scene cameras
   */
  void dispose(bool disableDepthRender = false, bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Constructor
   * @param name - The rendering pipeline name
   * @param scene - The scene linked to this pipeline
   * @param ratio - The size of the postprocesses. Can be a number shared
   * between passes or an object for more precision: { ssaoRatio: 0.5,
   * combineRatio: 1.0 }
   * @param cameras - The array of cameras that the rendering pipeline will be
   * attached to
   */
  SSAORenderingPipeline(const std::string& name, Scene* scene, float ratio,
                        const std::vector<CameraPtr>& cameras);
  SSAORenderingPipeline(const std::string& name, Scene* scene, const SSARatio& ratio,
                        const std::vector<CameraPtr>& cameras);

private:
  void _createBlurPostProcess(float ratio);
  void _createSSAOPostProcess(float ratio);
  void _createSSAOCombinePostProcess(float ratio);
  void _createRandomTexture();

public:
  /**
   * The output strength of the SSAO post-process. Default value is 1.0.
   */
  float totalStrength;
  /**
   * The radius around the analyzed pixel used by the SSAO post-process. Default
   * value is 0.0001
   */
  float radius;
  /**
   * Related to fallOff, used to interpolate SSAO samples (first interpolate
   * function input) based on the occlusion difference of each pixel
   * Must not be equal to fallOff and superior to fallOff.
   * Default value is 0.0075
   */
  float area;
  /**
   * Related to area, used to interpolate SSAO samples (second interpolate
   * function input) based on the occlusion difference of each pixel
   * Must not be equal to area and inferior to area.
   * Default value is 0.000001
   */
  float fallOff;

  /**
   * The base color of the SSAO post-process
   * The final result is "base + ssao" between [0, 1]
   */
  float base;

private:
  Scene* _scene;
  std::vector<CameraPtr> _cameraList;
  RenderTargetTexturePtr _depthTexture;
  DynamicTexturePtr _randomTexture;

  PassPostProcessPtr _originalColorPostProcess;
  PostProcessPtr _ssaoPostProcess;
  BlurPostProcessPtr _blurHPostProcess;
  BlurPostProcessPtr _blurVPostProcess;
  PostProcessPtr _ssaoCombinePostProcess;

  bool _firstUpdate;

}; // end of class SSAORenderingPipeline

} // end of namespace BABYLON

#endif // end of
       // BABYLON_POSTPROCESSES_RENDER_PIPELINE_PIPELINES_SSAO_RENDERING_PIPLINE_H
