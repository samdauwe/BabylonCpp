#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_RESERVED_DATA_STORAGE_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_RESERVED_DATA_STORAGE_H

#include <memory>

#include <babylon/animations/animation_range.h>
#include <babylon/babylon_api.h>
#include <babylon/engines/scene.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT AnimationControl {
  float from = 0.f;
  float to   = 0.f;
  bool loop  = false;
}; // end of struct AnimationControl

struct BABYLON_SHARED_EXPORT AnimationReservedDataStore {
  bool intialized    = false;
  Scene* scene       = nullptr;
  float currentFrame = 0.f;
  std::vector<AnimationPtr> _animations;
  std::vector<AnimationRangePtr> _ranges;
  AnimationControl _animationControl;
  AnimatablePtr _runningAnimatable             = nullptr;
  Observer<Scene>::Ptr _onBeforeRenderObserver = nullptr;
  bool _isPlaying                              = false;
}; // end of struct AnimationReservedDataStore

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_RESERVED_DATA_STORAGE_H
