#ifndef BABYLON_AUDIO_SOUND_TRACK_H
#define BABYLON_AUDIO_SOUND_TRACK_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SoundTrack {

public:
  SoundTrack(Scene* scene, bool mainTrack)
      : _scene{scene}, _mainTrack{mainTrack}
  {
  }
  void AddSound(const SoundPtr& /*sound*/)
  {
  }
  void RemoveSound(const SoundPtr& /*sound*/)
  {
  }

public:
  Scene* _scene;
  bool _mainTrack;

}; // end of class SoundTrack

} // end of namespace BABYLON

#endif // end of BABYLON_AUDIO_SOUND_TRACK_H
