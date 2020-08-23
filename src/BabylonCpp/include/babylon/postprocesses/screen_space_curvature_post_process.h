#ifndef BABYLON_POSTPROCESSES_SCREEN_SPACE_CURVATUTE_PROCESS_H
#define BABYLON_POSTPROCESSES_SCREEN_SPACE_CURVATUTE_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class GeometryBufferRenderer;
class ScreenSpaceCurvaturePostProcess;
using GeometryBufferRendererPtr          = std::shared_ptr<GeometryBufferRenderer>;
using ScreenSpaceCurvaturePostProcessPtr = std::shared_ptr<ScreenSpaceCurvaturePostProcess>;

/**
 * @brief The Screen Space curvature effect can help highlighting ridge and valley of a model.
 */
class BABYLON_SHARED_EXPORT ScreenSpaceCurvaturePostProcess : public PostProcess {

public:
  template <typename... Ts>
  static ScreenSpaceCurvaturePostProcessPtr New(Ts&&... args)
  {
    auto postProcess = std::shared_ptr<ScreenSpaceCurvaturePostProcess>(
      new ScreenSpaceCurvaturePostProcess(std::forward<Ts>(args)...));
    postProcess->add(postProcess);

    return postProcess;
  }
  ~ScreenSpaceCurvaturePostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "ScreenSpaceCurvaturePostProcess" string
   */
  std::string getClassName() const override;

  /**
   * @brief Support test.
   */
  static bool IsSupported();

  /**
   * @brief Hidden
   */
  static ScreenSpaceCurvaturePostProcessPtr _Parse(const json& parsedPostProcess,
                                                   const CameraPtr& targetCamera, Scene* scene,
                                                   const std::string& rootUrl);

protected:
  /**
   * @brief Creates a new instance ScreenSpaceCurvaturePostProcess.
   * @param name The name of the effect.
   * @param scene The scene containing the objects to blur according to their velocity.
   * @param options The required width/height ratio to downsize to before computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass. (default: 0)
   * @param engine The engine which the post process will be applied. (default: current engine)
   * @param reusable If the post process can be reused on the same frame. (default: false)
   * @param textureType Type of textures used when performing the post process. (default: 0)
   * @param blockCompilation If compilation of the shader should not be done in the constructor. The
   * updateEffect method can be used to compile the shader at a later time. (default: false)
   */
  ScreenSpaceCurvaturePostProcess(const std::string& name, Scene* scene,
                                  const std::variant<float, PostProcessOptions>& options,
                                  const CameraPtr& camera,
                                  const std::optional<unsigned int>& samplingMode = std::nullopt,
                                  Engine* engine = nullptr, bool reusable = false,
                                  unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                                  bool blockCompilation    = false);

public:
  /**
   * Defines how much ridge the curvature effect displays.
   */
  float ridge;

  /**
   * Defines how much valley the curvature effect displays.
   */
  float valley;

private:
  GeometryBufferRendererPtr _geometryBufferRenderer;

}; // end of class ScreenSpaceCurvaturePostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_SCREEN_SPACE_CURVATUTE_PROCESS_H
