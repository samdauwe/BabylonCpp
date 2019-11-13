#ifndef BABYLON_AUDIO_WEIGHTED_SOUND_H
#define BABYLON_AUDIO_WEIGHTED_SOUND_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Sound;
using SoundPtr = std::shared_ptr<Sound>;

/**
 * @brief Wraps one or more Sound objects and selects one with random weight for
 * playback.
 */
class BABYLON_SHARED_EXPORT WeightedSound {

public:
  /**
   * @brief Creates a new WeightedSound from the list of sounds given.
   * @param loop When true a Sound will be selected and played when the current
   * playing Sound completes.
   * @param sounds Array of Sounds that will be selected from.
   * @param weights Array of number values for selection weights; length must
   * equal sounds, values will be normalized to 1
   */
  WeightedSound(bool loop, const std::vector<SoundPtr>& sounds,
                const Float32Array& weights);
  ~WeightedSound(); // = default

  /**
   * @brief Suspend playback
   */
  void pause();

  /**
   * @brief Stop playback
   */
  void stop();

  /**
   * @brief Start playback.
   * @param startOffset Position the clip head at a specific time in seconds.
   */
  void play(const std::optional<size_t>& startOffset = std::nullopt);

protected:
  /**
   * @brief Gets the size of cone in degrees for a directional sound in which
   * there will be no attenuation.
   */
  float get_directionalConeInnerAngle() const;

  /**
   * @brief Sets the size of cone in degress for a directional sound in which
   * there will be no attenuation.
   */
  void set_directionalConeInnerAngle(float value);

  /**
   * @brief Gets the size of cone in degrees for a directional sound outside of
   * which there will be no sound. Listener angles between innerAngle and
   * outerAngle will falloff linearly.
   */
  float get_directionalConeOuterAngle() const;

  /**
   * @brief Sets the size of cone in degrees for a directional sound outside of
   * which there will be no sound. Listener angles between innerAngle and
   * outerAngle will falloff linearly.
   */
  void set_directionalConeOuterAngle(float value);

  /**
   * @brief Gets the playback volume.
   */
  unsigned int get_volume() const;

  /**
   * @brief Sets the playback volume.
   */
  void set_volume(unsigned int value);

private:
  void _onended();

public:
  /**
   * When true a Sound will be selected and played when the current playing
   * Sound completes
   */
  bool loop;

  /**
   * A Sound is currently playing.
   */
  bool isPlaying;

  /**
   * A Sound is currently paused.
   */
  bool isPaused;

  /**
   * The size of cone in degrees for a directional sound in which there will be
   * no attenuation.
   */
  Property<WeightedSound, float> directionalConeInnerAngle;

  /**
   * Size of cone in degrees for a directional sound outside of which there will
   * be no sound. Listener angles between innerAngle and outerAngle will falloff
   * linearly.
   */
  Property<WeightedSound, float> directionalConeOuterAngle;

  /**
   * Playback volume.
   */
  Property<WeightedSound, unsigned int> volume;

private:
  float _coneInnerAngle;
  float _coneOuterAngle;
  unsigned int _volume;

  std::vector<SoundPtr> _sounds;
  Float32Array _weights;
  std::optional<size_t> _currentIndex;

}; // end of class Sound

} // end of namespace BABYLON

#endif // end of BABYLON_AUDIO_WEIGHTED_SOUND_H
