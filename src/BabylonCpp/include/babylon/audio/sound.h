#ifndef BABYLON_AUDIO_SOUND_H
#define BABYLON_AUDIO_SOUND_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Sound {

public:
  bool isReady()
  {
    return false;
  }
  void play(nullable_t<size_t> /*time*/   = nullopt_t,
            nullable_t<size_t> /*offset*/ = nullopt_t)
  {
  }
  void pause()
  {
  }
  void stop()
  {
  }
  void setVolume(unsigned int /*volume*/)
  {
  }

public:
  string_t name;
  bool autoplay;
  float directionalConeInnerAngle;
  float directionalConeOuterAngle;

  /**
   * Observable event when the current playing sound finishes.
   */
  Observable<Sound> onEndedObservable;

}; // end of class Sound

} // end of namespace BABYLON

#endif // end of BABYLON_AUDIO_SOUND_H
