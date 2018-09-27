#ifndef BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_STANDARD_RENDERING_PIPLINE_H
#define BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_STANDARD_RENDERING_PIPLINE_H

#include <map>

#include <babylon/babylon_api.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class BlurPostProcess;
class Light;
class PostProcess;
class Scene;
class Texture;
using TexturePtr = std::shared_ptr<Texture>;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

class BABYLON_SHARED_EXPORT StandardRenderingPipeline
    : public PostProcessRenderPipeline,
      public IAnimatable {

public:
  // Luminance steps
  static unsigned int LuminanceSteps;

public:
  /**
   * @brief Constructor
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
                            PostProcess* originalPostProcess      = nullptr,
                            const std::vector<CameraPtr>& cameras = {});
  virtual ~StandardRenderingPipeline() override;

  float operator[](const std::string& key) const;

  bool bloomEnabled() const;
  void setBloomEnabled(bool enabled);
  bool depthOfFieldEnabled() const;
  void setDepthOfFieldEnabled(bool enabled);
  bool lensFlareEnabled() const;
  void setLensFlareEnabled(bool enabled);
  bool HDREnabled() const;
  void setHDREnabled(bool enabled);
  bool VLSEnabled() const;
  void setVLSEnabled(bool enabled);
  bool motionBlurEnabled() const;
  void setMotionBlurEnabled(bool enabled);
  float volumetricLightStepsCount() const;
  void setVolumetricLightStepsCount(float count);
  float motionBlurSamples() const;
  void setMotionBlurSamples(float samples);

  /**
   * @brief Dispose of the pipeline and stop all post processes.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Serialize the rendering pipeline (Used when exporting).
   * @returns the serialized object
   */
  Json::object serialize() const;

  /**
   * @brief Parse the serialized pipeline.
   * @param source Source pipeline.
   * @param scene The scene to load the pipeline to.
   * @param rootUrl The URL of the serialized pipeline.
   * @returns An instantiated pipeline from the serialized object.
   */
  static std::unique_ptr<StandardRenderingPipeline>
  Parse(const Json::value& source, Scene* scene, const std::string& url);

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
  PostProcess* originalPostProcess;
  PostProcess* downSampleX4PostProcess;
  PostProcess* brightPassPostProcess;
  std::vector<PostProcess*> blurHPostProcesses;
  std::vector<PostProcess*> blurVPostProcesses;
  PostProcess* textureAdderPostProcess;
  PostProcess* volumetricLightPostProcess;
  BlurPostProcess* volumetricLightSmoothXPostProcess;
  BlurPostProcess* volumetricLightSmoothYPostProcess;
  PostProcess* volumetricLightMergePostProces;
  PostProcess* volumetricLightFinalPostProcess;
  PostProcess* luminancePostProcess;
  std::vector<PostProcess*> luminanceDownSamplePostProcesses;
  PostProcess* hdrPostProcess;
  PostProcess* textureAdderFinalPostProcess;
  PostProcess* lensFlareFinalPostProcess;
  PostProcess* hdrFinalPostProcess;
  PostProcess* lensFlarePostProcess;
  PostProcess* lensFlareComposePostProcess;
  PostProcess* motionBlurPostProcess;
  PostProcess* depthOfFieldPostProcess;
  // Values
  float brightThreshold;
  float blurWidth;
  bool horizontalBlur;
  float exposure;
  TexturePtr lensTexture;
  float volumetricLightCoefficient;
  float volumetricLightPower;
  float volumetricLightBlurScale;
  Light* sourceLight;
  float hdrMinimumLuminance;
  float hdrDecreaseRate;
  float hdrIncreaseRate;
  TexturePtr lensColorTexture;
  float lensFlareStrength;
  float lensFlareGhostDispersal;
  float lensFlareHaloWidth;
  float lensFlareDistortionStrength;
  TexturePtr lensStarTexture;
  TexturePtr lensFlareDirtTexture;
  float depthOfFieldDistance;
  float depthOfFieldBlurWidth;
  float motionStrength;
  // IAnimatable
  std::vector<Animation> animations;

private:
  Scene* _scene;
  PostProcess* _currentDepthOfFieldSource;
  PostProcess* _basePostProcess;
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
  float _motionBlurSamples;
  float _volumetricLightStepsCount;

}; // end of class StandardRenderingPipeline

} // end of namespace BABYLON

#endif // end of
// BABYLON_POSTPROCESS_RENDER_PIPELINE_PIPELINES_STANDARD_RENDERING_PIPLINE_H
