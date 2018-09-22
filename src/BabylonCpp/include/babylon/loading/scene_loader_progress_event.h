#ifndef BABYLON_LOADING_SCENE_LOADER_PROGRESS_EVENT_H
#define BABYLON_LOADING_SCENE_LOADER_PROGRESS_EVENT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class ProgressEvent;

/**
 * @brief The SceneLoaderProgressEvent class represents events measuring
 * progress of an scene loading.
 */
class BABYLON_SHARED_EXPORT SceneLoaderProgressEvent {

public:
  /**
   * @brief Creates a SceneLoaderProgressEvent event with the given parameters.
   * @param lengthComputable indicates if the total work to be done, and the
   * amount of work already done, by the underlying process is calculable. In
   * other words, it tells if the progress is measurable or not. It defaults to
   * false.
   * @param loaded represents the amount of work already performed by the
   * underlying process. The ratio of work done can be calculated with the
   * property and SceneLoaderProgressEvent.total. When downloading a resource
   * using HTTP, this only represent the part of the content itself, not headers
   * and other overhead. It defaults to 0.
   * @param total represents the total amount of work that the underlying
   * process is in the progress of performing. When downloading a resource using
   * HTTP, this only represent the content itself, not headers and other
   * overhead. It defaults to 0.
   */
  SceneLoaderProgressEvent(bool lengthComputable = false, size_t loaded = 0,
                           size_t total = 0);
  SceneLoaderProgressEvent(const SceneLoaderProgressEvent& other);
  SceneLoaderProgressEvent(SceneLoaderProgressEvent&& other);
  SceneLoaderProgressEvent& operator=(const SceneLoaderProgressEvent& other);
  SceneLoaderProgressEvent& operator=(SceneLoaderProgressEvent&& other);
  ~SceneLoaderProgressEvent();

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
   * The Boolean flag indicating if the total work to be done, and the amount of
   * work already done, by the underlying process is calculable. In other words,
   * it tells if the progress is measurable or not.
   */
  ReadOnlyProperty<SceneLoaderProgressEvent, bool> lengthComputable;

  /**
   * The amount of work already performed by the underlying process. The ratio
   * of work done can be calculated with the property and
   * SceneLoaderProgressEvent.total.
   */
  ReadOnlyProperty<SceneLoaderProgressEvent, size_t> loaded;

  /**
   * The total amount of work that the underlying process is in the progress of
   * performing.
   */
  ReadOnlyProperty<SceneLoaderProgressEvent, size_t> total;

private:
  bool _lengthComputable;
  size_t _loaded;
  size_t _total;

}; // end of class SceneLoaderProgressEvent

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_SCENE_LOADER_PROGRESS_EVENT_H
