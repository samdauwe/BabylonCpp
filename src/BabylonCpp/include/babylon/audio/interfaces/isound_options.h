#ifndef BABYLON_AUDIO_INTERFACES_ISOUND_OPTIONS_H
#define BABYLON_AUDIO_INTERFACES_ISOUND_OPTIONS_H

#include <optional>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Interface used to define options for Sound class.
 */
struct BABYLON_SHARED_EXPORT ISoundOptions {
  /**
   * Does the sound autoplay once loaded.
   */
  std::optional<bool> autoplay = std::nullopt;
  /**
   * Does the sound loop after it finishes playing once.
   */
  std::optional<bool> loop = std::nullopt;
  /**
   * Sound's volume
   */
  std::optional<float> volume = std::nullopt;
  /**
   * Is it a spatial sound?
   */
  std::optional<bool> spatialSound = std::nullopt;
  /**
   * Maximum distance to hear that sound
   */
  std::optional<float> maxDistance = std::nullopt;
  /**
   * Uses user defined attenuation function
   */
  std::optional<bool> useCustomAttenuation = std::nullopt;
  /**
   * Define the roll off factor of spatial sounds.
   * @see https://doc.babylonjs.com/how_to/playing_sounds_and_music#creating-a-spatial-3d-sound
   */
  std::optional<float> rolloffFactor = std::nullopt;
  /**
   * Define the reference distance the sound should be heard perfectly.
   * @see https://doc.babylonjs.com/how_to/playing_sounds_and_music#creating-a-spatial-3d-sound
   */
  std::optional<float> refDistance = std::nullopt;
  /**
   * Define the distance attenuation model the sound will follow.
   * @see https://doc.babylonjs.com/how_to/playing_sounds_and_music#creating-a-spatial-3d-sound
   */
  std::string distanceModel;
  /**
   * Defines the playback speed (1 by default)
   */
  std::optional<float> playbackRate = std::nullopt;
  /**
   * Defines if the sound is from a streaming source
   */
  std::optional<bool> streaming = std::nullopt;
  /**
   * Defines an optional length (in seconds) inside the sound file
   */
  std::optional<float> length = std::nullopt;
  /**
   * Defines an optional offset (in seconds) inside the sound file
   */
  std::optional<float> offset = std::nullopt;
  /**
   * If true, URLs will not be required to state the audio file codec to use.
   */
  std::optional<bool> skipCodecCheck = std::nullopt;
}; // end of class ISoundOptions

} // end of namespace BABYLON

#endif // end of BABYLON_AUDIO_INTERFACES_ISOUND_OPTIONS_H
