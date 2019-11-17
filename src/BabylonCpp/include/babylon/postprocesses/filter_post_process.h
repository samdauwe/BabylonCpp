#ifndef BABYLON_POSTPROCESSES_FILTER_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_FILTER_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/math/matrix.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

/**
 * @brief Applies a kernel filter to the image.
 */
class BABYLON_SHARED_EXPORT FilterPostProcess : public PostProcess {

public:
  /**
   * @brief Creates a filter post process.
   * @param name The name of the effect.
   * @param kernelMatrix The matrix to be applied to the image
   * @param options The required width/height ratio to downsize to before
   * computing the render pass.
   * @param camera The camera to apply the render pass to.
   * @param samplingMode The sampling mode to be used when computing the pass.
   * (default: 0)
   * @param engine The engine which the post process will be applied. (default:
   * current engine)
   * @param reusable If the post process can be reused on the same frame.
   * (default: false)
   */
  FilterPostProcess(const std::string& name, const Matrix& kernelMatrix, float ratio,
                    const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
                    bool reusable = false);
  ~FilterPostProcess() override; // = default

public:
  /**
   * The matrix to be applied to the image
   */
  Matrix kernelMatrix;

}; // end of class FilterPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_FILTER_POST_PROCESS_H
