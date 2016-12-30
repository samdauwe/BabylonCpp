#ifndef BABYLON_ANIMATIONS_ANIMATABLE_H
#define BABYLON_ANIMATIONS_ANIMATABLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Animatable {

public:
  template <typename... Ts>
  static Animatable* New(Ts&&... args);
  ~Animatable();

  /** Methods **/
  std::vector<Animation*>& getAnimations();
  void appendAnimations(IAnimatable* target,
                        const std::vector<Animation*>& animations);
  Animation* getAnimationByTargetProperty(const std::string& property) const;
  void reset();
  void enableBlending(float blendingSpeed);
  void disableBlending();
  void goToFrame(int frame);
  void pause();
  void restart();
  void stop(const std::string& animationName = "");
  bool _animate(const millisecond_t& delay);

protected:
  Animatable(Scene* scene, IAnimatable* target, int fromFrame = 0,
             int toFrame = 100, bool loopAnimation = false,
             float speedRatio                            = 1.f,
             const std::function<void()>& onAnimationEnd = nullptr,
             const std::vector<Animation*>& animations
             = std::vector<Animation*>());

public:
  IAnimatable* target;
  bool animationStarted;
  int fromFrame;
  int toFrame;
  bool loopAnimation;
  float speedRatio;
  std::function<void()> onAnimationEnd;

private:
  millisecond_t _localDelayOffset;
  millisecond_t _pausedDelay;
  std::vector<Animation*> _animations;
  bool _paused;
  Scene* _scene;

}; // end of class Animatable

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATABLE_H
