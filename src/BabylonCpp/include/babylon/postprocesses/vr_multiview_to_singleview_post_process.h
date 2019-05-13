#ifndef BABYLON_POSTPROCESSES_VR_MULTIVIEW_TO_SINGLEVIEW_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_VR_MULTIVIEW_TO_SINGLEVIEW_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

/**
 * @brief VRMultiviewToSingleview used to convert multiview texture arrays to
 * standard textures for scenarios such as webVR This will not be used for webXR
 * as it supports displaying texture arrays directly.
 */
class BABYLON_SHARED_EXPORT VRMultiviewToSingleviewPostProcess
    : public PostProcess {

public:
  /**
   * @brief Initializes a VRMultiviewToSingleview.
   * @param name name of the post process
   * @param camera camera to be applied to
   * @param scaleFactor scaling factor to the size of the output texture
   */
  VRMultiviewToSingleviewPostProcess(const std::string& name,
                                     const CameraPtr& camera,
                                     float scaleFactor);
  ~VRMultiviewToSingleviewPostProcess();

}; // end of class VRDistortionCorrectionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_VR_MULTIVIEW_TO_SINGLEVIEW_POST_PROCESS_H
