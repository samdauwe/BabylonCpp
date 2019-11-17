#include <babylon/audio/audio_scene_component.h>

#include <babylon/audio/sound.h>
#include <babylon/audio/sound_track.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

AudioSceneComponent::AudioSceneComponent(Scene* iScene)
    : audioEnabled{this, &AudioSceneComponent::get_audioEnabled}
    , headphone{this, &AudioSceneComponent::get_headphone}
    , _audioEnabled{true}
    , _headphone{true}
{
  ISceneComponent::name = AudioSceneComponent::name;
  scene                 = iScene;
}

AudioSceneComponent::~AudioSceneComponent() = default;

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
  }
}

void AudioSceneComponent::removeFromContainer(AbstractScene& container,
                                              bool /*dispose*/)
{
  if (container.sounds.empty()) {
    return;
  }
  for (auto& sound : container.sounds) {
    sound->stop();
    sound->autoplay = false;
    scene->mainSoundTrack()->RemoveSound(sound);
  }
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

  for (auto& sound : scene->mainSoundTrack()->soundCollection) {
    sound->pause();
  }
  if (!scene->soundTracks.empty()) {
    for (auto& soundTrack : scene->soundTracks) {
      for (auto& sound : soundTrack->soundCollection) {
        sound->pause();
      }
    }
  }
}

void AudioSceneComponent::enableAudio()
{
  _audioEnabled = true;

  for (auto& sound : scene->mainSoundTrack()->soundCollection) {
    if (sound->isPaused) {
      sound->play();
    }
  }
  if (!scene->soundTracks.empty()) {
    for (auto& soundTrack : scene->soundTracks) {
      for (auto& sound : soundTrack->soundCollection) {
        if (sound->isPaused) {
          sound->play();
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
