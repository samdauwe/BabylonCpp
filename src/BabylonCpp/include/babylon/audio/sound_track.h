#ifndef BABYLON_AUDIO_SOUND_TRACK_H
#define BABYLON_AUDIO_SOUND_TRACK_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Scene;
class Sound;
using SoundPtr = std::shared_ptr<Sound>;

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
