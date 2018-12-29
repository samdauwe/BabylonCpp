#ifndef BABYLON_AUDIO_AUDIO_SCENE_COMPONENT_H
#define BABYLON_AUDIO_AUDIO_SCENE_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/engine/iscene_serializable_component.h>
#include <babylon/engine/scene_component_constants.h>

namespace BABYLON {

class AudioSceneComponent;
using AudioSceneComponentPtr = std::shared_ptr<AudioSceneComponent>;

/**
 * @brief Defines the sound scene component responsible to manage any sounds in
 * a given scene.
 */
class BABYLON_SHARED_EXPORT AudioSceneComponent
    : public ISceneSerializableComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_AUDIO;

public:
  template <typename... Ts>
  static AudioSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<AudioSceneComponent>(
      new AudioSceneComponent(std::forward<Ts>(args)...));
  }
  virtual ~AudioSceneComponent();

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of
   * context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Serializes the component data to the specified json object
   * @param serializationObject The object to serialize to
   */
  void serialize(json& serializationObject) override;

  /**
   * @brief Adds all the element from the container to the scene.
   * @param container the container holding the elements
   */
  void addFromContainer(AbstractScene& container) override;

  /**
   * @brief Removes all the elements in the container from the scene.
   * @param container contains the elements to remove
   */
  void removeFromContainer(AbstractScene& container) override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

  /**
   * @brief Disables audio in the associated scene.
   */
  void disableAudio();

  /**
   * @brief Enables audio in the associated scene.
   */
  void enableAudio();

  /**
   * Switch audio to headphone output.
   */
  void switchAudioModeForHeadphones();

  /**
   * Switch audio to normal speakers.
   */
  void switchAudioModeForNormalSpeakers();

protected:
  /**
   * @brief Creates a new instance of the component for the given scene.
   * @param scene Defines the scene to register the component in
   */
  AudioSceneComponent(Scene* scene);

  /**
   * @brief Gets whether audio is enabled or not.
   * Please use related enable/disable method to switch state.
   */
  bool get_audioEnabled() const;

  /**
   * @brief Gets whether audio is outputing to headphone or not.
   * Please use the according Switch methods to change output.
   */
  bool get_headphone() const;

private:
  void _afterRender();

public:
  /**
   * Gets whether audio is enabled or not.
   * Please use related enable/disable method to switch state.
   */
  ReadOnlyProperty<AudioSceneComponent, bool> audioEnabled;

  /**
   * Gets whether audio is outputing to headphone or not.
   * Please use the according Switch methods to change output.
   */
  ReadOnlyProperty<AudioSceneComponent, bool> headphone;

private:
  bool _audioEnabled;
  bool _headphone;

}; // end of class AudioSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_AUDIO_AUDIO_SCENE_COMPONENT_H
