#ifndef BABYLON_LOADING_PLUGINS_GLTF_2_0_GLTF_LOADER_TRACKER_H
#define BABYLON_LOADING_PLUGINS_GLTF_2_0_GLTF_LOADER_TRACKER_H

#include <functional>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace GLTF2 {

class BABYLON_SHARED_EXPORT GLTFLoaderTracker {

public:
  GLTFLoaderTracker(const std::function<void()>& onComplete);
  ~GLTFLoaderTracker();

  void _addPendingData();
  void _removePendingData();

private:
  int _pendingCount;
  std::function<void()> _callback;

}; // end of struct GLTFUtils

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_GLTF_2_0_GLTF_LOADER_TRACKER_H
