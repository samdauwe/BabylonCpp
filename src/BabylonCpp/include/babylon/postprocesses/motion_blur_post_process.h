#ifndef BABYLON_POSTPROCESSES_MOTION_BLUR_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_MOTION_BLUR_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>
#include <babylon/maths/matrix.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(GeometryBufferRenderer)
FWD_CLASS_SPTR(MotionBlurPostProcess)

// clang-format off
/**
 * @brief The Motion Blur Post Process which blurs an image based on the objects velocity in scene.
 * Velocity can be affected by each object's rotation, position and scale depending on the transformation speed.
 * As an example, all you have to do is to create the post-process:
 *  var mb = new BABYLON.MotionBlurPostProcess(
 *      'mb', // The name of the effect.
 *      scene, // The scene containing the objects to blur according to their velocity.
 *      1.0, // The required width/height ratio to downsize to before computing the render pass.
 *      camera // The camera to apply the render pass to.
 * );
 * Then, all objects moving, rotating and/or scaling will be blurred depending on the transformation speed.
 */
// clang-format on
class BABYLON_SHARED_EXPORT MotionBlurPostProcess : public PostProcess {

public:
  template <typename... Ts>
  static MotionBlurPostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<MotionBlurPostProcess>(
      new MotionBlurPostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~MotionBlurPostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "MotionBlurPostProcess" string
   */
  std::string getClassName() const override;

  /**
   * @brief Excludes the given skinned mesh from computing bones velocities.
   * Computing bones velocities can have a cost and that cost. The cost can be saved by calling this
   * function and by passing the skinned mesh reference to ignore.
   * @param skinnedMesh The mesh containing the skeleton to ignore when computing the velocity map.
   */
  void excludeSkinnedMesh(const AbstractMeshPtr& skinnedMesh);

  /**
   * @brief Removes the given skinned mesh from the excluded meshes to integrate bones velocities
   * while rendering the velocity map.
   * @param skinnedMesh The mesh containing the skeleton that has been ignored previously.
   * @see excludeSkinnedMesh to exclude a skinned mesh from bones velocity computation.
   */
  void removeExcludedSkinnedMesh(const AbstractMeshPtr& skinnedMesh);

  /**
   * @brief Disposes the post process.
   * @param camera The camera to dispose the post process on.
   */
  void dispose(Camera* camera = nullptr) override;

  /**
   * @brief Hidden
   */
  static MotionBlurPostProcessPtr _Parse(const json& parsedPostProcess,
                                         const CameraPtr& targetCamera, Scene* scene,
                                         const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new instance MotionBlurPostProcess.
   * @param name The name of the effect.
   * @param scene The scene containing the objects to blur according to their velocity.
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
  MotionBlurPostProcess(const std::string& name, Scene* scene,
                        const std::variant<float, PostProcessOptions>& options,
                        const CameraPtr& camera,
                        const std::optional<unsigned int>& samplingMode = std::nullopt,
                        Engine* engine = nullptr, bool reusable = false,
                        unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                        bool blockCompilation = false, bool forceGeometryBuffer = false);
  /**
   * @brief Gets the number of iterations are used for motion blur quality.
   * Default value is equal to 32.
   */
  unsigned int get_motionBlurSamples() const;

  /**
   * @brief Sets the number of iterations to be used for motion blur quality.
   */
  void set_motionBlurSamples(unsigned int samples);

  /**
   * @brief Gets whether or not the motion blur post-process is in object based mode.
   */
  bool get_isObjectBased() const;

  /**
   * @brief Sets whether or not the motion blur post-process is in object based mode.
   */
  void set_isObjectBased(bool value);

private:
  /**
   * @brief Hidden
   */
  GeometryBufferRendererPtr& get__geometryBufferRenderer();

  /**
   * @brief Hidden
   */
  PrePassRendererPtr& get__prePassRenderer();

  /**
   * @brief Called on the mode changed (object based or screen based).
   */
  void _applyMode();

  /**
   * @brief Called on the effect is applied when the motion blur post-process is in object based
   * mode.
   */
  void _onApplyObjectBased(Effect* effect);

  /**
   * @brief Called on the effect is applied when the motion blur post-process is in screen based
   * mode.
   */
  void _onApplyScreenBased(Effect* effect);

  /**
   * @brief Called on the effect must be updated (changed mode, samples count, etc.).
   */
  void _updateEffect();

public:
  /**
   * Defines how much the image is blurred by the movement. Default value is equal to 1
   */
  float motionStrength;

  /**
   * The number of iterations to be used for motion blur quality.
   */
  Property<MotionBlurPostProcess, unsigned int> motionBlurSamples;

  /**
   * Gets or sets whether or not the motion blur post-process is in object based mode.
   */
  Property<MotionBlurPostProcess, bool> isObjectBased;

private:
  unsigned int _motionBlurSamples;
  bool _isObjectBased;
  bool _forceGeometryBuffer;
  ReadOnlyProperty<MotionBlurPostProcess, GeometryBufferRendererPtr> _geometryBufferRenderer;
  ReadOnlyProperty<MotionBlurPostProcess, PrePassRendererPtr> _prePassRenderer;

  std::optional<Matrix> _invViewProjection;
  std::optional<Matrix> _previousViewProjection;

  GeometryBufferRendererPtr _nullGeometryBufferRenderer;
  PrePassRendererPtr _nullPrePassRenderer;

}; // end of class VRDistortionCorrectionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_MOTION_BLUR_POST_PROCESS_H
