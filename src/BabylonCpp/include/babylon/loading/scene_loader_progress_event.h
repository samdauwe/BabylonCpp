#ifndef BABYLON_LOADING_SCENE_LOADER_PROGRESS_EVENT_H
#define BABYLON_LOADING_SCENE_LOADER_PROGRESS_EVENT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class ProgressEvent;

/**
 * @brief Class used to represent data loading progression.
 */
class BABYLON_SHARED_EXPORT SceneLoaderProgressEvent {

public:
  /**
   * @brief Create a new progress event.
   * @param lengthComputable defines if data length to load can be evaluated
   * @param loaded defines the loaded data length
   * @param total defines the data length to load
   */
  SceneLoaderProgressEvent(bool lengthComputable = false, size_t loaded = 0,
                           size_t total = 0);
  SceneLoaderProgressEvent(const SceneLoaderProgressEvent& other);
  SceneLoaderProgressEvent(SceneLoaderProgressEvent&& other);
  SceneLoaderProgressEvent& operator=(const SceneLoaderProgressEvent& other);
  SceneLoaderProgressEvent& operator=(SceneLoaderProgressEvent&& other);
  ~SceneLoaderProgressEvent();

  /**
   * @brief Creates a new SceneLoaderProgressEvent from a ProgressEvent.
   * @param event defines the source event
   * @returns a new SceneLoaderProgressEvent
   */
  static SceneLoaderProgressEvent FromProgressEvent(const ProgressEvent& event);

protected:
  /**
   * @brief Gets the boolean flag indicating if the total work to be done, and
   * the amount of work already done, by the underlying process is calculable.
   * In other words, it tells if the progress is measurable or not.
   */
  bool get_lengthComputable() const;

  /**
   * @brief Gets the amount of work already performed by the underlying process.
   * The ratio of work done can be calculated with the property and
   * SceneLoaderProgressEvent.total.
   */
  size_t get_loaded() const;

  /**
   * @brief Gets the total amount of work that the underlying process is in the
   * progress of performing.
   */
  size_t get_total() const;

public:
  /**
   * defines if data length to load can be evaluated
   */
  ReadOnlyProperty<SceneLoaderProgressEvent, bool> lengthComputable;

  /**
   * defines the loaded data length
   */
  ReadOnlyProperty<SceneLoaderProgressEvent, size_t> loaded;

  /**
   * defines the data length to load
   */
  ReadOnlyProperty<SceneLoaderProgressEvent, size_t> total;

private:
  bool _lengthComputable;
  size_t _loaded;
  size_t _total;

}; // end of class SceneLoaderProgressEvent

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_PROGRESS_EVENT_H
