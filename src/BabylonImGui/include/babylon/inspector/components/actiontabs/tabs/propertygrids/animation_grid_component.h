#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_GRID_COMPONENT_H

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation_properties_override.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/inspector/components/actiontabs/lines/button_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/animation_reserved_data_store.h>

namespace BABYLON {

class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

struct BABYLON_SHARED_EXPORT AnimationGridComponent {

  static void
  componentDidMount(AnimationReservedDataStore& animationReservedDataStore)
  {
    animationReservedDataStore._onBeforeRenderObserver
      = animationReservedDataStore.scene->onBeforeRenderObservable.add(
        [&animationReservedDataStore](Scene* /*scene*/,
                                      EventState & /*es*/) -> void {
          if (!animationReservedDataStore._isPlaying
              || !animationReservedDataStore._runningAnimatable) {
            return;
          }
          animationReservedDataStore.currentFrame
            = animationReservedDataStore._runningAnimatable->masterFrame();
        });
  }

  static void
  componentWillUnmount(AnimationReservedDataStore& animationReservedDataStore)
  {
    if (animationReservedDataStore._onBeforeRenderObserver) {
      animationReservedDataStore.scene->onBeforeRenderObservable.remove(
        animationReservedDataStore._onBeforeRenderObserver);
      animationReservedDataStore._onBeforeRenderObserver = nullptr;
    }
  }

  static void
  initializeDatastore(const IAnimatablePtr& animatable,
                      AnimationReservedDataStore& animationReservedDataStore)
  {
    animationReservedDataStore.currentFrame = 0.f;

    animationReservedDataStore._ranges = animatable->getAnimationRanges();
    auto animatables                   = animatable->getAnimatables();
    auto& _animations                  = animationReservedDataStore._animations;
    _animations.clear();
    if (!animatables.empty()) {
      for (const auto& _animatable : animatables) {
        stl_util::concat(_animations, _animatable->getAnimations());
      }

      // Extract from and to
      auto& _animationControl = animationReservedDataStore._animationControl;
      if (!_animations.empty()) {
        for (const auto& animation : _animations) {
          const auto& keys = animation->getKeys();

          if (!keys.empty()) {
            if (keys[0].frame < _animationControl.from) {
              _animationControl.from = keys[0].frame;
            }
            auto lastKeyIndex = keys.size() - 1;
            if (keys[lastKeyIndex].frame > _animationControl.to) {
              _animationControl.to = keys[lastKeyIndex].frame;
            }
          }
        }
      }
    }

    componentDidMount(animationReservedDataStore);
  }

  static void
  playOrPause(const IAnimatablePtr& animatable,
              AnimationReservedDataStore& animationReservedDataStore)
  {
    animationReservedDataStore._isPlaying
      = animationReservedDataStore.scene->getAllAnimatablesByTarget(animatable)
          .size()
        > 0;

    if (animationReservedDataStore._isPlaying) {
      animationReservedDataStore.scene->stopAnimation(animatable);
      animationReservedDataStore._runningAnimatable = nullptr;
    }
    else {
      animationReservedDataStore._runningAnimatable
        = animationReservedDataStore.scene->beginAnimation(
          animatable, animationReservedDataStore._animationControl.from,
          animationReservedDataStore._animationControl.to,
          animationReservedDataStore._animationControl.loop);
    }
  }

  static void
  onCurrentFrameChange(float value,
                       AnimationReservedDataStore& animationReservedDataStore)
  {
    if (!animationReservedDataStore._runningAnimatable) {
      return;
    }

    animationReservedDataStore._runningAnimatable->goToFrame(value);
    animationReservedDataStore.currentFrame = value;
  }

