#ifndef BABYLON_POSTPROCESSES_MOTION_BLUR_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_MOTION_BLUR_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class AbstractMesh;
class GeometryBufferRenderer;
class MotionBlurPostProcess;
using AbstractMeshPtr           = std::shared_ptr<AbstractMesh>;
using GeometryBufferRendererPtr = std::shared_ptr<GeometryBufferRenderer>;
using MotionBlurPostProcessPtr  = std::shared_ptr<MotionBlurPostProcess>;

/**
 * @brief The Motion Blur Post Process which blurs an image based on the objects
 * velocity in scene. Velocity can be affected by each object's rotation,
 * position and scale depending on the transformation speed.
 *
 * As an example, all you have to do is to create the post-process:
 *  var mb = new BABYLON.MotionBlurPostProcess(
 *      'mb',  // The name of the effect.
 *      scene, // The scene containing the objects to blur according to their
 *             //velocity.
 *      1.0,   // The required width/height ratio to downsize to before
 *             // computing the render pass.
 *      camera // The camera to apply the render pass to.
 * );
 *
 * Then, all objects moving, rotating and/or scaling will be blurred depending
 * on the transformation speed.
 */
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
   * @brief Excludes the given skinned mesh from computing bones velocities.
   * Computing bones velocities can have a cost and that cost. The cost can be
   * saved by calling this function and by passing the skinned mesh reference to
   * ignore.
   * @param skinnedMesh The mesh containing the skeleton to ignore when
   * computing the velocity map.
   */
  void excludeSkinnedMesh(const AbstractMeshPtr& skinnedMesh);

  /**
   * @brief Removes the given skinned mesh from the excluded meshes to integrate
   * bones velocities while rendering the velocity map.
   * @param skinnedMesh The mesh containing the skeleton that has been ignored
   * previously.
   * @see excludeSkinnedMesh to exclude a skinned mesh from bones velocity
   * computation.
   */
  void removeExcludedSkinnedMesh(const AbstractMeshPtr& skinnedMesh);

  /**
   * @brief Disposes the post process.
   * @param camera The camera to dispose the post process on.
   */
  void dispose(Camera* camera = nullptr) override;

protected:
  /**
   * @brief Creates a new instance MotionBlurPostProcess
   * @param name The name of the effect.
   * @param scene The scene containing the objects to blur according to their
   * velocity.
   * @param options The required width/height ratio to downsize to before
   * computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass.
   * (default: 0)
   * @param engine The engine which the post process will be applied. (default:
   * current engine)
   * @param reusable If the post process can be reused on the same frame.
   * (default: false)
   * @param textureType Type of textures used when performing the post process.
   * (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in
   * the constructor. The updateEffect method can be used to compile the shader
   * at a later time. (default: false)
   */
  MotionBlurPostProcess(const std::string& name, Scene* scene,
                        const std::variant<float, PostProcessOptions>& options,
                        const CameraPtr& camera,
                        unsigned int samplingMode = Constants::TEXTURE_NEAREST_SAMPLINGMODE,
                        Engine* engine = nullptr, bool reusable = false,
                        unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                        bool blockCompilation    = false);

  /**
   * @brief Gets the number of iterations are used for motion blur quality.
   * Default value is equal to 32.
   */
  unsigned int get_motionBlurSamples() const;

  /**
   * @brief Sets the number of iterations to be used for motion blur quality.
   */
  void set_motionBlurSamples(unsigned int samples);

public:
  /**
   * Defines how much the image is blurred by the movement. Default value is
   * equal to 1
   */
  float motionStrength;

  /**
   * The number of iterations to be used for motion blur quality.
   */
  Property<MotionBlurPostProcess, unsigned int> motionBlurSamples;

private:
  unsigned int _motionBlurSamples;
  GeometryBufferRendererPtr _geometryBufferRenderer;

}; // end of class VRDistortionCorrectionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_MOTION_BLUR_POST_PROCESS_H
