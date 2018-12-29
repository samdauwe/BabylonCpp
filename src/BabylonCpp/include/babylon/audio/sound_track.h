#ifndef BABYLON_AUDIO_SOUND_TRACK_H
#define BABYLON_AUDIO_SOUND_TRACK_H

#include <memory>
#include <vector>

#include <babylon/audio/isound_track_options.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class Scene;
class Sound;
class SoundTrack;
using SoundPtr      = std::shared_ptr<Sound>;
using SoundTrackPtr = std::shared_ptr<SoundTrack>;

class BABYLON_SHARED_EXPORT SoundTrack {

public:
  template <typename... Ts>
  static SoundTrackPtr New(Ts&&... args)
  {
    return std::shared_ptr<SoundTrack>(
      new SoundTrack(std::forward<Ts>(args)...));
  }
  void AddSound(const SoundPtr& /*sound*/)
  {
  }
  void RemoveSound(const SoundPtr& /*sound*/)
  {
  }
  void switchPanningModelToHRTF()
  {
  }
  void switchPanningModelToEqualPower()
  {
  }
  void dispose()
  {
  }

protected:
  /**
   * @brief Creates a new sound track.
   * @see
   * http://doc.babylonjs.com/how_to/playing_sounds_and_music#using-sound-tracks
   * @param scene Define the scene the sound track belongs to
   * @param options
   */
  SoundTrack(Scene* /*scene*/, const ISoundTrackOptions& /*options*/ = {})
  {
  }

public:
  Scene* _scene;
  bool _mainTrack;
  std::vector<SoundPtr> soundCollection;

}; // end of class SoundTrack

} // end of namespace BABYLON

#endif // end of BABYLON_AUDIO_SOUND_TRACK_H