  static void render(const IAnimatablePtr& animatable,
                     AnimationReservedDataStore& animationReservedDataStore)
  {
    if (!animationReservedDataStore.intialized) {
      initializeDatastore(animatable, animationReservedDataStore);
      animationReservedDataStore.intialized = true;
    }

    auto animatablesForTarget
      = animationReservedDataStore.scene->getAllAnimatablesByTarget(animatable);
    animationReservedDataStore._isPlaying = animatablesForTarget.size() > 0;

    auto& _animations        = animationReservedDataStore._animations;
    auto& _animationControl  = animationReservedDataStore._animationControl;
    auto& _isPlaying         = animationReservedDataStore._isPlaying;
    auto& _ranges            = animationReservedDataStore._ranges;
    auto& _runningAnimatable = animationReservedDataStore._runningAnimatable;

    if (_isPlaying && !_runningAnimatable) {
      animationReservedDataStore._runningAnimatable = animatablesForTarget[0];
    }

    if (_runningAnimatable) {
      _animationControl.from = _runningAnimatable->fromFrame;
      _animationControl.to   = _runningAnimatable->toFrame;
      _animationControl.loop = _runningAnimatable->loopAnimation;
    }

    if (_ranges.size() > 0 || _animations.size() > 0) {
      // --- ANIMATION OVERRIDE ---
      static auto animationOverrideContainerOpened = true;
      ImGui::SetNextTreeNodeOpen(animationOverrideContainerOpened,
                                 ImGuiCond_Always);
      if (ImGui::CollapsingHeader("ANIMATION OVERRIDE")) {
        const auto enableOverride
          = animatable->animationPropertiesOverride() != nullptr;
        if (CheckBoxLineComponent::render("Enable override", enableOverride)) {
          if (!enableOverride) {
            animatable->animationPropertiesOverride
              = std::make_shared<AnimationPropertiesOverride>();
            animatable->animationPropertiesOverride()->blendingSpeed = 0.05f;
          }
          else {
            animatable->animationPropertiesOverride = nullptr;
          }
        }
        if (animatable->animationPropertiesOverride() != nullptr) {
          if (CheckBoxLineComponent::render(
                "Enable blending",
                animatable->animationPropertiesOverride()->enableBlending)) {
            animatable->animationPropertiesOverride()->enableBlending
              = !animatable->animationPropertiesOverride()->enableBlending;
          }
          auto sliderChange = SliderLineComponent::render(
            "Blending speed",
            animatable->animationPropertiesOverride()->blendingSpeed, 0.f, 0.1f,
            0.01f, "%.2f");
          if (sliderChange) {
            animatable->animationPropertiesOverride()->blendingSpeed
              = sliderChange.value();
          }
        }
        animationOverrideContainerOpened = true;
      }
      else {
        animationOverrideContainerOpened = false;
      }
    }

    if (_ranges.size() > 0) {
      // --- ANIMATION RANGES ---
      static auto animationRangesContainerOpened = true;
      ImGui::SetNextTreeNodeOpen(animationRangesContainerOpened,
                                 ImGuiCond_Always);
      if (ImGui::CollapsingHeader("ANIMATION RANGES")) {
        for (const auto& range : _ranges) {
          if (ButtonLineComponent::render(range->name.c_str())) {
            _runningAnimatable = nullptr;
            animationReservedDataStore.scene->beginAnimation(
              animatable, range->from, range->to, true);
          }
        }
        animationRangesContainerOpened = true;
      }
      else {
        animationRangesContainerOpened = false;
      }
    }

    if (_animations.size() > 0) {
      // --- ANIMATIONS ---
      static auto animationsContainerOpened = true;
      ImGui::SetNextTreeNodeOpen(animationsContainerOpened, ImGuiCond_Always);
      if (ImGui::CollapsingHeader("ANIMATIONS")) {
        TextLineComponent::render("Count", std::to_string(_animations.size()));
        FloatLineComponent::render("From", _animationControl.from);
        FloatLineComponent::render("To", _animationControl.to);
        if (CheckBoxLineComponent::render("Loop", _animationControl.loop)) {
          _animationControl.loop = !_animationControl.loop;
        }
        if (ButtonLineComponent::render(_isPlaying ? "Stop" : "Play")) {
          playOrPause(animatable, animationReservedDataStore);
        }
        if (_isPlaying) {
          auto sliderChange = SliderLineComponent::render(
            "Current frame", animationReservedDataStore.currentFrame,
            _animationControl.from, _animationControl.to,
            (_animationControl.to - _animationControl.from), "%.4f");
          if (sliderChange) {
            onCurrentFrameChange(sliderChange.value(),
                                 animationReservedDataStore);
          }
        }
        animationsContainerOpened = true;
      }
      else {
        animationsContainerOpened = false;
      }
    }
  }

}; // end of struct AnimationGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_ANIMATION_GRID_COMPONENT_H
