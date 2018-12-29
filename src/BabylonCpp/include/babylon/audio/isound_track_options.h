#ifndef BABYLON_AUDIO_ISOUND_TRACK_OPTIONS_H
#define BABYLON_AUDIO_ISOUND_TRACK_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Options allowed during the creation of a sound track.
 */
struct BABYLON_SHARED_EXPORT ISoundTrackOptions {
  /**
   * The volume the sound track should take during creation
   */
  std::optional<bool> volume = std::nullopt;
  /**
   * Define if the sound track is the main sound track of the scene
   */
  std::optional<bool> mainTrack = std::nullopt;
}; // end of struct ISoundTrackOptions

} // end of namespace BABYLON

#endif // end of BABYLON_AUDIO_ISOUND_TRACK_OPTIONS_H
