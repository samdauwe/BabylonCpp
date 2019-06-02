#ifndef BABYLON_AUDIO_SOUND_H
#define BABYLON_AUDIO_SOUND_H

#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/misc/observable.h>
#include <nlohmann/json_fwd.hpp>

using json = nlohmann::json;

namespace BABYLON {

class Scene;
class Sound;
using SoundPtr = std::shared_ptr<Sound>;

class BABYLON_SHARED_EXPORT Sound {

public:
  template <typename... Ts>
  static SoundPtr New(Ts&&... args)
  {
    return std::shared_ptr<Sound>(new Sound(std::forward<Ts>(args)...));
  }
  virtual ~Sound()
  {
  }

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
  static SoundPtr Parse(const json& /*parsedSound*/, Scene* /*scene*/,
                        const std::string& /*rootUrl*/,
                        const SoundPtr& /*sound*/ = nullptr)
  {
    return nullptr;
  }

protected:
  Sound(const std::string& /*name*/,
        const std::optional<std::variant<std::string, ArrayBuffer>>&
        /*urlOrArrayBuffer*/,
        Scene* /*scene*/)
  {
  }

public:
  bool isPaused;
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
