#ifndef BABYLON_POSTPROCESSES_SCREEN_SPACE_REFLECTION_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_SCREEN_SPACE_REFLECTION_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

FWD_CLASS_SPTR(GeometryBufferRenderer)
FWD_CLASS_SPTR(ScreenSpaceReflectionPostProcess)

/**
 * @brief The ScreenSpaceReflectionPostProcess performs realtime reflections using only and only the
 * available informations on the screen (positions and normals). Basically, the screen space
 * reflection post-process will compute reflections according the material's reflectivity.
 */
class BABYLON_SHARED_EXPORT ScreenSpaceReflectionPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static ScreenSpaceReflectionPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<ScreenSpaceReflectionPostProcess>(
      new ScreenSpaceReflectionPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~ScreenSpaceReflectionPostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "ScreenSpaceReflectionPostProcess" string
   */
  std::string getClassName() const override;

  /**
   * @brief Hidden
   */
  static ScreenSpaceReflectionPostProcessPtr _Parse(const json& parsedPostProcess,
                                                    const CameraPtr& targetCamera, Scene* scene,
                                                    const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new instance of ScreenSpaceReflectionPostProcess.
   * @param name The name of the effect.
   * @param scene The scene containing the objects to calculate reflections.
   * @param options The required width/height ratio to downsize to before computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   * @param textureType Type of textures used when performing the post process. (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in the constructor. The
   * updateEffect method can be used to compile the shader at a later time. (default: true)
   * @param forceGeometryBuffer If this post process should use geometry buffer instead of prepass
   * (default: false)
   */
  ScreenSpaceReflectionPostProcess(const std::string& name, Scene* scene, float options,
                                   const CameraPtr& camera, unsigned int samplingMode = 0,
                                   Engine* engine = nullptr, bool reusable = false,
                                   unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                                   bool blockCompilation = false, bool forceGeometryBuffer = false);

  /**
   * @brief Gets whether or not smoothing reflections is enabled.
   * Enabling smoothing will require more GPU power and can generate a drop in FPS.
   */
  bool get_enableSmoothReflections() const;

  /**
   * @brief Sets whether or not smoothing reflections is enabled.
   * Enabling smoothing will require more GPU power and can generate a drop in FPS.
   */
  void set_enableSmoothReflections(bool enabled);

  /**
   * @brief Gets the number of samples taken while computing reflections. More samples count is
   * high, more the post-process wil require GPU power and can generate a drop in FPS. Basically in
   * interval [25, 100].
   */
  unsigned int get_reflectionSamples() const;

  /**
   * @brief Sets the number of samples taken while computing reflections. More samples count is
   * high, more the post-process wil require GPU power and can generate a drop in FPS. Basically in
   * interval [25, 100].
   */
  void set_reflectionSamples(unsigned int samples);

  /**
   * @brief Gets the number of samples taken while smoothing reflections. More samples count is
   * high, more the post-process will require GPU power and can generate a drop in FPS. Default
   * value (5.0) work pretty well in all cases but can be adjusted.
   */
  unsigned int get_smoothSteps() const;

  /**
   * @brief Sets the number of samples taken while smoothing reflections. More samples count is
   * high, more the post-process will require GPU power and can generate a drop in FPS. Default
   * value (5.0) work pretty well in all cases but can be adjusted.
   */
  void set_smoothSteps(unsigned int steps);

private:
  void _updateEffectDefines();

public:
  /**
   * Gets or sets a reflection threshold mainly used to adjust the reflection's height.
   */
  float threshold;

  /**
   * Gets or sets the current reflection strength. 1.0 is an ideal value but can be
   * increased/decreased for particular results.
   */
  float strength;

  /**
   * Gets or sets the falloff exponent used while computing fresnel. More the exponent is high, more
   * the reflections will be discrete.
   */
  float reflectionSpecularFalloffExponent;

  /**
   * Gets or sets the step size used to iterate until the effect finds the color of the reflection's
   * pixel. Typically in interval [0.1, 1.0]
   */
  float step;

  /**
   * Gets or sets the factor applied when computing roughness. Default value is 0.2.
   */
  float roughnessFactor;

  /**
   * Gets or sets whether or not smoothing reflections is enabled.
   * Enabling smoothing will require more GPU power and can generate a drop in FPS.
   */
  Property<ScreenSpaceReflectionPostProcess, bool> enableSmoothReflections;

  /**
   * Gets or sets the number of samples taken while computing reflections. More samples count is
   * high, more the post-process wil require GPU power and can generate a drop in FPS. Basically in
   * interval [25, 100].
   */
  Property<ScreenSpaceReflectionPostProcess, unsigned int> reflectionSamples;

  /**
   * Gets or sets the number of samples taken while smoothing reflections. More samples count is
   * high, more the post-process will require GPU power and can generate a drop in FPS. Default
   * value (5.0) work pretty well in all cases but can be adjusted.
   */
  Property<ScreenSpaceReflectionPostProcess, unsigned int> smoothSteps;

private:
  bool _forceGeometryBuffer;
  GeometryBufferRendererPtr _geometryBufferRenderer;
  PrePassRendererPtr _prePassRenderer;
  bool _enableSmoothReflections;
  unsigned int _reflectionSamples;
  unsigned int _smoothSteps;

}; // end of class ScreenSpaceReflectionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_SCREEN_SPACE_REFLECTION_POST_PROCESS_H
