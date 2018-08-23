#ifndef BABYLON_POSTPROCESS_FILTER_POST_PROCESS_H
#define BABYLON_POSTPROCESS_FILTER_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/math/matrix.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT FilterPostProcess : public PostProcess {

public:
  FilterPostProcess(const string_t& name, const Matrix& kernelMatrix,
                    float ratio, const CameraPtr& camera,
                    unsigned int samplingMode, Engine* engine,
                    bool reusable = false);
  ~FilterPostProcess();

public:
  Matrix kernelMatrix;

}; // end of class FilterPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_FILTER_POST_PROCESS_H
