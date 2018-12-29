#include <babylon/audio/audio_scene_component.h>

#include <babylon/audio/sound.h>
#include <babylon/audio/sound_track.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

AudioSceneComponent::AudioSceneComponent(Scene* iScene)
    : audioEnabled{this, &AudioSceneComponent::get_audioEnabled}
    , headphone{this, &AudioSceneComponent::get_headphone}
    , _audioEnabled{true}
    , _headphone{true}
{
  scene = iScene;
}

AudioSceneComponent::~AudioSceneComponent()
{
}

bool AudioSceneComponent::get_audioEnabled() const
{
  return _audioEnabled;
}

bool AudioSceneComponent::get_headphone() const
{
  return _headphone;
}

void AudioSceneComponent::_register()
{
  scene->_afterRenderStage.registerStep(
    SceneComponentConstants::STEP_AFTERRENDER_AUDIO, this,
    [this]() { _afterRender(); });
}

void AudioSceneComponent::rebuild()
{
  // Nothing to do here. (Not rendering related)
}

void AudioSceneComponent::serialize(json& /*serializationObject*/)
{
}

void AudioSceneComponent::addFromContainer(AbstractScene& container)
{
  if (container.sounds.empty()) {
    return;
  }
  for (auto& sound : container.sounds) {
    sound->play();
    sound->autoplay = true;
    scene->mainSoundTrack()->AddSound(sound);
  };
}

void AudioSceneComponent::removeFromContainer(AbstractScene& container)
{
  if (container.sounds.empty()) {
    return;
  }
  for (auto& sound : container.sounds) {
    sound->stop();
    sound->autoplay = false;
    scene->mainSoundTrack()->RemoveSound(sound);
  };
}

void AudioSceneComponent::dispose()
{
  if (scene->mainSoundTrack()) {
    scene->mainSoundTrack()->dispose();
  }

  if (!scene->soundTracks.empty()) {
    for (auto& soundTrack : scene->soundTracks) {
      soundTrack->dispose();
    }
  }
}

void AudioSceneComponent::disableAudio()
{
  _audioEnabled = false;

  for (size_t i = 0; i < scene->mainSoundTrack()->soundCollection.size(); ++i) {
    scene->mainSoundTrack()->soundCollection[i]->pause();
  }
  if (!scene->soundTracks.empty()) {
    for (auto& soundTrack : scene->soundTracks) {
      for (size_t j = 0; j < soundTrack->soundCollection.size(); ++j) {
        soundTrack->soundCollection[j]->pause();
      }
    }
  }
}

void AudioSceneComponent::enableAudio()
{
  _audioEnabled = true;

  for (size_t i = 0; i < scene->mainSoundTrack()->soundCollection.size(); ++i) {
    if (scene->mainSoundTrack()->soundCollection[i]->isPaused) {
      scene->mainSoundTrack()->soundCollection[i]->play();
    }
  }
  if (!scene->soundTracks.empty()) {
    for (auto& soundTrack : scene->soundTracks) {
      for (size_t j = 0; j < soundTrack->soundCollection.size(); ++j) {
        if (soundTrack->soundCollection[j]->isPaused) {
          soundTrack->soundCollection[j]->play();
        }
      }
    }
  }
}

void AudioSceneComponent::switchAudioModeForHeadphones()
{
  _headphone = true;

  scene->mainSoundTrack()->switchPanningModelToHRTF();

  if (!scene->soundTracks.empty()) {
    for (auto& soundTrack : scene->soundTracks) {
      soundTrack->switchPanningModelToHRTF();
    }
  }
}

void AudioSceneComponent::switchAudioModeForNormalSpeakers()
{
  _headphone = false;

  scene->mainSoundTrack()->switchPanningModelToEqualPower();

  if (!scene->soundTracks.empty()) {
    for (auto& soundTrack : scene->soundTracks) {
      soundTrack->switchPanningModelToEqualPower();
    }
  }
}

void AudioSceneComponent::_afterRender()
{
}

} // end of namespace BABYLON
