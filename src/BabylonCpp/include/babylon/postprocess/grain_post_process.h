#ifndef BABYLON_POSTPROCESS_GRAIN_POST_PROCESS_H
#define BABYLON_POSTPROCESS_GRAIN_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief The GrainPostProcess adds noise to the image at mid luminance levels.
 */
class BABYLON_SHARED_EXPORT GrainPostProcess : public PostProcess {

public:
  /**
   * @brief Creates a new instance of @see GrainPostProcess.
   * @param name The name of the effect.
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
  GrainPostProcess(const string_t& name,
                   const Variant<float, PostProcessOptions>& options,
                   const CameraPtr& camera, unsigned int samplingMode,
                   Engine* engine, bool reusable = false,
                   unsigned int textureType
                   = EngineConstants::TEXTURETYPE_UNSIGNED_INT,
                   bool blockCompilation = false);
  ~GrainPostProcess();

public:
  /**
   * The intensity of the grain added (default: 30)
   */
  float intensity;
  /**
   * If the grain should be randomized on every frame
   */
  bool animated;

}; // end of class GrainPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_GRAIN_POST_PROCESS_H
