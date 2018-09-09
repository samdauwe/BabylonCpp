#ifndef BABYLON_AUDIO_SOUND_H
#define BABYLON_AUDIO_SOUND_H

#include <babylon/babylon_api.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Sound {

public:
  bool isReady()
  {
    return false;
  }
  void play(std::optional<size_t> /*time*/   = std::nullopt,
            std::optional<size_t> /*offset*/ = std::nullopt)
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
  std::string name;
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
