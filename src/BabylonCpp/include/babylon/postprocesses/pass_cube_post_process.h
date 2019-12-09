#ifndef BABYLON_POSTPROCESSES_PASS_CUBE_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_PASS_CUBE_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class PassCubePostProcess;
using PassCubePostProcessPtr = std::shared_ptr<PassCubePostProcess>;

/**
 * @brief PassCubePostProcess which produces an output the same as it's input
 * (which must be a cube texture).
 */
class BABYLON_SHARED_EXPORT PassCubePostProcess : public PostProcess {

public:
  template <typename... Ts>
  static PassCubePostProcessPtr New(Ts&&... args)
  {
    auto postProcess
      = std::shared_ptr<PassCubePostProcess>(new PassCubePostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~PassCubePostProcess() override; // = default

protected:
  /**
   * @brief Creates the PassCubePostProcess.
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
   * @param textureType The type of texture to be used when performing the post
   * processing.
   * @param blockCompilation If compilation of the shader should not be done in
   * the constructor. The updateEffect method can be used to compile the shader
   * at a later time. (default: false)
   */
  PassCubePostProcess(const std::string& name, float ratio, const CameraPtr& camera,
                      const std::optional<unsigned int>& samplingMode = std::nullopt,
                      Engine* engine = nullptr, bool reusable = false,
                      unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                      bool blockCompilation    = false);

  [[nodiscard]] unsigned int get_face() const;
  void set_face(unsigned int value);

public:
  /**
   * Gets or sets the cube face to display.
   *  * 0 is +X
   *  * 1 is -X
   *  * 2 is +Y
   *  * 3 is -Y
   *  * 4 is +Z
   *  * 5 is -Z
   */
  Property<PassCubePostProcess, unsigned int> face;

private:
  unsigned int _face;

}; // end of class PassCubePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_PASS_POST_PROCESS_H
